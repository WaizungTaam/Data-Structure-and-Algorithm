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
 * Create time: 2016-09-03
 * Email: waizungtaam@gmail.com
 */

#ifndef LIST_H_
#define LIST_H_

#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string>

template <typename Tp>
class List {
public:
  typedef Tp value_type;
  typedef Tp& reference;
  typedef const Tp& const_reference;
  typedef std::size_t size_type;

  List() : size_(0), head_ptr_(nullptr) {}
  List(const List& other) : size_(other.size_), head_ptr_(nullptr) {
    if (!other.empty()) {
      deep_copy(head_ptr_, other);
    }
  }
  List(List&& other) : 
    size_(std::move(other.size_)), head_ptr_(std::move(other.head_ptr_)) {}
  List(const std::initializer_list<value_type>& il) : 
    size_(0), head_ptr_(nullptr) {
    size_type index = 0;
    for (const value_type& element : il) {
      insert(index, element);
      ++index;
    }
  }
  List& operator=(const List& rhs) {
    size_type new_size = rhs.size_;
    NodePtr new_head_ptr = nullptr;
    if (!rhs.empty()) {
      deep_copy(new_head_ptr, rhs);
    }
    clear();
    size_ = new_size;
    head_ptr_ = new_head_ptr;
    return *this;
  }
  List& operator=(List&& rhs) {
    size_ = std::move(rhs.size_);
    head_ptr_ = std::move(rhs.head_ptr_);
    return *this;
  }
  List& operator=(const std::initializer_list<value_type>& il) {
    clear();
    size_type index = 0;
    for (const value_type& element : il) {
      insert(index, element);
      ++index;
    }
    return *this;
  }
  virtual ~List() {}

  bool empty() const { return size_ == 0; }
  size_type size() const { return size_; }
  void clear() {
    size_ = 0;
    head_ptr_ = nullptr;
  }

  const_reference retrieve(const size_type& index) const {
    require_nonempty("List::retrieve()");
    check_range_inclusive_exclusive(index);
    return to_node_ptr(index)->value;
  }
  void replace(const size_type& index, const value_type& value) {
    require_nonempty("List::replace()");
    check_range_inclusive_exclusive(index);
    to_node_ptr(index)->value = value;
  }
  value_type remove(const size_type& index) {
    require_nonempty("List::remove()");
    check_range_inclusive_exclusive(index);
    value_type removed_value = value_type();
    if (index == 0) {
      removed_value = head_ptr_->value;
      head_ptr_ = head_ptr_->next;
    } else {
      NodePtr prev_ptr = to_node_ptr(index - 1);
      removed_value = prev_ptr->next->value;
      prev_ptr->next = prev_ptr->next->next;
    }
    --size_;
    return removed_value;
  }
  void insert(const size_type& index, const value_type& value) {
    check_range_inclusive_inclusive(index);
    NodePtr new_ptr = std::make_shared<Node>(value);
    if (index == 0) {
      new_ptr->next = head_ptr_;
      head_ptr_ = new_ptr;
    } else {
      NodePtr prev_ptr = to_node_ptr(index - 1);
      new_ptr->next = prev_ptr->next;
      prev_ptr->next = new_ptr;
    }
    ++size_;
  }
  void traverse(void (*f)(value_type&)) {
    NodePtr traverse_ptr = head_ptr_;
    while (traverse_ptr != nullptr) {
      (*f)(traverse_ptr->value);
      traverse_ptr = traverse_ptr->next;
    }
  }

protected:
  struct Node;
  typedef std::shared_ptr<Node> NodePtr;
  struct Node {
    Node() : value(value_type()), next(nullptr) {}
    explicit Node(const value_type& x) : value(x), next(nullptr) {}
    explicit Node(value_type&& x) : value(std::move(x)), next(nullptr) {}
    Tp value;
    NodePtr next;
  };

  void deep_copy(NodePtr& new_head_ptr, const List& other) {
    new_head_ptr = std::make_shared<Node>(other.head_ptr_->value);
    NodePtr copy_ptr = new_head_ptr, other_ptr = other.head_ptr_;
    while (other_ptr->next != nullptr) {
      other_ptr = other_ptr->next;
      copy_ptr->next = std::make_shared<Node>(other_ptr->value);
      copy_ptr = copy_ptr->next;
    }
  }
  NodePtr to_node_ptr(const size_type& index) const {
    NodePtr iter_ptr = head_ptr_;
    for (size_type i = 0; i < index; ++i) {
      iter_ptr = iter_ptr->next;
    }
    return iter_ptr;
  }
  void require_nonempty(const std::string& function_name) const {
    if (empty()) {
      throw std::out_of_range(
        function_name + " is undefined when the stack is empty.");
    }
  }
  void check_range_inclusive_exclusive(const size_type& index) const {
    if (!(index >= 0 && index <= size_ - 1)) {
      throw std::out_of_range(
        "List range check failed: " + 
        std::to_string(index) + " is out of the range [0, " +
        std::to_string(size_) + ").");
    }
  }  
  void check_range_inclusive_inclusive(const size_type& index) const {
    if (!(index >= 0 && index <= size_)) {
      throw std::out_of_range(
        "List range check failed: " + 
        std::to_string(index) + " is out of the range [0, " +
        std::to_string(size_) + "].");
    }
  }  

  size_type size_;
  NodePtr head_ptr_;
};

#endif  // LIST_H_