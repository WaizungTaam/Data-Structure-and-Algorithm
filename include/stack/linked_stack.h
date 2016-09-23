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

#ifndef STACK_H_
#define STACK_H_

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>


template <typename Tp>
class Stack {
public:
  typedef Tp value_type;
  typedef Tp& reference;
  typedef const Tp& const_reference;
  typedef std::size_t size_type;

  Stack() : size_(0), top_ptr_(nullptr) {}
  Stack(const Stack& other) : size_(other.size_), top_ptr_(nullptr) {
    if (!other.empty()) {
      deep_copy(top_ptr_, other);
    }
  }
  Stack(Stack&& other) : 
    size_(std::move(other.size_)), top_ptr_(std::move(other.top_ptr_)) {}
  Stack& operator=(const Stack& rhs) {
    size_type new_size = rhs.size_;
    NodePtr new_top_ptr = nullptr;
    if (!rhs.empty()) {
      deep_copy(new_top_ptr, rhs);
    }
    clear();
    size_ = new_size; 
    top_ptr_ = new_top_ptr;
    return *this;
  }
  Stack& operator=(Stack&& rhs) {
    size_ = std::move(rhs.size_);
    top_ptr_ = std::move(rhs.top_ptr_);
    return *this;
  }
  virtual ~Stack() {}

  bool empty() const { return size_ == 0; }
  size_type size() const { return size_; }
  void clear() {
    size_ = 0;
    top_ptr_ = nullptr;
  }

  reference top() {
    require_nonempty("Stack::top()");
    return top_ptr_->value;
  }
  const_reference top() const {
    require_nonempty("Stack::pop()");
    return top_ptr_->value;
  }

  void push(const value_type& value) {
    NodePtr new_top_ptr = std::make_shared<Node>(value);
    new_top_ptr->next = top_ptr_;
    top_ptr_ = new_top_ptr;
    ++size_;
  }
  void pop() {
    require_nonempty("Stack::pop()");
    top_ptr_ = top_ptr_->next;
    --size_;
  }

protected:
  struct Node;
  typedef std::shared_ptr<Node> NodePtr;
  struct Node {
    Node() : value(Tp()), next(nullptr) {}
    explicit Node(const Tp& x) : value(x), next(nullptr) {}
    explicit Node(Tp&& x) : value(std::move(x)), next(nullptr) {}
    Tp value;
    NodePtr next;
  };

  void deep_copy(NodePtr& new_top_ptr, const Stack& other) {
    new_top_ptr = std::make_shared<Node>(other.top_ptr_->value);
    NodePtr copy_ptr = new_top_ptr, other_ptr = other.top_ptr_;
    while (other_ptr->next != nullptr) {
      other_ptr = other_ptr->next;
      copy_ptr->next = std::make_shared<Node>(other_ptr->value);
      copy_ptr = copy_ptr->next;
    }
  }
  void require_nonempty(const std::string& function_name) {
    if (empty()) {
      throw std::out_of_range(
        function_name + " is undefined when the stack is empty.");
    }
  }

  size_type size_;
  NodePtr top_ptr_;
};

#endif  // STACK_H_