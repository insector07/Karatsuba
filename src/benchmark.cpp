// benchmark.cpp - Mide clásico vs Karatsuba para n = 2, 4, ..., 4096 dígitos.
// Uso: ./benchmark [umbral_hibrido] [ext]
//   umbral_hibrido: caso base del Karatsuba hibrido (por defecto 128)
//   ext: ademas mide 8192..32768 digitos (solo clasica e hibrido) fuera del rango orientativo
// Salida: tabla por pantalla y results/tiempos.csv (si se ejecuta desde la raíz del repo).
#include "bignum.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <random>
#include <iostream>
#include <string>

using reloj = std::chrono::steady_clock;

static volatile unsigned long long sumidero = 0;   // consume el resultado: evita que el compilador elimine el cálculo

static Num aleatorio(std::mt19937& g, size_t n) {
    Num a(n);
    for (auto& d : a) d = g() % 10;
    if (a.back() == 0) a.back() = 1;
    return a;
}

template <class F>
static double medirMicros(F f, double minSegundos = 0.2) {
    // Calibra repeticiones para que el tiempo total supere con holgura la resolución del reloj.
    size_t reps = 1;
    for (;;) {
        auto t0 = reloj::now();
        for (size_t i = 0; i < reps; ++i) {
            Num r = f();
            sumidero += r[0] + r.size();
        }
        double s = std::chrono::duration<double>(reloj::now() - t0).count();
        if (s >= minSegundos) return s / reps * 1e6;
        reps *= (s < 0.01) ? 10 : 2;
    }
}

int main(int argc, char** argv) {
    size_t umbralH = argc > 1 ? std::strtoul(argv[1], nullptr, 10) : 128;
    bool ext = argc > 2 && std::string(argv[2]) == "ext";
    std::mt19937 g(2026);

    std::ofstream csv("results/tiempos.csv");
    csv << "n,clasica_us,karatsuba_us,karatsuba_hibrido_us\n";
    std::printf("%6s %14s %14s %14s %10s\n", "n", "clasica(us)", "karatsuba(us)", "hibrido(us)", "clas/hib");

    for (size_t n = 2; n <= (ext ? 32768u : 4096u); n *= 2) {
        Num A = aleatorio(g, n), B = aleatorio(g, n);
        // verificación previa de correctitud en cada tamaño
        if (multiplicacionClasica(A, B) != karatsuba(A, B, umbralH)) { std::fprintf(stderr, "ERROR n=%zu\n", n); return 1; }

        double tc = medirMicros([&] { return multiplicacionClasica(A, B); });
        double th = medirMicros([&] { return karatsuba(A, B, umbralH); });
        if (n <= 4096) {
            double tk = medirMicros([&] { return karatsuba(A, B, 1); });
            std::printf("%6zu %14.3f %14.3f %14.3f %10.3f\n", n, tc, tk, th, tc / th);
            csv << n << "," << tc << "," << tk << "," << th << "\n";
        } else {   // Karatsuba puro omitido: tardaria demasiado y ya se sabe que va por detras
            std::printf("%6zu %14.3f %14s %14.3f %10.3f\n", n, tc, "-", th, tc / th);
            csv << n << "," << tc << ",," << th << "\n";
        }
    }

    // Barrido del umbral del caso base (n = 4096): cuanto cuesta/ayuda cortar la recursion antes.
    std::ofstream csv2("results/umbral.csv");
    csv2 << "umbral,karatsuba_4096_us\n";
    std::printf("\nBarrido de umbral para n = 4096 (clasica = referencia):\n%8s %14s\n", "umbral", "tiempo(us)");
    {
        Num A = aleatorio(g, 4096), B = aleatorio(g, 4096);
        for (size_t u : {1u, 2u, 4u, 8u, 16u, 32u, 64u, 128u, 256u, 512u}) {
            double t = medirMicros([&] { return karatsuba(A, B, u); });
            std::printf("%8zu %14.3f\n", u, t);
            csv2 << u << "," << t << "\n";
        }
    }
    std::printf("\n(umbral del Karatsuba hibrido = %zu digitos; checksum=%llu)\n", umbralH, (unsigned long long)sumidero);
    return 0;
}
