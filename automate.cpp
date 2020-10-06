#include <bits/stdc++.h>
#define debug
using namespace std;
struct undeterminated_vertex {
  std::map<char, std::set<int>> go;
  bool is_terminal = 0;
  undeterminated_vertex(std::map<char, std::set<int>>& edges) {
    for (auto [_symb, _set] : edges) {
      go[_symb] = _set;
    }
  }
  undeterminated_vertex() = default;
  undeterminated_vertex(const undeterminated_vertex&) = default;
  void add_edge(char x, int to) { go[x].insert(to); }
};
struct vertex {
  std::map<char, int> go;
  bool is_terminal = 0;
  int num;
  vertex(int num, std::map<char, int>& edges, bool is_Terminal)
      : num(num), is_terminal(is_Terminal) {
    for (auto [symb, v] : edges) {
      go[symb] = v;
    }
  }
  vertex(int num) : num(num) {}
  void print() {
    std::cout << "number: " << num << '\n';
    if (is_terminal) {
      std::cout << "terminal: yes\n";
    }
    for (auto [symb, v] : go) {
      std::cout << "  " << v << ": " << symb << '\n';
    }
  }
  void add_edge(char x, int to) { go[x] = to; }
};
class determinated_automate {
 private:
  std::vector<vertex> _mass;
  std::vector<char> alphabet = {'a', 'b'};

 public:
  determinated_automate(std::vector<vertex>& s, std::vector<char> alphabet)
      : _mass(s), alphabet(alphabet) {}
  determinated_automate(std::vector<undeterminated_vertex>& edges,
                        std::set<int>& terminal) {
    std::map<std::set<int>, int> vect;
    std::queue<std::set<int>> q;
    q.push({0});
    int counter = -1;
    vect[{0}] = ++counter;
    while (!q.empty()) {
      std::set<int> current = q.front();
      q.pop();
      std::map<char, int> new_edges;
      std::map<char, std::set<int>> new_vertexes;
      bool is_terminal = 0;
      for (auto& elem : current) {
        is_terminal |= (terminal.find(elem) != terminal.end());
      }
      for (auto v : current) {
        for (auto [symb, value] : edges[v].go) {
          for (auto to : value) {
            new_vertexes[symb].insert(to);
          }
        }
      }
      for (auto [key, value] : new_vertexes) {
        if (vect.find(value) == vect.end()) {
          q.push(value);
          vect[value] = ++counter;
        }
      }
      for (auto [symb, value] : new_vertexes) {
        new_edges[symb] = vect[value];
      }
      _mass.push_back(vertex(_mass.size(), new_edges, is_terminal));
    }
  }

  void make_rubbish_vertex() {
    _mass.push_back(vertex(_mass.size()));
    for (auto symb : alphabet) {
      _mass.back().go[symb] = _mass.size() - 1;
    }
  };
  void make_a_full() {
    bool flag = 0;
    for (auto& elem : _mass) {
      for (auto symb : alphabet) {
        if (elem.go.find(symb) == elem.go.end()) {
          if (!flag) {
            flag |= 1;
            make_rubbish_vertex();
          }
          elem.add_edge(symb, _mass.size() - 1);
        }
      }
    }
  }
  determinated_automate minimize();
  void print() {
    for (int i = 0; i < _mass.size(); i++) {
      std::cout << i << "{\n";
      _mass[i].print();
      std::cout << "}\n";
    }
  }
};
determinated_automate determinated_automate::minimize() {
  std::vector<std::vector<int>> met((_mass.size() + 1) * (_mass.size() + 1),
                                    std::vector<int>(_mass.size() + 1, 0));
  std::vector<std::vector<std::pair<int, int>>> reversed_edges(alphabet.size());
  for (int i = 0; i < _mass.size(); i++) {
    for (auto [symb, to] : _mass[i].go) {
      reversed_edges[symb - 'a'].push_back({to, i});
    }
  }
  // step 3
  std::queue<std::pair<int, int>> q;
  for (int i = 0; i < _mass.size(); i++) {
    for (int j = 0; j < _mass.size(); j++) {
      if (!met[i][j] && _mass[i].is_terminal != _mass[j].is_terminal) {
        met[i][j] = met[j][i] = true;
        q.push({i, j});
        q.push({i, j});
      }
    }
  }
  // step 4
  while (!q.empty()) {
    auto [v, to] = q.front();
    q.pop();
    for (auto s : alphabet) {
      int symb = s - 'a';
      for (int i = 0; i < reversed_edges[symb].size(); i++) {
        for (int j = 0; j < reversed_edges[symb].size(); j++) {
          auto t1 = reversed_edges[symb][i], t2 = reversed_edges[symb][j];
          if (v == t1.first && to == t2.first && !met[t1.second][t2.second]) {
            met[t1.second][t2.second] = met[t2.second][t1.second] = true;
            q.push({t1.second, t2.second});
            q.push({t2.second, t1.second});
          }
        }
      }
    }
  }
  // step 5
  std::vector<int> component(_mass.size(), -1);
  for (int i = 0; i < _mass.size(); i++) {
    if (!met[0][i]) {
      component[i] = 0;
    }
  }
  int components_count = 0;
  for (int i = 1; i < _mass.size(); i++) {
    if (component[i] == -1) {
      components_count++;
      component[i] = components_count;
      for (int j = i + 1; j < _mass.size(); j++) {
        if (!met[i][j]) {
          component[j] = components_count;
        }
      }
    }
  }
  std::vector<vertex> s(components_count + 1, 0);
  for (int i = 0; i < components_count + 1; i++) {
    s[i] = vertex(i);
    int v = 0;
    for (int j = 0; j < _mass.size(); j++) {
      if (component[j] == i) {
        v = j;
        break;
      }
    }
    for (auto symb : alphabet) {
      if (_mass[v].go.find(symb) != _mass[v].go.end()) {
        int to = _mass[v].go[symb];
        s[i].add_edge(symb, component[to]);
        s[i].is_terminal = _mass[v].is_terminal;
        s[i].num = i;
      }
    }
  }
  determinated_automate r(s, alphabet);
  return r;
}
int main() {
  std::map<std::pair<int, char>, std::set<int>> t;
  int n;
  std::cin >> n;
  std::vector<undeterminated_vertex> vertexes(n);
  int x = 1, z;
  char y = '1';
  while (true) {
    std::cin >> x >> y >> z;
    if (x == -1) break;
    t[{x, y}].insert(z);
    vertexes[x].add_edge(y, z);
  }
  int count_terminal;
  std::cin >> count_terminal;
  std::set<int> terminal;
  for (int i = 0, x; i < count_terminal; i++) {
    std::cin >> x;
    terminal.insert(x);
  }
  determinated_automate autom(vertexes, terminal);
  autom.make_a_full();
  std::cout << "DETERMINATED AUTOMATE:\n";
  autom.print();
  std::cout << "\n";
  determinated_automate avto = autom.minimize();
  std::cout << "MDKA:\n";
  avto.print();
}