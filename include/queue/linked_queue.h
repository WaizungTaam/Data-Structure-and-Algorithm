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

#ifndef QUEUE_H_
#define QUEUE_H_

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>

template <typename Tp>
class Queue {
public:
  typedef Tp value_type;
  typedef Tp& reference;
  typedef const Tp& const_reference;
  typedef std::size_t size_type;

  Queue() : size_(0), front_ptr_(nullptr), back_ptr_(nullptr) {}
  Queue(const Queue& other) : 
    size_(other.size_), front_ptr_(nullptr), back_ptr_(nullptr) {
    if (!other.empty()) {
      deep_copy(front_ptr_, back_ptr_, other);
    }
  }
  Queue(Queue&& other) : 
    size_(std::move(other.size_)), front_ptr_(std::move(other.front_ptr_)),
    back_ptr_(std::move(other.back_ptr_)) {}
  Queue& operator=(const Queue& rhs) {
    size_type new_size = rhs.size_;
    NodePtr new_front_ptr = nullptr, new_back_ptr = nullptr;
    if (!rhs.empty()) {
      deep_copy(new_front_ptr, new_back_ptr, rhs);
    }
    clear();
    size_ = new_size;
    front_ptr_ = new_front_ptr;
    back_ptr_ = new_back_ptr;
    return *this;
  }
  Queue& operator=(Queue&& rhs) {
    size_ = std::move(rhs.size_);
    front_ptr_ = std::move(rhs.front_ptr_);
    back_ptr_ = std::move(rhs.back_ptr_);
    return *this;
  }
  virtual ~Queue() {}

  bool empty() const { return size_ == 0; }
  size_type size() const { return size_; }
  void clear() {
    size_ = 0;
    front_ptr_ = nullptr;
    back_ptr_ = nullptr;
  }

  reference front() {
    require_nonempty("Queue::front()");
    return front_ptr_->value;
  }
  const_reference front() const {
    require_nonempty("Queue::front()");
    return front_ptr_->value;
  }
  reference back() {
    require_nonempty("Queue::back()");
    return back_ptr_->value;
  }
  const_reference back() const {
    require_nonempty("Queue::back()");
    return back_ptr_->value;
  }

  void push(const value_type& value) {
    NodePtr new_back_ptr = std::make_shared<Node>(value);
    if (size_ == 0) {
      front_ptr_ = back_ptr_ = new_back_ptr;
    } else {
      back_ptr_->next = new_back_ptr;
      back_ptr_ = new_back_ptr;
    }
    ++size_;
  }
  void pop() {
    require_nonempty("Queue::pop()");
    front_ptr_ = front_ptr_->next;
    if (size_ == 1) {
      back_ptr_ = nullptr;
    }
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

  void deep_copy(NodePtr& new_front_ptr, NodePtr& new_back_ptr, 
                 const Queue& other) {
    new_front_ptr = std::make_shared<Node>(other.front_ptr_->value);
    NodePtr copy_ptr = new_front_ptr, other_ptr = other.front_ptr_;
    while (other_ptr->next != nullptr) {
      other_ptr = other_ptr->next;
      copy_ptr->next = std::make_shared<Node>(other_ptr->value);
      copy_ptr = copy_ptr->next;
    }
    new_back_ptr = copy_ptr;
  }
  void require_nonempty(const std::string& function_name) {
    if (empty()) {
      throw std::out_of_range(
        function_name + " is undefined when the queue is empty.");
    }
  }

  size_type size_;
  NodePtr front_ptr_;
  NodePtr back_ptr_;
};

#endif  // QUEUE_H_