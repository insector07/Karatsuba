#include "../src/bignum.hpp"
#include <iostream>
#include <random>
#include <cstdlib>

static int fallos = 0, total = 0;

static void comprobar(const std::string& nombre, const std::string& a, const std::string& b,
                      size_t umbral = 1) {
    Num A = desdeCadena(a), B = desdeCadena(b);
    std::string c = aCadena(multiplicacionClasica(A, B));
    std::string k = aCadena(karatsuba(A, B, umbral));
    ++total;
    if (c != k) {
        ++fallos;
        std::cout << "FALLO [" << nombre << "] umbral=" << umbral << "\n  clasica  : " << c.substr(0, 60)
                  << "...\n  karatsuba: " << k.substr(0, 60) << "...\n";
    } else {
        std::cout << "ok    [" << nombre << "]\n";
    }
}

static std::string aleatorio(std::mt19937& g, size_t n, bool dispersos = false) {
    std::string s;
    for (size_t i = 0; i < n; ++i) {
        int d = g() % 10;
        if (dispersos && g() % 3) d = 0;
        s.push_back(char('0' + d));
    }
    if (!s.empty() && s[0] == '0' && !dispersos) s[0] = '1';
    return s;
}

int main() {
    std::mt19937 g(12345);

    // 1 dígito (las combinaciones)
    for (int a = 0; a <= 9; ++a)
        for (int b = 0; b <= 9; ++b)
            comprobar("1 digito " + std::to_string(a) + "x" + std::to_string(b),
                      std::string(1, char('0' + a)), std::string(1, char('0' + b)));

    // valores conocidos calculados
    ++total;
    if (aCadena(karatsuba(desdeCadena("1234"), desdeCadena("5678"))) != "7006652") { ++fallos; std::cout << "FALLO 1234*5678\n"; }
    else std::cout << "ok    [1234*5678 = 7006652]\n";
    ++total;
    if (aCadena(karatsuba(desdeCadena("99999999"), desdeCadena("99999999"))) != "9999999800000001") { ++fallos; std::cout << "FALLO 99999999^2\n"; }
    else std::cout << "ok    [99999999^2 = 9999999800000001]\n";

    // cero y uno
    comprobar("por cero", "123456789", "0");
    comprobar("cero por cero", "0", "0");
    comprobar("por uno", "987654321987654321", "1");

    // muchos ceros a la izquierda
    comprobar("ceros izquierda", "000000000000123", "0000000000000000000456");
    comprobar("solo ceros", "0000000000000000", "0000000000000000");

    // ceros intercalados
    comprobar("ceros intercalados 1", "100000000000000000001", "10000000000000000000000000001");
    comprobar("ceros intercalados 2", "1000200030004", "9000800070006");
    comprobar("potencias de 10", "1000000000000", "1000000000000");

    // todos nueves 
    comprobar("nueves 64", std::string(64, '9'), std::string(64, '9'));
    comprobar("nueves 127 x 128", std::string(127, '9'), std::string(128, '9'));
    comprobar("nueves 33", std::string(33, '9'), std::string(33, '9'));

    // longitudes inpares y distintas entre ellos
    for (size_t n : {3u, 5u, 7u, 9u, 13u, 21u, 33u, 63u, 65u, 101u})
        comprobar("impar n=" + std::to_string(n), aleatorio(g, n), aleatorio(g, n));
    comprobar("longitudes distintas 3 vs 100", aleatorio(g, 3), aleatorio(g, 100));
    comprobar("longitudes distintas 1 vs 257", aleatorio(g, 1), aleatorio(g, 257));

    // productos grandes
    comprobar("grande 500 digitos", aleatorio(g, 500), aleatorio(g, 500));
    comprobar("grande 1000 digitos", aleatorio(g, 1000), aleatorio(g, 1000));
    comprobar("grande 3000 digitos (con muchos ceros)", aleatorio(g, 3000, true), aleatorio(g, 3000, true));

    // aleatorio masivo
    int malos = 0;
    for (size_t umbral : {1u, 4u, 16u, 64u})
        for (int t = 0; t < 300; ++t) {
            size_t na = 1 + g() % 200, nb = 1 + g() % 200;
            Num A = desdeCadena(aleatorio(g, na, t % 3 == 0)), B = desdeCadena(aleatorio(g, nb, t % 4 == 0));
            ++total;
            if (aCadena(multiplicacionClasica(A, B)) != aCadena(karatsuba(A, B, umbral))) {
                ++fallos; ++malos;
                std::cout << "FALLO aleatorio na=" << na << " nb=" << nb << " umbral=" << umbral << "\n";
            }
        }
    std::cout << (malos ? "FALLO" : "ok   ") << " [1200 productos aleatorios, umbrales 1/4/16/64]\n";

    std::cout << "\nResultado: " << (total - fallos) << "/" << total << " pruebas correctas\n";
    return fallos ? EXIT_FAILURE : EXIT_SUCCESS;
}
