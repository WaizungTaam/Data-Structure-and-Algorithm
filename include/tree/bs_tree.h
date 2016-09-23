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
 * Create time: 2016-09-05
 * Email: waizungtaam@gmail.com
 */

#ifndef BS_TREE_H_
#define BS_TREE_H_

#include <memory>

template <typename Tp>
class BSTree {
public:
  typedef Tp value_type;
  typedef Tp& reference;
  typedef const Tp& const_reference;
  typedef std::size_t size_type;

  BSTree() : size_(0), root_ptr_(nullptr) {}
  BSTree(const BSTree& other) : size_(other.size_), root_ptr_(nullptr) {
    if (!other.empty()) {
      deep_copy(root_ptr_, other.root_ptr_);
    }
  }
  BSTree(BSTree&& other) : 
    size_(std::move(other.size_)), root_ptr_(std::move(other.root_ptr_)) {}
  BSTree& operator=(const BSTree& rhs) {
    size_type new_size = rhs.size_;
    NodePtr new_root_ptr = nullptr;
    if (!rhs.empty()) {
      deep_copy(new_root_ptr, rhs.root_ptr_);
    }
    clear();
    size_ = new_size;
    root_ptr_ = new_root_ptr;
    return *this;
  }
  BSTree& operator=(BSTree&& rhs) {
    size_ = std::move(rhs.size_);
    root_ptr_ = std::move(rhs.root_ptr_);
    return *this;
  }
  virtual ~BSTree() {}

  bool empty() const { return size_ == 0; }
  size_type size() const { return size_; }
  void clear() {
    size_ = 0;
    root_ptr_ = nullptr;
  }

  bool find(const value_type& value) const {
    return search(root_ptr_, value) != nullptr;
  }
  void remove(const value_type& value) {
    remove(root_ptr_, value);
  }
  void insert(const value_type& value) {
    insert(root_ptr_, value);
  }

  void preorder(void (*func)(value_type&)) {
    preorder(root_ptr_, func);
  }
  void inorder(void (*func)(value_type&)) {
    inorder(root_ptr_, func);
  }
  void postorder(void (*func)(value_type&)) {
    postorder(root_ptr_, func);
  }

  value_type max() const {
    return max(root_ptr_)->value;
  }
  value_type min() const {
    return min(root_ptr_)->value;
  }

protected:
  struct Node;
  typedef std::shared_ptr<Node> NodePtr;
  struct Node {
    Node() : value(Tp()), left(nullptr), right(nullptr) {}
    explicit Node(const Tp& x) : value(x), left(nullptr), right(nullptr) {}
    explicit Node(Tp&& x) : value(x), left(nullptr), right(nullptr) {}
    Tp value;
    NodePtr left;
    NodePtr right;
  };

  void deep_copy(NodePtr& copy_ptr, const NodePtr& other_ptr) {
    if (other_ptr != nullptr) {
      copy_ptr = std::make_shared<Node>(other_ptr->value);
      deep_copy(copy_ptr->left, other_ptr->left);
      deep_copy(copy_ptr->right, other_ptr->right);
    }
  }
  NodePtr search(const NodePtr& sub_root_ptr, 
                 const value_type& value) const {
    NodePtr iter_ptr = sub_root_ptr;
    while (iter_ptr != nullptr && iter_ptr->value != value) {
      if (iter_ptr->value > value) {
        iter_ptr = iter_ptr->left;
      } else {
        iter_ptr = iter_ptr->right;
      }
    }
    return iter_ptr;
  }
  void insert(NodePtr& sub_root_ptr, const value_type& value) {
    if (sub_root_ptr == nullptr) {
      sub_root_ptr = std::make_shared<Node>(value);
      ++size_;
    } else if (sub_root_ptr->value > value) {
      insert(sub_root_ptr->left, value);
    } else if (sub_root_ptr->value < value) {
      insert(sub_root_ptr->right, value);
    }
  }
  void remove(NodePtr& rm_ptr, const value_type& value) {
    if (rm_ptr == nullptr) {
      return;
    }
    if (rm_ptr->value == value) {
      if (rm_ptr->left == nullptr || rm_ptr->right == nullptr) {
        NodePtr child_ptr = rm_ptr->left != nullptr ? 
                            rm_ptr->left : rm_ptr->right;
        rm_ptr = child_ptr;
        --size_;
      } else {
        NodePtr replace_ptr = max(rm_ptr->left);
        rm_ptr->value = replace_ptr->value;
        remove(rm_ptr->left, rm_ptr->value);
      }
    } else if (rm_ptr->value > value) {
      remove(rm_ptr->left, value);
    } else {
      remove(rm_ptr->right, value);      
    }
  }
  void preorder(NodePtr& sub_root_ptr, void (*func)(value_type&)) {
    if (sub_root_ptr != nullptr) {
      (*func)(sub_root_ptr->value);
      preorder(sub_root_ptr->left, func);
      preorder(sub_root_ptr->right, func);
    }
  }
  void inorder(NodePtr& sub_root_ptr, void (*func)(value_type&)) {
    if (sub_root_ptr != nullptr) {
      inorder(sub_root_ptr->left, func);
      (*func)(sub_root_ptr->value);
      inorder(sub_root_ptr->right, func);
    }
  }
  void postorder(NodePtr& sub_root_ptr, void (*func)(value_type&)) {
    if (sub_root_ptr != nullptr) {
      postorder(sub_root_ptr->left, func);
      postorder(sub_root_ptr->right, func);
      (*func)(sub_root_ptr->value);
    }
  }
  NodePtr max(const NodePtr& sub_root_ptr) const {
    NodePtr iter_ptr = sub_root_ptr;
    while (iter_ptr->right != nullptr) {
      iter_ptr = iter_ptr->right;
    }
    return iter_ptr;
  }
  NodePtr min(const NodePtr& sub_root_ptr) const {
    NodePtr iter_ptr = sub_root_ptr;
    while (iter_ptr->left != nullptr) {
      iter_ptr = iter_ptr->left;
    }
    return iter_ptr;
  }

  size_type size_;
  NodePtr root_ptr_;
};

#endif  // BS_TREE_H_