// calc.cpp - Multiplica dos enteros dados por linea de comandos con ambos algoritmos.
// Uso: ./bin/calc 123456789 987654321      ->  imprime: <producto clasico> <producto karatsuba>
#include "bignum.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 3) { std::cerr << "Uso: " << argv[0] << " <A> <B>\n"; return 1; }
    Num A = desdeCadena(argv[1]), B = desdeCadena(argv[2]);
    std::cout << aCadena(multiplicacionClasica(A, B)) << " " << aCadena(karatsuba(A, B)) << "\n";
    return 0;
}
