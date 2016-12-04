#include <limits>
#include <map>
#include <set>
#include <utility>

template <typename Distance>
struct Infinity {
  inline static Distance value() {
    return std::numeric_limits<Distance>::max();
  }
};

template <typename Vertex>
struct Undefined {
  inline static Vertex value() { return Vertex(); }
};
template <> struct Undefined<int> 
{ inline static int value() { return -1; } };
template <> struct Undefined<char>
{ inline static char value() { return '?'; } };

template <typename Vertex, typename Distance>
struct Less {
  bool operator()(const std::pair<Vertex, Distance>& a, 
                  const std::pair<Vertex, Distance>& b) {
    return a.second < b.second || (a.second == b.second && a.first < b.first);
  }
};

template <typename Vertex, typename Distance>
void dijkstra(const std::map<Vertex, std::map<Vertex, Distance>>& graph,
              const Vertex& source, std::map<Vertex, Distance>& distance,
              std::map<Vertex, Vertex>& predcessor) {
  std::set<std::pair<Vertex, Distance>, Less<Vertex, Distance>> unvisited;
  for (const auto& p : graph) {
    Vertex vertex = p.first;
    if (vertex != source) {
      unvisited.insert(std::make_pair(vertex, Infinity<Distance>::value()));
      distance[vertex] = Infinity<Distance>::value();
      predcessor[vertex] = Undefined<Vertex>::value();
    }
  }
  unvisited.insert(std::make_pair(source, 0));
  distance[source] = 0;
  predcessor[source] = source;

  while (!unvisited.empty()) {
    Vertex current = (*unvisited.begin()).first;
    unvisited.erase(unvisited.begin());
    for (const auto& p : graph.at(current)) {
      Vertex neighbor = p.first;
      Distance dis_from_cur = p.second;
      Distance new_dis = distance[current] + dis_from_cur; // possibly overflow
      if (new_dis >= distance[current] && new_dis < distance[neighbor]) {
        auto it = unvisited.find(std::make_pair(neighbor, distance[neighbor]));
        if (it != unvisited.end()) {
          unvisited.erase(it);
          unvisited.insert(std::make_pair(neighbor, new_dis));
        }
        distance[neighbor] = new_dis;
        predcessor[neighbor] = current;
      }
    }
  }
}