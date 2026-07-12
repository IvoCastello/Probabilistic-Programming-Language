#include "vm.h"
#include "parser.h"
#include "primitives.h"
#include <variant>

namespace ppl {

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

Machine initial_machine(const std::string& program, std::mt19937& rng) {
    auto forms = parse(program);
    auto genv = std::make_shared<Env>();
    Expr main;
    bool has_main = false;
    for (auto& form : forms) {
        if (form.is_list() && form.size() > 0 && form[0].is_symbol() && form[0].as_symbol() == "defn") {
            auto& items = form.as_list().items;
            Symbol name = items[1].as_symbol();
            std::vector<Symbol> params;
            if (items[2].is_list()) {
                for (auto& p : items[2].as_list().items) params.push_back(p.as_symbol());
            }
            std::vector<Expr> body;
            for (size_t i = 3; i < items.size(); ++i) body.push_back(items[i]);
            (*genv)[name] = std::make_shared<Closure>(std::move(params), std::move(body), genv);
        } else {
            main = form;
            has_main = true;
        }
    }
    std::vector<Instruction> C;
    C.push_back(Instruction{Instruction::Ev{main, *genv, Address{}}});
    return Machine(std::move(C), {}, *genv, &rng, 0.0);
}

void push_body(std::vector<Instruction>& C, const std::vector<Expr>& body,
               const Env& env, const Address& addr) {
    std::vector<Instruction> seq;
    for (size_t n = 0; n + 1 < body.size(); ++n) {
        seq.push_back(Instruction{Instruction::Ev{body[n], env, addr + int(n)}});
        seq.push_back(Instruction{Instruction::Discard{}});
    }
    if (!body.empty()) {
        seq.push_back(Instruction{Instruction::Ev{body.back(), env,
            addr + int(body.size() - 1)}});
    }
    for (auto it = seq.rbegin(); it != seq.rend(); ++it) C.push_back(*it);
}

Message resume(Machine& m) {
    auto& C = m.C;
    auto& V = m.V;

    while (!C.empty()) {
        auto instr = std::move(C.back());
        C.pop_back();

        if (std::holds_alternative<Instruction::SampleK>(instr.data)) {
            Value d = V.back(); V.pop_back();
            return Message{SampleMsg{std::get<Instruction::SampleK>(instr.data).addr, d}};
        }
        if (std::holds_alternative<Instruction::ObserveK>(instr.data)) {
            double y = V.back().as_number(); V.pop_back();
            Value d = V.back(); V.pop_back();
            return Message{ObserveMsg{std::get<Instruction::ObserveK>(instr.data).addr, d, y}};
        }

        std::visit(overloaded{
            [&](const Instruction::Ev& ev) {
                std::visit(overloaded{
                    [&](double lit) { V.push_back(Value(lit)); },
                    [&](const Symbol& sym) {
                        if (ev.env.count(sym)) {
                            V.push_back(ev.env.at(sym));
                        } else if (is_primitive(sym)) {
                            auto it = PRIMITIVES.find(sym);
                            if (it != PRIMITIVES.end()) V.push_back(it->second);
                        }
                    },
                    [&](const std::shared_ptr<Expr::List>& list) {
                        const auto& e = list->items;
                        std::visit(overloaded{
                            [&](const Symbol& head) {
                                if (head == "let") {
                                    auto binds_expr = std::get<std::shared_ptr<Expr::List>>(e[1].data);
                                    auto& binds = binds_expr->items;
                                    std::vector<Expr> body;
                                    for (size_t i = 2; i < e.size(); ++i) body.push_back(e[i]);
                                    if (!binds.empty()) {
                                        C.push_back(Instruction{Instruction::LetK{binds, 0, body, ev.env, ev.addr}});
                                        C.push_back(Instruction{Instruction::Ev{binds[1], ev.env, ev.addr + std::string("let")}});
                                    } else {
                                        push_body(C, body, ev.env, ev.addr);
                                    }
                                }
                                else if (head == "if") {
                                    C.push_back(Instruction{Instruction::IfK{e[2], e[3], ev.env, ev.addr}});
                                    C.push_back(Instruction{Instruction::Ev{e[1], ev.env, ev.addr + std::string("test")}});
                                }
                                else if (head == "fn") {
                                    auto params_expr = std::get<std::shared_ptr<Expr::List>>(e[1].data);
                                    std::vector<Symbol> params;
                                    for (auto& p : params_expr->items) params.push_back(p.as_symbol());
                                    std::vector<Expr> body;
                                    for (size_t i = 2; i < e.size(); ++i) body.push_back(e[i]);
                                    V.push_back(Value(std::make_shared<Closure>(std::move(params), std::move(body), std::make_shared<Env>(ev.env))));
                                }
                                else if (head == "sample") {
                                    C.push_back(Instruction{Instruction::SampleK{ev.addr}});
                                    C.push_back(Instruction{Instruction::Ev{e[1], ev.env, ev.addr + std::string("d")}});
                                }
                                else if (head == "observe") {
                                    C.push_back(Instruction{Instruction::ObserveK{ev.addr}});
                                    C.push_back(Instruction{Instruction::Ev{e[2], ev.env, ev.addr + std::string("v")}});
                                    C.push_back(Instruction{Instruction::Ev{e[1], ev.env, ev.addr + std::string("d")}});
                                }
                                else {
                                    size_t n = e.size() - 1;
                                    C.push_back(Instruction{Instruction::CallK{n, ev.addr}});
                                    for (int i = (int)n; i >= 1; --i) {
                                        C.push_back(Instruction{Instruction::Ev{e[i], ev.env, ev.addr + int(i-1)}});
                                    }
                                    C.push_back(Instruction{Instruction::Ev{e[0], ev.env, ev.addr + std::string("fn")}});
                                }
                            },
                            [&](auto&) {
                                size_t n = e.size() - 1;
                                C.push_back(Instruction{Instruction::CallK{n, ev.addr}});
                                for (int i = (int)n; i >= 1; --i) {
                                    C.push_back(Instruction{Instruction::Ev{e[i], ev.env, ev.addr + int(i-1)}});
                                }
                                C.push_back(Instruction{Instruction::Ev{e[0], ev.env, ev.addr + std::string("fn")}});
                            }
                        }, e[0].data);
                    }
                }, ev.expr.data);
            },

            [&](const Instruction::LetK& k) {
                Env new_env = k.env;
                new_env[k.binds[2*k.i].as_symbol()] = V.back();
                V.pop_back();
                if (2*(k.i + 1) < k.binds.size()) {
                    C.push_back(Instruction{Instruction::LetK{k.binds, k.i + 1, k.body, new_env, k.addr}});
                    C.push_back(Instruction{Instruction::Ev{k.binds[2*(k.i+1)+1], new_env, k.addr + int(2*(k.i+1))}});
                } else {
                    push_body(C, k.body, new_env, k.addr);
                }
            },

            [&](const Instruction::IfK& k) {
                bool cond = V.back().as_number() != 0.0;
                V.pop_back();
                auto branch = cond ? k.then_branch : k.else_branch;
                auto tag = cond ? std::string("then") : std::string("else");
                C.push_back(Instruction{Instruction::Ev{branch, k.env, k.addr + tag}});
            },

            [&](const Instruction::Discard&) { V.pop_back(); },

            [&](const Instruction::CallK& k) {
                std::vector<Value> args;
                for (size_t i = 0; i < k.n; ++i) { args.push_back(V.back()); V.pop_back(); }
                std::reverse(args.begin(), args.end());
                Value f = V.back(); V.pop_back();
                if (f.is_closure()) {
                    auto& clos = f.as_closure();
                    Env new_env = *clos->env;
                    for (size_t i = 0; i < clos->params.size(); ++i)
                        new_env[clos->params[i]] = args[i];
                    push_body(C, clos->body, new_env, k.addr);
                } else if (f.is_primitive()) {
                    V.push_back(f.as_primitive()(args));
                }
            },

            [&](const Instruction::SampleK&) {},
            [&](const Instruction::ObserveK&) {},
        }, instr.data);
    }

    return Message{DoneMsg{V.back()}};
}

} // namespace ppl