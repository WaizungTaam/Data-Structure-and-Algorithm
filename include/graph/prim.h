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

template <typename Vertex, typename Distance>
struct Less {
  bool operator()(const std::pair<Vertex, std::pair<Vertex, Distance>>& a, 
                  const std::pair<Vertex, std::pair<Vertex, Distance>>& b) {
    return a.second.second < b.second.second ||
           (a.second.second == b.second.second && a.first < b.first) ||
           (a.second.second == b.second.second && a.first == b.first &&
            a.second.first < b.second.first);

  }
};

template <typename Vertex, typename Distance>
void prim(const std::map<Vertex, std::map<Vertex, Distance>>& graph,
          const Vertex& source, std::map<Vertex, Vertex>& mst) {
  std::set<std::pair<Vertex, std::pair<Vertex, Distance>>, 
           Less<Vertex, Distance>> unconnected;
  for (const auto& p : graph) {
    unconnected.insert(std::make_pair(p.first, 
      std::make_pair(p.first, p.first == source ? 0 : 
        Infinity<Distance>::value())));
  }
  while (!unconnected.empty()) {
    Vertex new_vertex = unconnected.begin()->first;
    Vertex parent = unconnected.begin()->second.first;
    mst.insert(std::make_pair(new_vertex, parent));
    unconnected.erase(unconnected.begin());
    for (const auto& p : graph.at(new_vertex)) {
      Vertex neighbor = p.first;
      Distance new_dis = p.second;
      auto it_neighbor = unconnected.begin();
      for (; it_neighbor != unconnected.end(); ++it_neighbor) {
        if (it_neighbor->first == neighbor) break;
      }
      if (it_neighbor != unconnected.end()) {
        if (new_dis < it_neighbor->second.second) {
          unconnected.erase(it_neighbor);
          unconnected.insert(std::make_pair(neighbor,
            std::make_pair(new_vertex, new_dis)));
        }
      }
    }
  }
}