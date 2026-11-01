// bignum.hpp - Aritmética de enteros grandes sobre dígitos en base 10.
//
// REPRESENTACIÓN (decisión de diseño documentada):
//   Un número es un std::vector<int> con UN dígito (0..9) por posición,
//   en orden "little-endian": A[0] es el dígito menos significativo.
//   Así, desplazar(A, k) equivale a insertar k ceros AL INICIO del vector
//   (los ceros "a la derecha" del número escrito normalmente).
//   Ningún tipo nativo se usa para el producto completo; solo se multiplican
//   dígitos individuales (<= 9*9) y se acumulan columnas en int.
#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

using Num = std::vector<int>;

// ---------- utilidades ----------
inline void normalizar(Num& a) {                 // quita ceros a la izquierda (extremo alto)
    while (a.size() > 1 && a.back() == 0) a.pop_back();
    if (a.empty()) a.push_back(0);
}

inline Num desdeCadena(const std::string& s) {   // "1234" -> {4,3,2,1}
    Num a;
    for (auto it = s.rbegin(); it != s.rend(); ++it) {
        if (*it < '0' || *it > '9') throw std::invalid_argument("cadena no numerica");
        a.push_back(*it - '0');
    }
    if (a.empty()) a.push_back(0);
    return a;                                    // NO normaliza: conserva ceros a la izquierda
}

inline std::string aCadena(Num a) {              // normaliza y devuelve "1234"
    normalizar(a);
    std::string s;
    for (auto it = a.rbegin(); it != a.rend(); ++it) s.push_back(char('0' + *it));
    return s;
}

inline bool esCero(const Num& a) {
    for (int d : a) if (d != 0) return false;
    return true;
}

// ---------- interfaz de la sección 4 ----------

// A + B. Admite longitudes distintas; el resultado puede tener un dígito más.
inline Num sumar(const Num& A, const Num& B) {
    size_t n = std::max(A.size(), B.size());
    Num R;
    R.reserve(n + 1);
    int acarreo = 0;
    for (size_t i = 0; i < n; ++i) {
        int s = acarreo;
        if (i < A.size()) s += A[i];
        if (i < B.size()) s += B[i];
        acarreo = (s >= 10);                     // s <= 19: evita división/módulo por dígito
        R.push_back(acarreo ? s - 10 : s);
    }
    if (acarreo) R.push_back(acarreo);
    return R;
}

// A - B con precondición A >= B (valor numérico). Resultado normalizado.
inline Num restar(const Num& A, const Num& B) {
    Num R;
    R.reserve(A.size());
    int prestamo = 0;
    for (size_t i = 0; i < A.size(); ++i) {
        int d = A[i] - prestamo - (i < B.size() ? B[i] : 0);
        if (d < 0) { d += 10; prestamo = 1; } else prestamo = 0;
        R.push_back(d);
    }
    // Si sobró préstamo o B tiene dígitos no nulos más allá de A, se violó A >= B.
    if (prestamo) throw std::logic_error("restar: se violo la precondicion A >= B");
    for (size_t i = A.size(); i < B.size(); ++i)
        if (B[i] != 0) throw std::logic_error("restar: se violo la precondicion A >= B");
    normalizar(R);
    return R;
}

// A * 10^k : añade k ceros a la derecha del número (al inicio del vector).
inline Num desplazar(const Num& A, size_t k) {
    if (esCero(A)) return Num{0};
    Num R;
    R.reserve(k + A.size());
    R.assign(k, 0);
    R.insert(R.end(), A.begin(), A.end());
    return R;
}

// Separa A en (alta, baja) respecto de la posición m: baja = A[0..m), alta = A[m..).
inline void partir(const Num& A, size_t m, Num& alta, Num& baja) {
    if (A.size() <= m) {
        baja = A;
        alta.assign(1, 0);
    } else {
        baja.assign(A.begin(), A.begin() + m);
        alta.assign(A.begin() + m, A.end());
    }
    normalizar(baja);
    normalizar(alta);
}

// Multiplicación escolar Θ(n^2): referencia correcta.
inline Num multiplicacionClasica(const Num& A, const Num& B) {
    std::vector<int> col(A.size() + B.size(), 0);   // columnas: sumas de productos de dígitos
    for (size_t i = 0; i < A.size(); ++i) {
        if (A[i] == 0) continue;
        for (size_t j = 0; j < B.size(); ++j) col[i + j] += A[i] * B[j];
    }
    Num R(col.size());
    int acarreo = 0;
    for (size_t k = 0; k < col.size(); ++k) {
        int v = col[k] + acarreo;
        R[k] = v % 10;
        acarreo = v / 10;
    }
    // acarreo final = 0: el producto cabe en |A|+|B| dígitos
    normalizar(R);
    return R;
}

// ---------- Karatsuba ----------
// 'umbral': si max(|A|,|B|) <= umbral se usa multiplicación directa (caso base).
// Con umbral = 1 es el Karatsuba "puro" pedido (caso base de 1 dígito).
inline Num karatsubaRec(const Num& A, const Num& B, size_t umbral) {
    size_t n = std::max(A.size(), B.size());
    if (n <= umbral) {
        if (n == 1) { int p = A[0] * B[0]; return Num{ p % 10, p / 10 }; } // 1 dígito x 1 dígito
        return multiplicacionClasica(A, B);
    }
    size_t m = (n + 1) / 2;                       // posición de corte (n/2 si n es par)

    Num x1, x0, y1, y0;
    partir(A, m, x1, x0);
    partir(B, m, y1, y0);

    Num P1 = karatsubaRec(x1, y1, umbral);                         // x1*y1
    Num P2 = karatsubaRec(x0, y0, umbral);                         // x0*y0
    Num P3 = karatsubaRec(sumar(x1, x0), sumar(y1, y0), umbral);   // (x1+x0)(y1+y0)

    Num medio = restar(restar(P3, P1), P2);       // orden de la sección 3: (P3-P1)-P2, siempre >= 0

    Num R = sumar(sumar(desplazar(P1, 2 * m), desplazar(medio, m)), P2);
    normalizar(R);
    return R;
}

// Envoltorio: rellena con ceros a la izquierda hasta una MISMA potencia de 2
// (sección 3) y llama a la recursión. Los intermedios pueden tener longitud m+1.
inline Num karatsuba(const Num& A, const Num& B, size_t umbral = 1) {
    size_t n = std::max(A.size(), B.size()), p = 1;
    while (p < n) p <<= 1;
    Num a = A, b = B;
    a.resize(p, 0);
    b.resize(p, 0);
    Num R = karatsubaRec(a, b, umbral);
    normalizar(R);
    return R;
}
