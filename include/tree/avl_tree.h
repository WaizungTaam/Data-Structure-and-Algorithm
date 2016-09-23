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
 * Create time: 2016-09-08
 * Email: waizungtaam@gmail.com
 */


#ifndef AVL_TREE_H_
#define AVL_TREE_H_

#include <memory>

template <typename Tp>
class AVLTree {
public:
  typedef Tp value_type;
  typedef Tp& reference;
  typedef const Tp& const_reference;
  typedef std::size_t size_type;

  AVLTree() : size_(0), root_(nullptr) {}
  AVLTree(const AVLTree& other) : size_(other.size_), root_(nullptr) {
    if (!other.empty()) {
      deep_copy(root_, other.root_);
    }
  }
  AVLTree(AVLTree&& other) : 
    size_(std::move(other.size_)), root_(std::move(other.root_)) {}
  AVLTree& operator=(const AVLTree& rhs) {
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
  AVLTree& operator=(AVLTree&& rhs) {
    size_ = std::move(rhs.size_);
    root_ = std::move(rhs.root_);
    return *this;
  }
  virtual ~AVLTree() {}

  bool empty() const { return size_ == 0; }
  size_type size() const { return size_; }
  void clear() {
    size_ = 0;
    root_ = nullptr;
  }

  bool find(const value_type& value) const {
    return search(root_, value) != nullptr;
  }

  void insert(const value_type& value) {
    bool height_increased = false;
    insert(root_, value, height_increased);
  }
  void remove(const value_type& value) {
    bool height_decreased = false;
    remove(root_, value, height_decreased);
  }

  void preorder(void (*func)(value_type&)) {
    preorder(root_, func);
  }
  void inorder(void (*func)(value_type&)) {
    inorder(root_, func);
  }
  void postorder(void (*func)(value_type&)) {
    postorder(root_, func);
  }

  value_type max() const {
    return max(root_)->value;
  }
  value_type min() const {
    return min(root_)->value;
  }

protected:
  enum BalanceFactor { left_higher, equal_height, right_higher };

  struct Node;
  typedef std::shared_ptr<Node> NodePtr;
  struct Node {
    Node() : value(Tp()), left(nullptr), right(nullptr), 
             balance(equal_height) {}
    explicit Node(const Tp& x) : value(x), left(nullptr), right(nullptr),
                                 balance(equal_height) {}
    explicit Node(Tp&& x) : value(x), left(nullptr), right(nullptr),
                            balance(equal_height) {}
    Tp value;
    NodePtr left;
    NodePtr right;
    BalanceFactor balance;
  };

  void deep_copy(NodePtr& copy_ptr, const NodePtr& other_ptr) {
    if (other_ptr != nullptr) {
      copy_ptr = std::make_shared<Node>(other_ptr->value);
      deep_copy(copy_ptr->left, other_ptr->left);
      deep_copy(copy_ptr->right, other_ptr->right);
    }    
  }
  void search(const NodePtr& sub_root, const value_type& value) const {
    NodePtr iter_ptr = sub_root;
    while (iter_ptr != nullptr && iter_ptr->value != value) {
      if (iter_ptr->value > value) {
        iter_ptr = iter_ptr->left;
      } else {
        iter_ptr = iter_ptr->right;
      }
    }
    return iter_ptr;    
  }
  void insert(NodePtr& sub_root, const value_type& value, 
              bool& height_increased) {
    if (sub_root != nullptr && sub_root->value == value) {
      height_increased = false;
      return;
    }
    if (sub_root == nullptr) {
      sub_root = std::make_shared<Node>(value);
      height_increased = true;
      ++size_;
    } else if (sub_root->value > value) {
      insert(sub_root->left, value, height_increased);
      if (height_increased) {
        if (sub_root->balance == left_higher) {
          balance_left(sub_root);
          height_increased = false;
        } else if (sub_root->balance == right_higher) {
          sub_root->balance = equal_height;
          height_increased = false;
        } else {
          sub_root->balance = left_higher;
        }
      }
    } else {
      insert(sub_root->right, value, height_increased);
      if (height_increased) {
        if (sub_root->balance == left_higher) {
          sub_root->balance = equal_height;
          height_increased = false;
        } else if (sub_root->balance == right_higher) {
          balance_right(sub_root);
          height_increased = false;
        } else {
          sub_root->balance = right_higher;
        }
      }
    }
  }
  void remove(NodePtr& rm_ptr, const value_type& value, 
              bool& height_decreased) {
    if (rm_ptr == nullptr) {
      return;
    }
    if (rm_ptr->value == value) {
      if (rm_ptr->left == nullptr || rm_ptr->right == nullptr) {
        NodePtr child_ptr = rm_ptr->left != nullptr ? 
                            rm_ptr->left : rm_ptr->right;
        rm_ptr = child_ptr;
        height_decreased = true;
        --size_;
      } else {
        NodePtr replace_ptr = max(rm_ptr->left);
        rm_ptr->value = replace_ptr->value;
        remove(rm_ptr->left, rm_ptr->value, height_decreased);
      }
    } else if (rm_ptr->value > value) {
      remove(rm_ptr->left, value, height_decreased);
      if (height_decreased) {
        if (rm_ptr->balance == left_higher) {
          rm_ptr->balance = equal_height;
          height_decreased = false;
        } else if (rm_ptr->balance == right_higher) {
          balance_right(rm_ptr);
          height_decreased = false;
        } else {
          rm_ptr->balance = right_higher;
        }
      }
    } else {
      remove(rm_ptr->right, value, height_decreased);
      if (height_decreased) {
        if (rm_ptr->balance == left_higher) {
          balance_left(rm_ptr);
          height_decreased = false;
        } else if (rm_ptr->balance == right_higher) {
          rm_ptr->balance = equal_height;
          height_decreased = false;
        } else {
          rm_ptr->balance = left_higher;
        }
      }
    }    
  }
  void balance_left(NodePtr& sub_root) {
    NodePtr sub_root_l = sub_root->left;
    if (sub_root_l->balance == left_higher) {
      sub_root->balance = equal_height;
      sub_root_l->balance = equal_height;
      rotate_right(sub_root);
    } else if (sub_root_l->balance == right_higher) {
      NodePtr sub_root_l_r = sub_root_l->right;
      if (sub_root_l_r->balance == right_higher) {
        sub_root->balance = equal_height;
        sub_root_l->balance = left_higher;
      } else if (sub_root_l_r->balance == left_higher) {
        sub_root->balance = right_higher;
        sub_root_l->balance = equal_height;
      } else {
        sub_root->balance = equal_height;
        sub_root_l->balance = equal_height;
      }
      sub_root_l_r->balance = equal_height;
      rotate_left(sub_root_l);
      rotate_right(sub_root);
    }
  }
  void balance_right(NodePtr& sub_root) {
    NodePtr sub_root_r = sub_root->right;
    if (sub_root_r->balance == right_higher) {
      sub_root->balance = equal_height;
      sub_root_r->balance = equal_height;
      rotate_left(sub_root);
    } else if (sub_root_r->balance == left_higher) {
      NodePtr sub_root_r_l = sub_root_r->left;
      if (sub_root_r_l->balance == left_higher) {
        sub_root->balance = equal_height;
        sub_root_r->balance = right_higher;
      } else if (sub_root_r_l->balance == right_higher) {
        sub_root->balance = left_higher;
        sub_root_r->balance = equal_height;
      } else {
        sub_root->balance = equal_height;
        sub_root_r->balance = equal_height;
      }
      sub_root_r_l->balance = equal_height;
      rotate_right(sub_root->right);
      rotate_left(sub_root);
    }
  }
  void rotate_left(NodePtr& sub_root) {
    if (sub_root == nullptr || sub_root->right == nullptr) {
      return;
    }
    NodePtr sub_root_r = sub_root->right;
    sub_root->right = sub_root_r->left;
    sub_root_r->left = sub_root;
    sub_root = sub_root_r;
  }
  void rotate_right(NodePtr& sub_root) {
    if (sub_root == nullptr || sub_root->left == nullptr) {
      return;
    }
    NodePtr sub_root_l = sub_root->left;
    sub_root->left = sub_root_l->right;
    sub_root_l->right = sub_root;
    sub_root = sub_root_l;
  }

  void preorder(NodePtr& sub_root, void (*func)(value_type&)) {
    if (sub_root != nullptr) {
      (*func)(sub_root->value);
      preorder(sub_root->left, func);
      preorder(sub_root->right, func);
    }
  }
  void inorder(NodePtr& sub_root, void (*func)(value_type&)) {
    if (sub_root != nullptr) {      
      inorder(sub_root->left, func);
      (*func)(sub_root->value);
      inorder(sub_root->right, func);
    }
  }
  void postorder(NodePtr& sub_root, void (*func)(value_type&)) {
    if (sub_root != nullptr) {
      postorder(sub_root->left, func);
      postorder(sub_root->right, func);
      (*func)(sub_root->value);      
    }
  }    
  NodePtr max(const NodePtr& sub_root) const {
    NodePtr iter_ptr = sub_root;
    while (iter_ptr->right != nullptr) {
      iter_ptr = iter_ptr->right;
    }
    return iter_ptr;
  }
  NodePtr min(const NodePtr& sub_root) const {
    NodePtr iter_ptr = sub_root;
    while (iter_ptr->left != nullptr) {
      iter_ptr = iter_ptr->left;
    }
    return iter_ptr;
  }  

  size_type size_;
  NodePtr root_;
};

#endif  // AVL_TREE_H_