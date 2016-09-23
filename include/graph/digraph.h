/*
 * Copyright 2016 Waizung Taam
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Create time: 2016-09-19
 * Email: waizungtaam@gmail.com
 */

#ifndef DIGRAPH_H_
#define DIGRAPH_H_

#include <functional>
#include <map>
#include <queue>
#include <vector>


template <typename Vertex>
class Digraph {
public:
  Digraph() {}
  Digraph(const std::map<Vertex, std::vector<Vertex>>& g) {
    neighbor_ = g;
  }

  void depth_first(const std::function<void(const Vertex&)>& func) {
    std::map<Vertex, bool> visited;
    for (const auto& pair : neighbor_) {
      visited[pair.first] = false;
    }
    for (const auto& pair : neighbor_) {
      traverse(pair.first, func, visited);
    }
  }
  void breadth_first(const std::function<void(const Vertex&)>& func) {
    std::map<Vertex, bool> visited;
    for (const auto& pair : neighbor_) {
      visited[pair.first] = false;
    }
    std::queue<Vertex> visit_queue;
    for (const auto& pair : neighbor_) {
      if (!visited.at(pair.first)) {
        visit_queue.push(pair.first);
        while (!visit_queue.empty()) {
          Vertex v = visit_queue.front();
          if (!visited.at(v)) {
            func(v);
            visited.at(v) = true;
            for (const auto& adj_v : neighbor_.at(v)) {
              visit_queue.push(adj_v);
            }
          }
          visit_queue.pop();
        }
      }
    }
  }

private:
  void traverse(const Vertex& v, 
                const std::function<void(const Vertex&)>& func,
                std::map<Vertex, bool>& visited) {
    if (!visited.at(v)) {
      func(v);
      visited.at(v) = true;
      for (const Vertex& adj_v : neighbor_.at(v)) {
        traverse(adj_v, func, visited);
      }      
    }
  }
  
  std::map<Vertex, std::vector<Vertex>> neighbor_;
};

#endif  // DIGRAPH_H_