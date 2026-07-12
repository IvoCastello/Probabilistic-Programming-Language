# PPL-C++: Probabilistic Programming Language in C++

Implementación en C++ de un lenguaje de programación probabilística basado en el libro **"An Introduction to Probabilistic Programming"** de van de Meent, Paige, Yang y Wood.
Este proyecto realizado durante la cursada de "Introducción a los Lenguajes de Programación Probabilísticos" bajo el docente Javier Burroni demuestra los conceptos fundamentales de PPLs con un intérprete de estilo Lisp que soporta inferencia bayesiana mediante múltiples algoritmos de muestreo.

## Características del Lenguaje

### Sintaxis
El lenguaje utiliza sintaxis de tipo S-expression con las siguientes construcciones:

| Forma                                 | Descripción               | Ejemplo                        |
| `(let [v1 e1 ...] body...)`           | Ligaduras locales         | `(let [x 10] (+ x 5))`         |
| `(if cond then else)`                 | Condicional               | `(if (&gt; x 0) 1 0)`          |
| `(fn [params...] body...)`            | Funciones anónimas        | `(fn [x] (* x 2))`             |
| `(defn name [params...] body...)`     | Definición de funciones   | `(defn sq [x] (* x x))`        |
| `(sample dist)`                       | Muestreo de prior         | `(sample (normal 0 1))`        |
| `(observe dist value)`                | Condicionamiento          | `(observe (normal mu 1) 2.3)`  |
| `(primitive args...)`                 | Llamadas primitivas       | `(+ 1 2)`, `(* a b)`           |

### Distribuciones Soportadas
- **Normal**: `(normal μ σ)` — distribución gaussiana
- **Bernoulli**: `(bernoulli p)` — distribución de Bernoulli

### Primitivas Aritméticas
`+`, `-`, `*`, `/`, `==`, `&lt;`, `&gt;`

## Algoritmos de Inferencia Implementados

|Algoritmo                            |Función                               | Descripción                                      |
| **Likelihood Weighting**            | `run_lw()`, `likelihood_weighting()` | Importance sampling con prior como proposal      |
| **Sequential Monte Carlo**          | `run_smc()`                          | Particle filtering con resampling en observes    |
| **Single-Site Metropolis-Hastings** | `single_site_mh()`                   | MCMC con propuestas de un solo sitio             |

## Arquitectura del Proyecto
main.cpp
ppl/
expr.{h,cpp}          # Expresiones del lenguaje
parser.{h,cpp}        # Parser de S-expressions
value.{h,cpp}         # Sistema de valores
distribution.{h,cpp}  # Distribuciones
primitives.{h,cpp}    # Funciones primitivas
address.{h,cpp}       # Direccionamiento de variables aleatorias
instruction.h         # Instrucciones de la máquina virtual
machine.{h,cpp}       # Estado de la máquina de inferencia
message.h             # Protocolo de mensajes
vm.{h,cpp}            # Máquina virtual
inference.{h,cpp}     # Algoritmos de inferencia bayesiana

## Compilación y Ejecución

### Requisitos
- Compilador C++17

### Compilación Manual

### Compilar todos los archivos fuente
g++ -std=c++17 -O2 -I. ppl/address.cpp ppl/expr.cpp ppl/distribution.cpp ppl/value.cpp ppl/machine.cpp ppl/parser.cpp ppl/primitives.cpp ppl/vm.cpp ppl/inference.cpp main.cpp -o ppl_runner

# Ejecutar
./ppl_runner

# Salida Esperada
closure: (f 3) = 13 (expect 13)
geom mean = 2.34002   exact (1-p)/p = 2.33333
LW   mean = 1.15791  (exact 1.150)
SSMH mean = 1.16805  std = 0.714711  (exact 1.150, 0.707107)
bits SSMH mean = 4.99833
SMC  mean = 1.14198

All tests passed!
