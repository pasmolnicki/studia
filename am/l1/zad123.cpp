#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

struct Node {
  int id;
  float x, y;
};

std::vector<std::vector<int>> distMatrix;

// Wczytywanie
std::vector<Node> readTSP(const std::string &filename) {
  std::ifstream file(filename);
  std::vector<Node> nodes;
  std::string line;
  bool reading = false;

  while (std::getline(file, line)) {
    if (line.find("NODE_COORD_SECTION") != std::string::npos) {
      reading = true;
      continue;
    }
    if (line.find("EOF") != std::string::npos)
      break;

    if (reading) {
      std::stringstream ss(line);
      Node n;
      if (ss >> n.id >> n.x >> n.y)
        nodes.push_back(n);
    }
  }
  return nodes;
}

// matryca odleglosci miedzy miastami
void buildDistanceMatrix(const std::vector<Node> &nodes) {
  int n = nodes.size();
  distMatrix.assign(n, std::vector<int>(n));

  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      float dx = nodes[i].x - nodes[j].x;
      float dy = nodes[i].y - nodes[j].y;
      int d = (int)round(sqrt(dx * dx + dy * dy));
      distMatrix[i][j] = distMatrix[j][i] = d;
    }
  }
}

// dlugosc drogi
int calculateTourLength(const std::vector<int> &route) {
  int n = route.size(), sum = 0;
  for (int i = 0; i < n - 1; i++)
    sum += distMatrix[route[i]][route[i + 1]];
  sum += distMatrix[route[n - 1]][route[0]];
  return sum;
}

// nowa permutacja
void shuffleRoute(std::vector<int> &route) {
  int n = route.size();
  for (int i = n - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    std::swap(route[i], route[j]);
  }
}

// zmiana na stringa
std::string routeToString(const std::vector<int> &route) {
  std::stringstream ss;
  for (int i = 0; i < route.size(); i++) {
    ss << route[i];
    if (i != route.size() - 1)
      ss << "-";
  }
  return ss.str();
}

// INVERT
void applyInvert(std::vector<int> &route, int i, int j) {
  while (i < j) {
    std::swap(route[i], route[j]);
    i++;
    j--;
  }
}

int calculateInvertDelta(const std::vector<int> &route, int i, int j) {
  int n = route.size();
  int im1 = (i - 1 + n) % n;
  int jp1 = (j + 1) % n;

  int oldCost =
      distMatrix[route[im1]][route[i]] + distMatrix[route[j]][route[jp1]];
  int newCost =
      distMatrix[route[im1]][route[j]] + distMatrix[route[i]][route[jp1]];

  return newCost - oldCost;
}

// TRANSPOSE
void applyTranspose(std::vector<int> &route, int i, int j) {
  std::swap(route[i], route[j]);
}

int calculateTransposeDelta(const std::vector<int> &route, int i, int j) {
  int n = route.size();

  int im1 = (i - 1 + n) % n;
  int ip1 = (i + 1) % n;
  int jm1 = (j - 1 + n) % n;
  int jp1 = (j + 1) % n;

  int a = route[i], b = route[j];
  int delta = 0;
  int newCost = 0;
  int oldCost = 0;
  if (i + 1 == j || (i == 0 && j == n - 1)) {

    int firstIdx = i, secondIdx = j;
    if (i == 0 && j == n - 1) {
      firstIdx = j;
      secondIdx = i;
    }

    int prev = (firstIdx - 1 + n) % n;
    int next = (secondIdx + 1) % n;

    // Odejmowanie starych krawędzi zewnętrznych
    delta -= distMatrix[route[prev]][route[firstIdx]];
    delta -= distMatrix[route[secondIdx]][route[next]];
    oldCost = distMatrix[route[prev]][route[firstIdx]] +
              distMatrix[route[secondIdx]][route[next]];

    // Dodawanie nowych krawędzi (po zamianie)
    delta += distMatrix[route[prev]][route[secondIdx]];
    delta += distMatrix[route[firstIdx]][route[next]];
    newCost = distMatrix[route[prev]][route[secondIdx]] +
              distMatrix[route[firstIdx]][route[next]];

  } else {
    // Wierzchołki rozłączne (brak wspólnych krawędzi)
    delta -= distMatrix[route[im1]][a];
    delta -= distMatrix[a][route[ip1]];
    delta -= distMatrix[route[jm1]][b];
    delta -= distMatrix[b][route[jp1]];
    oldCost = distMatrix[route[im1]][a] + distMatrix[a][route[ip1]] +
              distMatrix[route[jm1]][b] + distMatrix[b][route[jp1]];

    delta += distMatrix[route[im1]][b];
    delta += distMatrix[b][route[ip1]];
    delta += distMatrix[route[jm1]][a];
    delta += distMatrix[a][route[jp1]];
    newCost = distMatrix[route[im1]][b] + distMatrix[b][route[ip1]] +
              distMatrix[route[jm1]][a] + distMatrix[a][route[jp1]];
  }
  return newCost - oldCost;
}

