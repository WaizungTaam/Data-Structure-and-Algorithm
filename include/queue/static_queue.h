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
#include <stdexcept>
#include <string>

template <typename Tp>
class Queue {
public:
  typedef Tp value_type;
  typedef Tp& reference;
  typedef const Tp& const_reference;
  typedef std::size_t size_type;

  Queue() : size_(0), front_index_(0), back_index_(capacity_ - 1), 
            container_{Tp()} {}
  virtual ~Queue() {}

  bool empty() const { return size_ == 0; }
  bool full() const { return size_ >= capacity_; }

  size_type size() const { return size_; }
  size_type capacity() const { return capacity_; }

  void clear() {
    size_ = 0;
    front_index_ = 0;
    back_index_ = capacity_ - 1;
  }

  reference front() {
    require_nonempty("Queue::front()");
    return container_[front_index_];
  }
  const_reference front() const {
    require_nonempty("Queue::front()");
    return container_[front_index_];
  }
  reference back() {
    require_nonempty("Queue::back()");
    return container_[back_index_];
  }
  const_reference back() const {
    require_nonempty("Queue::back()");
    return container_[back_index_];
  }

  void push(const value_type& value) {
    require_nonfull("Queue::push()");
    back_index_ = back_index_ + 1 == capacity_ ? 0 : back_index_ + 1;
    container_[back_index_] = value;
    ++size_;
  }
  void pop() {
    require_nonempty("Queue::pop()");
    front_index_ = front_index_ + 1 == capacity_ ? 0 : front_index_ + 1;
    --size_;
  }

protected:
  void require_nonempty(const std::string& function_name) {
    if (empty()) {
      throw std::out_of_range(
        function_name + " is undefined when the stack is empty.");
    }
  }
  void require_nonfull(const std::string& function_name) {
    if (full()) {
      throw std::length_error(
        function_name + " is invalid when the stack is full.");
    }
  }

  static constexpr size_type capacity_ = 64;
  size_type size_;
  size_type front_index_;
  size_type back_index_;
  Tp container_[capacity_];
};

#endif  // QUEUE_H_