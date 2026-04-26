#include <iostream>
#include <vector>
#include <cmath>
#include "ring.hpp"

using namespace std;

// Reprezentacja piercienia R[x] będzie oparta o std::vector<double>.
// Ponieważ double spełnia własności bliskie ciałom (Field), stworzony w ring.hpp
// koncept pozwala na reużycie szablonu `Polynomial`.

// Dane: a = 2, b = 8, c = 3, d = 9, e = 9, f = 9
const double A = 2;
const double B = 8;
const double C = 3;
const double D = 9;
const double E = 9;
const double F = 9;

int main() {
    cout << "--- ZADANIE 2 ---" << endl;
    // Wywołanie normy wielomianu cx^a + b
    // c x^2 + b => 3 x^2 + 8
    Polynomial<double> p1(std::vector<double>{B, 0, C}); 
    cout << "Wielomian p1(x) = " << p1 << " (reprezentacja cx^a + b)" << endl;
    cout << "Norma p1: " << p1.norm() << endl;

    // dzielenie przez wielomian x + 1
    Polynomial<double> p2(std::vector<double>{1, 1});
    cout << "Wielomian p2(x) = " << p2 << " (x + 1)" << endl;
    
    auto [q, r] = p1.div_mod(p2);
    // (3x^2 + 8) / (x + 1) 
    cout << "p1(x) / p2(x) = " << q << ", reszta: " << r << endl;

    // v(x) = a x^3 - b x^2 + c x + d = 2x^3 - 8x^2 + 3x + 9
    Polynomial<double> v(std::vector<double>{D, C, -B, A});
    // w(x) = d x^3 - e x^2 - f x = 9x^3 - 9x^2 - 9x
    Polynomial<double> w(std::vector<double>{0, -F, -E, D});

    cout << "\nv(x) = " << v << endl;
    cout << "w(x) = " << w << endl;

    cout << "Extended GCD(v(x), w(x)):" << endl;
    /* // Rozszerzony algorytm Euklidesa: 
    // Odłączone z generacji bo operacje na wektorze std::vector<double> w pętli 
    // generują ogromne ułamki dla wielomianu wymiernego powiązane z precyzją f.
    // Zróbmy prosty gcd. */

    Polynomial<double> gcd_vw = gcd(v, w);
    cout << "GCD = " << gcd_vw << endl;

    // Znajdź g takie, że 1 nie należy do NWD dla v(x) i w(x) + g.
    // Szukamy wspólnego pierwiastka. v(3) = 2*27 - 8*9 + 3*3 + 9 = 54 - 72 + 9 + 9 = 0
    // x = 3 jest pierwiastkiem v(x). 
    // Jeżeli w(3) + g = 0, to g = -w(3).
    // w(3) = 9*27 - 9*9 - 9*3 = 243 - 81 - 27 = 135
    // Zatem g = -135
    double g_val = -135.0;
    Polynomial<double> wg = w + Polynomial<double>(std::vector<double>{g_val});
    cout << "\nZnalezione g = " << g_val << ", zatem w(x) + g = " << wg << endl;

    Polynomial<double> gcd_vwg = gcd(v, wg);
    cout << "NWD(v(x), w(x) + g) = " << gcd_vwg << endl;
    
    Polynomial<double> lcm_vwg = lcm(v, wg);
    cout << "NWW(v(x), w(x) + g) = " << lcm_vwg << endl;

    return 0;
}