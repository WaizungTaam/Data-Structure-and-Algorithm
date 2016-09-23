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
 * Create time: 2016-09-15
 * Email: waizungtaam@gmail.com
 */


#ifndef B_TREE_H_
#define B_TREE_H_

#include <memory>
#include <array>


typedef std::size_t order_type;

template <typename Tp, order_type order>
class BTree {
public:
  typedef Tp value_type;
  typedef Tp& reference;
  typedef const Tp& const_reference;
  typedef std::size_t size_type;

  BTree() : size_(0), root_(nullptr) {}
  BTree(const BTree& other) : size_(other.size_), root_(nullptr) {
    if (!other.empty()) {
      deep_copy(root_, other.root_);
    }
  }
  BTree(BTree&& other) : size_(other.size_), root_(other.root_) {}
  BTree& operator=(const BTree& rhs) {
    size_type new_size = rhs.size_;
    NodePtr new_root = nullptr;
    if (!rhs.empty()) {
      deep_copy(new_root, rhs.root_);
    }
    clear();
    size_ = new_size;
    root_ = new_root;
    return *this;    
  }
  BTree& operator=(BTree&& rhs) {
    size_ = std::move(rhs.size_);
    root_ = std::move(rhs.root_);
    return *this;
  }
  virtual ~BTree() {}

  bool empty() const { return size_ != 0; }
  size_type size() const { return size_; }
  void clear() {
    size_ = 0;
    root_ = nullptr;
  }

  bool find(const value_type& value) {
    return search(root_, value) != nullptr;
  }
  void insert(const value_type& value) {
    value_type median;
    NodePtr right_branch;
    if (!push_down(root_, value, median, right_branch)) {
      NodePtr new_root = std::make_shared<Node>();
      new_root->size = 1;
      new_root->data[0] = median;
      new_root->branch[0] = root_;
      new_root->branch[1] = right_branch;
      root_ = new_root;
    }
  }
  void remove(const value_type& value) {
    remove(root_, value);
    if (root_ != nullptr && root_->size == 0) {
      NodePtr old_root = root_;
      root_ = root_->branch[0];
    }
  }
  void traverse(void (*func)(value_type&)) {
    traverse(root_, func);
  }

protected:
  struct Node;
  typedef std::shared_ptr<Node> NodePtr;
  struct Node {
    template <typename ListT, order_type ListN>
    using List = std::array<ListT, ListN>;
    Node() : size(0), data(List<Tp, order - 1>()),
             branch(List<NodePtr, order>()) {}
    size_type size;
    List<Tp, order - 1> data;
    List<NodePtr, order> branch;
  };

