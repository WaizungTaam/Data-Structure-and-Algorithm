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
 * Create time: 2016-09-22
 * Email: waizungtaam@gmail.com
 */

#ifndef RB_TREE_H_
#define RB_TREE_H_

#include <cstddef>
#include <functional>
#include <memory>


template <typename Tp>
class RBTree {
public:
  typedef Tp value_type;
  typedef Tp& reference;
  typedef const Tp& const_reference;
  typedef std::size_t size_type;

  RBTree() : size_(0), root_(nullptr) {}
  RBTree(const RBTree& other) {
    if (!other.empty()) {
      deep_copy(root_, other.root_);
    }
  }
  RBTree(RBTree&& other) : size_(std::move(other.size_)), 
    root_(std::move(other.root_)) {}
  RBTree& operator=(const RBTree& rhs) {
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
  RBTree& operator=(RBTree&& rhs) {
    size_ = std::move(rhs.size_);
    root_ = std::move(rhs.root_);
    return *this;
  }
  virtual ~RBTree() {}

  bool empty() const { return size_ == 0; }
  size_type size() const { return size_; }
  void clear() { size_ = 0; root_ = nullptr; }

  void insert(const value_type& value) {
    NodePtr insert_node = std::make_shared<Node>(value);
    insert(root_, insert_node);
    ++size_;
  }
  void remove(const value_type& value) {
    NodePtr remove_node = search(root_, value);
    if (remove_node != nullptr) {
      remove(root_, remove_node);
      --size_;
    }
  }

  bool find(const value_type& value) const {
    return search(root_, value) != nullptr;
  }
  void preorder(const std::function<void(value_type&)>& func) {
    preorder(root_, func);
  }
  void inorder(const std::function<void(value_type&)>& func) {
    inorder(root_, func);
  }
  void postorder(const std::function<void(value_type&)>& func) {
    postorder(root_, func);
  }

protected:
  enum Color { red, black };
  struct Node;
  typedef std::shared_ptr<Node> NodePtr;
  struct Node {
    Node() : value(Tp()), parent(nullptr), left(nullptr), right(nullptr), 
      color(black) {}
    explicit Node(const Tp& x) : value(x), parent(nullptr), left(nullptr),
      right(nullptr), color(black) {}
    explicit Node(Tp&& x) : value(std::move(x)), parent(nullptr), 
      left(nullptr), right(nullptr), color(black) {}
    Tp value;
    NodePtr parent;
    NodePtr left;
    NodePtr right;
    Color color;
  };

  void deep_copy(NodePtr& copy_ptr, const NodePtr& other_ptr) {
    if (other_ptr != nullptr) {
      copy_ptr = std::make_shared<Node>(other_ptr->value);
      deep_copy(copy_ptr->left, other_ptr->left);
      deep_copy(copy_ptr->right, other_ptr->right);
    }    
  }
  NodePtr search(const NodePtr& sub_root, const value_type& value) {
    while (sub_root != nullptr && sub_root->value != value) {
      if (sub_root->value > value) {
        return search(sub_root->left, value);  
      } else {
        return search(sub_root->right, value);
      }
    }
    return sub_root;
  }
  void rotate_left(NodePtr& sub_root, const NodePtr& node) {
    NodePtr rchild = node->right;
    node->right = rchild->left;
    if (rchild->left != nullptr) {
      rchild->left->parent = node;
    }
    rchild->parent = node->parent;
    if (node->parent == nullptr) {
      sub_root = rchild;
    } else {
      if (node->parent->left == node) {
        node->parent->left = rchild;
      } else {
        node->parent->right = rchild;
      }
    }
    rchild->left = node;
    node->parent = rchild;
  }
  void rotate_right(NodePtr& sub_root, const NodePtr& node) {
    NodePtr lchild = node->left;
    node->left = lchild->right;
    if (lchild->right != nullptr) {
      lchild->right->parent = node;
    }
    lchild->parent = node->parent;
    if (node->parent == nullptr) {
      sub_root = lchild;
    } else {
      if (node->parent->left == node) {
        node->parent->left = lchild;
      } else {
        node->parent->right = lchild;
      }
    }
    lchild->right = node;
    node->parent = lchild;
  }
  void insert(NodePtr& sub_root, const NodePtr& insert_node) {
    NodePtr iter_parent = nullptr, iter_node = sub_root;
    while (iter_node != nullptr) {
      iter_parent = iter_node;
      if (insert_node->value < iter_node->value) {
        iter_node = iter_node->left;
      } else {
        iter_node = iter_node->right;
      }
    }
    insert_node->parent = iter_parent;
    if (iter_parent != nullptr) {
      if (insert_node->value < iter_parent->value) {
        iter_parent->left = insert_node;
      } else {
        iter_parent->right = insert_node;
      }
    } else {
      sub_root = insert_node;
    }
    insert_node->color = red;
    insert_fix_up(sub_root, insert_node);
  }
  void insert_fix_up(NodePtr& sub_root, const NodePtr& inserted_node) {
    NodePtr node = inserted_node;
    NodePtr parent_node = node->parent;
    while (parent_node != nullptr && parent_node->color == red) {
      NodePtr grandparent_node = parent_node->parent;
      if (grandparent_node == nullptr) {
        return;
      } else if (parent_node == grandparent_node->left) {
        insert_fix_up_left(sub_root, node, parent_node, grandparent_node);
      } else if (parent_node == grandparent_node->right) {
        insert_fix_up_right(sub_root, node, parent_node, grandparent_node);
      }
      parent_node = node->parent;
    }
    sub_root->color = black;
  }
  void flip_color(const NodePtr& parent_node, const NodePtr& uncle_node,
                  const NodePtr& grandparent_node) {
    parent_node->color = black;
    uncle_node->color = black;
    grandparent_node->color = red;    
  }
  void insert_fix_up_left(NodePtr& sub_root, NodePtr& node, 
                          NodePtr& parent_node, NodePtr& grandparent_node) {
    NodePtr uncle_node = grandparent_node->right;
    if (uncle_node != nullptr && uncle_node->color == red) {
      flip_color(parent_node, uncle_node, grandparent_node);
      node = grandparent_node;
      return;
    }
    if (node == parent_node->right) {
      rotate_left(sub_root, parent_node);
      NodePtr swap_cache = parent_node;
      parent_node = node;
      node = swap_cache;
    }
    parent_node->color = black;
    grandparent_node->color = red;
    rotate_right(sub_root, grandparent_node);
  }
  void insert_fix_up_right(NodePtr& sub_root, NodePtr& node, 
                           NodePtr& parent_node, NodePtr& grandparent_node) {
    NodePtr uncle_node = grandparent_node->left;
    if (uncle_node != nullptr && uncle_node->color == red) {
      flip_color(parent_node, uncle_node, grandparent_node);
      node = grandparent_node;
      return;
    }
    if (node == parent_node->left) {
      rotate_right(sub_root, parent_node);
      NodePtr swap_cache = parent_node;
      parent_node = node;
      node = swap_cache;      
    }
    parent_node->color = black;
    grandparent_node->color = red;
    rotate_left(sub_root, grandparent_node);
  }
  NodePtr max(const NodePtr& sub_root) const {
    NodePtr iter_node = sub_root;
    while (iter_node->right != nullptr) {
      iter_node = iter_node->right;
    }
    return iter_node;
  }
  NodePtr min(const NodePtr& sub_root) const {
    NodePtr iter_node = sub_root;
    while (iter_node->left != nullptr) {
      iter_node = iter_node->left;
    }
    return iter_node;
  }  
  void remove(NodePtr& sub_root, const NodePtr& remove_node) {
    if (remove_node->left != nullptr && remove_node->right != nullptr) {
      NodePtr replace_node = min(remove_node->right);
      if (remove_node->parent != nullptr) {
        if (remove_node->parent->left == remove_node) {
          remove_node->parent->left = replace_node;
        } else {
          remove_node->parent->right = replace_node;
        }
      } else {
        sub_root = replace_node;
      }
      NodePtr child_node = replace_node->right, 
              parent_node = replace_node->parent;
      Color replace_color = replace_node->color;
      if (parent_node == remove_node) {
        parent_node = replace_node;
      } else {
        if (child_node != nullptr) {
          child_node->parent = parent_node;
        }
        parent_node->left = child_node;
        replace_node->right = remove_node->right;
        remove_node->right->parent = replace_node;
      }
      replace_node->parent = remove_node->parent;
      replace_node->left = remove_node->left;
      replace_node->color = remove_node->color;
      remove_node->left->parent = replace_node;
      if (replace_color == black) {
        remove_fix_up(sub_root, child_node, parent_node);
      }
    } else {
      NodePtr child_node;
      if (remove_node->left != nullptr) {
        child_node = remove_node->left;
      } else {
        child_node = remove_node->right;
      }
      NodePtr parent_node = remove_node->parent;
      Color remove_color = remove_node->color;
      if (child_node != nullptr) {
        child_node->parent = parent_node;
      }
      if (parent_node != nullptr) {
        if (remove_node == parent_node->left) {
          parent_node->left = child_node;
        } else {
          parent_node->right = child_node;
        }
      } else {
        sub_root = child_node;
      }
      if (remove_color == black) {
        remove_fix_up(sub_root, child_node, parent_node);
      }
    }
  }
  void remove_fix_up(NodePtr& sub_root, const NodePtr& removed_node_child,
                     const NodePtr& removed_node_parent) {
    NodePtr node = removed_node_child, parent_node = removed_node_parent;
    while ((node == nullptr || node->color == black) && node != sub_root) {
      if (parent_node->left == node) {
        remove_fix_up_left(sub_root, node, parent_node);
      } else {
        remove_fix_up_right(sub_root, node, parent_node);
      }
    }
    if (node != nullptr) {
      node->color = black;
    }
  }
  void remove_fix_up_left(NodePtr& sub_root, NodePtr& node, 
                          NodePtr& parent_node) {
    NodePtr brother_node = parent_node->right;
    if (brother_node->color == red) {
      brother_node->color = black;
      parent_node->color = red;
      rotate_left(sub_root, parent_node);
      brother_node = parent_node->right;
    }
    if ((brother_node->left == nullptr || 
         brother_node->left->color == black) &&
        (brother_node->right == nullptr ||
         brother_node->right->color == black)) {
      brother_node->color = red;
      node = parent_node;
      parent_node = node->parent;
    } else {
      if (brother_node->right == nullptr || 
          brother_node->right->color == black) {
        brother_node->left->color = black;
        brother_node->color = red;
        rotate_right(sub_root, brother_node);
        brother_node = parent_node->right;
      }
      brother_node->color = parent_node->color;
      parent_node->color = black;
      brother_node->right->color = black;
      rotate_left(sub_root, parent_node);
      node = sub_root;
    }
  }
  void remove_fix_up_right(NodePtr& sub_root, NodePtr& node,
                           NodePtr& parent_node) {
    NodePtr brother_node = parent_node->left;
    if (brother_node->color == red) {
      brother_node->color = black;
      parent_node->color = red;
      rotate_right(sub_root, parent_node);
      brother_node = parent_node->left;
    }
    if ((brother_node->left == nullptr || 
         brother_node->left->color == black) &&
        (brother_node->right == nullptr ||
         brother_node->right->color == black)) {
      brother_node->color = red;
      node = parent_node;
      parent_node = node->parent;
    } else {
      if (brother_node->left == nullptr || 
          brother_node->left->color == black) {
        brother_node->right->color = black;
        brother_node->color = red;
        rotate_left(sub_root, brother_node);
        brother_node = parent_node->left;
      }
      brother_node->color = parent_node->color;
      parent_node->color = black;
      brother_node->left->color = black;
      rotate_right(sub_root, parent_node);
      node = sub_root;
    }      
  }
  void preorder(NodePtr& sub_root, 
                const std::function<void(value_type&)>& func) {
    if (sub_root != nullptr) {
      func(sub_root->value);
      preorder(sub_root->left, func);
      preorder(sub_root->right, func);
    }
  }
  void inorder(NodePtr& sub_root, 
               const std::function<void(value_type&)>& func) {
    if (sub_root != nullptr) {
      inorder(sub_root->left, func);
      func(sub_root->value);
      inorder(sub_root->right, func);
    }
  }
  void postorder(NodePtr& sub_root, 
                 const std::function<void(value_type&)>& func) {
    if (sub_root != nullptr) {
      postorder(sub_root->left, func);
      postorder(sub_root->right, func);
      func(sub_root->value);
    }
  }

  size_type size_;
  NodePtr root_;
};

#endif  // RB_TREE_H_