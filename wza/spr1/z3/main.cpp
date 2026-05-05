#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

struct Tuple2 {
    unsigned int coords[2];
    bool operator<=(const Tuple2& other) const {
        return coords[0] <= other.coords[0] && coords[1] <= other.coords[1];
    }
    bool operator==(const Tuple2& other) const {
        return coords[0] == other.coords[0] && coords[1] == other.coords[1];
    }
};

struct Tuple3 {
    unsigned int coords[3];
    bool operator<=(const Tuple3& other) const {
        return coords[0] <= other.coords[0] && coords[1] <= other.coords[1] && coords[2] <= other.coords[2];
    }
};

struct Tuple4 {
    unsigned int coords[4];
    bool operator<=(const Tuple4& other) const {
        return coords[0] <= other.coords[0] && coords[1] <= other.coords[1] && 
               coords[2] <= other.coords[2] && coords[3] <= other.coords[3];
    }
    bool operator==(const Tuple4& other) const {
        return coords[0] == other.coords[0] && coords[1] == other.coords[1] &&
               coords[2] == other.coords[2] && coords[3] == other.coords[3];
    }
};

template <typename T>
std::vector<T> get_minimal(const std::vector<T>& set) {
    std::vector<T> min_elems;
    for (size_t i = 0; i < set.size(); ++i) {
        bool is_min = true;
        for (size_t j = 0; j < set.size(); ++j) {
            if (i == j) continue;
            if (set[j] <= set[i] && !(set[i] <= set[j])) {
                is_min = false;
                break;
            }
        }
        if (is_min) {
            bool exists = false;
            for (const auto& m : min_elems) {
                if (m == set[i]) exists = true;
            }
            if (!exists) min_elems.push_back(set[i]);
        }
    }
    return min_elems;
}

int main() {
    cout << "--- ZADANIE 3 ---" << endl;
    Tuple2 t2_1 = {2, 8}; // (a,b)
    Tuple2 t2_2 = {3, 9}; // (c,d)
    Tuple2 t2_3 = {9, 9}; // (e,f)

    cout << "Pary: (2,8) <= (3,9): " << (t2_1 <= t2_2) << endl;
    cout << "Pary: (3,9) <= (9,9): " << (t2_2 <= t2_3) << endl;

    Tuple3 t3_1 = {2, 3, 9}; // (a,c,e)
    Tuple3 t3_2 = {8, 9, 9}; // (b,d,f)
    cout << "Trójki: (2,3,9) <= (8,9,9): " << (t3_1 <= t3_2) << endl;

    // Znajdź elementy minimalne w zbiorze A
    // A = {(x, y) z N^2: (x - 2)^2 + (y - 8)^2 <= 5}
    std::vector<Tuple2> set_A;
    for (int x = 0; x <= 10; ++x) {
        for (int y = 0; y <= 15; ++y) {
            if (pow(x - 2, 2) + pow(y - 8, 2) <= 5) {
                set_A.push_back({(unsigned int)x, (unsigned int)y});
            }
        }
    }
    auto min_A = get_minimal(set_A);
    cout << "\nElementy A:" << endl;
    for (auto a : set_A) cout << "(" << a.coords[0] << ", " << a.coords[1] << ") ";
    cout << "\nElementy minimalne A:" << endl;
    for (auto m : min_A) cout << "(" << m.coords[0] << ", " << m.coords[1] << ") ";
    cout << endl;

    // B = {(x,y,z,w) : (x-3)^2 + (y-9)^2 + (z-9)^2 + (w-9)^2 > 224}
    // Szukamy wszystkich elementów minimalnych z B ograniczając bounding boxa.
    std::vector<Tuple4> candidate_B;
    for (unsigned int x = 0; x <= 20; ++x) {
        for (unsigned int y = 0; y <= 20; ++y) {
            for (unsigned int z = 0; z <= 20; ++z) {
                for (unsigned int w = 0; w <= 20; ++w) {
                    double dist = pow((double)x - 3, 2) + pow((double)y - 9, 2) + pow((double)z - 9, 2) + pow((double)w - 9, 2);
                    if (dist > 224) {
                        candidate_B.push_back({x, y, z, w});
                    }
                }
            }
        }
    }
    
    auto min_B = get_minimal(candidate_B);
    cout << "\nSkończono szukanie. Wstępna lista kandydatów z pudła (20): " << candidate_B.size() << endl;
    cout << "Znalezione lokalne elementy minimalne B w N^4:" << endl;
    int count = 0;
    for (auto m : min_B) {
        cout << "(" << m.coords[0] << ", " << m.coords[1] << ", " << m.coords[2] << ", " << m.coords[3] << ") ";
        if (++count > 20) { cout << "... (" << min_B.size() << " łącznie)"; break; }
    }
    cout << endl;

    return 0;
}