  void deep_copy(NodePtr& copy_ptr, const NodePtr& other_ptr) {
    if (other_ptr != nullptr) {
      copy_ptr = std::make_shared<Node>(other_ptr->value);
      deep_copy(copy_ptr->left, other_ptr->left);
      deep_copy(copy_ptr->right, other_ptr->right);
    }    
  }  
  NodePtr search(const NodePtr& sub_root, const value_type& value, 
                 size_type& index) {
    if (sub_root == nullptr) {
      return nullptr;
    }
    if (is_in_node(sub_root, value, index)) {
      return sub_root;
    } else {
      return search(sub_root->branch[index], value, index);
    }
  }
  NodePtr search(const NodePtr& sub_root, const value_type& value) {
    size_type dummy;
    return search(sub_root, value, dummy);
  }
  bool is_in_node(const NodePtr& current_node, const value_type& value, 
                  size_type& index) {
    size_type pos = 0;
    while (pos < current_node->size && current_node->data[pos] < value) {
      ++pos;
    }
    index = pos;
    return pos < current_node->size && current_node->data[pos] == value;
  }
  bool push_down(NodePtr& current_node, const value_type& value, 
                 value_type& median, NodePtr& right_branch) {
    if (current_node == nullptr) {
      median = value;
      right_branch = nullptr;
      ++size_;
      return false;
    } else {
      size_type index;
      if (is_in_node(current_node, value, index)) {
        return true;
      } else {
        value_type extra_value;
        NodePtr extra_branch;
        if (push_down(current_node->branch[index], value, 
                      extra_value, extra_branch)) {
          return true;
        } else {
          if (current_node->size < order - 1) {
            push_in(current_node, extra_value, extra_branch, index);
            return true;
          } else {
            split_node(current_node, extra_value, extra_branch, index, 
                       median, right_branch);
            return false;
          }
        }
      }
    }
  }
  void push_in(NodePtr& current_node, const value_type& value, 
               const NodePtr& right_branch, const size_type& index) {
    for (size_type i = current_node->size; i > index; --i) {
      current_node->data[i] = current_node->data[i - 1];
      current_node->branch[i + 1] = current_node->branch[i];
    }
    current_node->data[index] = value;
    current_node->branch[index + 1] = right_branch;
    ++current_node->size;
  }
  void split_node(NodePtr& current_node, const value_type& extra_value,
                  NodePtr& extra_branch, const size_type& index, 
                  value_type& median, NodePtr& right_branch) {
    right_branch = std::make_shared<Node>();
    size_type middle = static_cast<size_type>(order / 2);
    bool index_le_middle = (index <= middle);
    middle = index_le_middle ? middle : middle + 1;
    for (size_type i = middle; i < order - 1; ++i) {
      right_branch->data[i - middle] = current_node->data[i];
      right_branch->branch[i + 1 - middle] = current_node->branch[i + 1];
    }
    current_node->size = middle;
    right_branch->size = order - 1 - middle;
    if (index_le_middle) {
      push_in(current_node, extra_value, extra_branch, index);  
    } else {
      push_in(right_branch, extra_value, extra_branch, index - middle);
    }
    median = current_node->data[current_node->size - 1];
    right_branch->branch[0] = current_node->branch[current_node->size];
    --current_node->size;
  }
  void remove(NodePtr& current_node, const value_type& value) {
    if (current_node == nullptr) {
      return;
    }
    size_type index;
    if (is_in_node(current_node, value, index)) {
      if (current_node->branch[index] != nullptr) {
        copy_from_predecessor(current_node, index);
        remove(current_node->branch[index], current_node->data[index]);
      } else {
        remove_data(current_node, index);
        --size_;
      }
    } else {
      remove(current_node->branch[index], value);
    }
    if (current_node->branch[index] != nullptr && 
        current_node->branch[index]->size < 
        static_cast<size_type>((order - 1) / 2)) {
      restore(current_node, index);
    }
  }
  void remove_data(NodePtr& current_node, const size_type& index) {
    for (size_type i = index; i < current_node->size - 1; ++i) {
      current_node->data[i] = current_node->data[i + 1];
    }
    --current_node->size;
  }
  void copy_from_predecessor(NodePtr& current_node, const size_type& index) {
    NodePtr leaf = current_node->branch[index];
    while (leaf->branch[leaf->size] != nullptr) {
      leaf = leaf->branch[leaf->size];
    }
    current_node->data[index] = leaf->data[leaf->size - 1];
  }
  void restore(NodePtr& current_node, const size_type& index) {
    if (index == 0) {
      if (current_node->branch[1]->size > 
          static_cast<size_type>((order - 1) / 2)) {
        move_left_from_branch(current_node, 1);
      } else {
        combine(current_node, 1);
      }
    } else if (index == current_node->size) {
      if (current_node->branch[index - 1]->size >
          static_cast<size_type>((order - 1) / 2)) {
        move_right_from_branch(current_node, index - 1);
      } else {
        combine(current_node, index);
      }
    } else {
      if (current_node->branch[index - 1]->size > 
          static_cast<size_type>((order - 1) / 2)) {
        move_right_from_branch(current_node, index - 1);
      } else if (current_node->branch[index + 1]->size >
                 static_cast<size_type>((order - 1) / 2)) {
        move_left_from_branch(current_node, index + 1);
      } else {
        combine(current_node, index);
      }
    }
  }
  void move_left_from_branch(NodePtr& current_node, const size_type& index) {
    NodePtr to_branch = current_node->branch[index - 1],
            from_branch = current_node->branch[index];
    to_branch->data[to_branch->size] = current_node->data[index - 1];
    ++to_branch->size;
    current_node->data[index - 1] = from_branch->data[0];
    --from_branch->size;
    to_branch->branch[to_branch->size] = from_branch->branch[0];
    for (size_type i = 0; i < from_branch->size; ++i) {
      from_branch->data[i] = from_branch->data[i + 1];
    }
    for (size_type i = 0; i < from_branch->size + 1; ++i) {
      from_branch->branch[i] = from_branch->branch[i + 1];
    }
  }
  void move_right_from_branch(NodePtr& current_node, const size_type& index) {
    NodePtr to_branch = current_node->branch[index + 1],
            from_branch = current_node->branch[index];
    for (size_type i = to_branch->size - 1; i >= 0; --i) {
      to_branch->data[i + 1] = to_branch->data[i];
    }
    for (size_type i = to_branch->size; i >= 0; --i) {
      to_branch->branch[i + 1] = to_branch->branch[i];
    }
    ++to_branch->size;
    to_branch->data[0] = current_node->data[index];
    current_node->data[index] = from_branch->data[from_branch->size];
    to_branch->branch[0] = from_branch->branch[from_branch->size];
    --from_branch->size;
  }
  void combine(NodePtr& current_node, const size_type& index) {
    NodePtr left_branch = current_node->branch[index - 1],
            right_branch = current_node->branch[index];
    left_branch->data[left_branch->size] = current_node->data[index - 1];
    ++left_branch->size;
    --current_node->size;
    left_branch->branch[left_branch->size] = right_branch->branch[0];
    for (size_type i = 0; i < right_branch->size; ++i) {
      left_branch->data[left_branch->size] = right_branch->data[i];
      ++left_branch->size;
      left_branch->branch[left_branch->size] = right_branch->branch[i + 1];
    }
    for (size_type i = index - 1; i < current_node->size; ++i) {
      current_node->data[i] = current_node->data[i + 1];
      current_node->branch[i + 1] = current_node->branch[i + 2];
    }
  }

  void traverse(NodePtr& current_node, void (*func)(value_type&)) {
    if (current_node != nullptr) {
      for (size_type i = 0; i < current_node->size; ++i) {
        (*func)(current_node->data[i]);
      }
      for (size_type i = 0; i < current_node->size + 1; ++i) {
        traverse(current_node->branch[i], func);
      }
    }
  }

  size_type size_;
  NodePtr root_;
};

#endif  // B_TREE_H_