// LOCAL SEARCH
std::pair<int, int> localSearchInvertFull(std::vector<int> &route) {
  int n = route.size();
  int len = calculateTourLength(route);
  int steps = 0;

  bool improved = true;
  while (improved) {
    improved = false;
    int bestDelta = 0, bi, bj;

    for (int i = 0; i < n - 1; i++) {
      for (int j = i + 1; j < n; j++) {
        if (i == 0 && j == n - 1)
          continue;

        int d = calculateInvertDelta(route, i, j);
        if (d < bestDelta) {
          bestDelta = d;
          bi = i;
          bj = j;
        }
      }
    }

    if (bestDelta < 0) {
      applyInvert(route, bi, bj);
      len += bestDelta;
      steps++;
      improved = true;
    }
  }
  return {len, steps};
}

std::pair<int, int> localSearchInvertRandom(std::vector<int> &route) {
  int n = route.size();
  int len = calculateTourLength(route);
  int steps = 0;

  bool improved = true;
  while (improved) {
    improved = false;
    int bestDelta = 0, bi, bj;

    for (int k = 0; k < n; k++) {
      int i = rand() % n;
      int j = rand() % n;
      if (i >= j || (i == 0 && j == n - 1))
        continue;

      int d = calculateInvertDelta(route, i, j);
      if (d < bestDelta) {
        bestDelta = d;
        bi = i;
        bj = j;
      }
    }

    if (bestDelta < 0) {
      applyInvert(route, bi, bj);
      len += bestDelta;
      steps++;
      improved = true;
    }
  }
  return {len, steps};
}

std::pair<int, int> localSearchTransposeFull(std::vector<int> &route) {
  int n = route.size();
  int len = calculateTourLength(route);
  int steps = 0;

  bool improved = true;
  while (improved) {
    improved = false;
    int bestDelta = 0, bi, bj;

    for (int i = 0; i < n - 1; i++) {
      for (int j = i + 1; j < n; j++) {
        int d = calculateTransposeDelta(route, i, j);
        if (d < bestDelta) {
          bestDelta = d;
          bi = i;
          bj = j;
        }
      }
    }

    if (bestDelta < 0) {
      applyTranspose(route, bi, bj);
      len += bestDelta;
      steps++;
      improved = true;
    }
  }
  return {len, steps};
}

// EKSPERYMENT

void runExperiment(const std::string &filename) {
  auto nodes = readTSP(filename);
  if (nodes.empty())
    return;

  int n = nodes.size();
  buildDistanceMatrix(nodes);

  std::vector<int> base(n);
  for (int i = 0; i < n; i++)
    base[i] = i;

  std::string baseName = filename.substr(0, filename.find('.'));

  std::ofstream f1(baseName + "_invert_full.csv");
  std::ofstream f2(baseName + "_invert_random.csv");
  std::ofstream f3(baseName + "_transpose.csv");

  f1 << "iteracja,wynik,kroki\n";
  f2 << "iteracja,wynik,kroki\n";
  f3 << "iteracja,wynik,kroki\n";

  double sumF = 0, sumR = 0, sumT = 0;
  double stepsF = 0, stepsR = 0, stepsT = 0;

  int bestF = INT_MAX, bestR = INT_MAX, bestT = INT_MAX;
  std::vector<int> bestRouteF, bestRouteR, bestRouteT;

  for (int i = 0; i < n; i++) {
    std::vector<int> r;
    if (i % 10 == 0) {
      std::cout << i << std::endl;
    }

    r = base;
    shuffleRoute(r);
    auto [f, sf] = localSearchInvertFull(r);
    f1 << i + 1 << "," << f << "," << sf << "\n";
    sumF += f;
    stepsF += sf;
    if (f < bestF) {
      bestF = f;
      bestRouteF = r;
    }

    r = base;
    shuffleRoute(r);
    auto [rr, sr] = localSearchInvertRandom(r);
    f2 << i + 1 << "," << rr << "," << sr << "\n";
    sumR += rr;
    stepsR += sr;
    if (rr < bestR) {
      bestR = rr;
      bestRouteR = r;
    }

    r = base;
    shuffleRoute(r);
    auto [t, st] = localSearchTransposeFull(r);
    f3 << i + 1 << "," << t << "," << st << "\n";
    sumT += t;
    stepsT += st;
    if (t < bestT) {
      bestT = t;
      bestRouteT = r;
    }
  }

  std::ofstream summary(baseName + "_summary.txt");
  summary << std::fixed << std::setprecision(2);

  summary << "Plik: " << filename << "\n";
  summary << "n = " << n << "\n\n";

  summary << "Invert Full:\n";
  summary << "avg = " << sumF / n << "\nsteps = " << stepsF / n
          << "\nbest = " << bestF << "\n";
  summary << "route = " << routeToString(bestRouteF) << "\n\n";

  summary << "Invert Random:\n";
  summary << "avg = " << sumR / n << "\nsteps = " << stepsR / n
          << "\nbest = " << bestR << "\n";
  summary << "route = " << routeToString(bestRouteR) << "\n\n";

  summary << "Transpose:\n";
  summary << "avg = " << sumT / n << "\nsteps = " << stepsT / n
          << "\nbest = " << bestT << "\n";
  summary << "route = " << routeToString(bestRouteT) << "\n";

  std::cout << "Zrobiono: " << filename << std::endl;
}

int main() {
  srand(time(0));

  std::vector<std::string> files = {"wi29.tsp", "dj38.tsp", "qa194.tsp",
                                    "uy734.tsp", "zi929.tsp"};

  for (auto &f : files)
    runExperiment(f);

  return 0;
}
