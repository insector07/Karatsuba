# Práctica 2 — Multiplicación rápida de enteros: Karatsuba

**Curso:** Análisis y Diseño de Algoritmos · **Tema:** Divide y Vencerás — Recurrencias y Teorema Maestro
**Lenguaje:** C++17 · **Integrante:** Juan Fernando Gómez Rivas

Implementación de la multiplicación clásica Θ(n²) y de Karatsuba Θ(n^log₂3) sobre enteros representados como dígitos en base 10, con validación, medición de tiempos e informe.

## Estructura

```
src/bignum.hpp        Biblioteca: sumar, restar, desplazar, partir, multiplicacionClasica, karatsuba
src/benchmark.cpp     Experimentos de tiempo (steady_clock, repeticiones calibradas)
src/calc.cpp          Utilidad: multiplica dos números dados por línea de comandos
tests/test_karatsuba.cpp         Validación Karatsuba vs. clásica (casos de borde + aleatorios)
tests/verificar_con_python.py    Verificación independiente contra los enteros de Python
results/              tiempos.csv, umbral.csv, salida_benchmark.txt y gráficas (.png)
docs/                 informe.docx, informe.pdf y declaración de uso de IA (uso_ia.md)
Makefile
```

## Compilar y ejecutar

Requisitos: `g++` con soporte C++17 y `make` (Linux/macOS; en Windows, MSYS2/MinGW o WSL).

```bash
make            # compila bin/test, bin/benchmark y bin/calc
make test       # ejecuta las pruebas de correctitud
make bench      # ejecuta el benchmark (rango 2..4096 dígitos; escribe results/*.csv)
./bin/benchmark 128 ext        # igual, y además mide 8192..32768 (solo clásica e híbrido)
./bin/calc 1234 5678           # imprime el producto con ambos algoritmos
python3 results/graficar.py    # regenera las gráficas (requiere matplotlib)
python3 tests/verificar_con_python.py   # verificación independiente (requiere bin/calc)
```

Sin `make`:
```bash
g++ -std=c++17 -O2 -o test tests/test_karatsuba.cpp && ./test
g++ -std=c++17 -O2 -o benchmark src/benchmark.cpp && ./benchmark
```

## Decisiones de diseño

- **Representación:** `std::vector<int>`, un dígito (0–9) por posición, orden *little-endian* (índice 0 = dígito menos significativo). Así `desplazar(A, k)` inserta k ceros al inicio del vector.
- **Sin tipos nativos** para el producto completo: solo se multiplican dígitos sueltos (≤ 9·9) y se acumulan columnas de la multiplicación clásica.
- **Relleno:** `karatsuba()` rellena ambos operandos con ceros hasta una misma potencia de 2. Las funciones auxiliares admiten longitudes variables (la suma de mitades puede dar m+1 dígitos) y normalizan ceros a la izquierda.
- **Resta sin signo:** el término central se calcula como `(P3 − P1) − P2`; ambas restas son ≥ 0 porque `P3 − P1 = P2 + x1·y0 + x0·y1 ≥ P2`. `restar` lanza excepción si se viola la precondición A ≥ B.
- **Caso base:** parámetro `umbral`. Con `umbral = 1` es el Karatsuba puro (1 dígito, multiplicación directa). Con umbral mayor (p. ej. 128) es la variante híbrida usada en el análisis de rendimiento.

## Resultados (resumen)

Ver `results/` y el informe. En el rango 2–4096 el Karatsuba **puro** no supera al clásico (constantes ocultas y asignaciones de memoria); el **híbrido** (caso base de 128 dígitos) lo supera a partir de ≈ 256–512 dígitos.

## Declaración de uso de IA

Ver `docs/uso_ia.md`.
