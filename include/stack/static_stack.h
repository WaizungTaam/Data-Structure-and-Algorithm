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
#include <stdexcept>
#include <string>


template <typename Tp>
class Stack {
public:
  typedef Tp value_type;
  typedef Tp& reference;
  typedef const Tp& const_reference;
  typedef std::size_t size_type;

  Stack() : size_(0), container_{Tp()} {}
  virtual ~Stack() {}

  bool empty() const { return size_ == 0; }
  bool full() const { return size_ >= capacity_; }

  size_type size() const { return size_; }
  size_type capacity() const { return capacity_; }

  void clear() { size_ = 0; }

  reference top() {
    require_nonempty("Stack::top()");
    return container_[size_ - 1]; 
  }
  const_reference top() const {
    require_nonempty("Stack::top()");
    return container_[size_ - 1];    
  }

  void push(const value_type& value) {
    require_nonfull("Stack::push()");
    container_[size_] = value;
    ++size_;
  }
  void pop() {
    require_nonempty("Stack::pop()");
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
  Tp container_[capacity_];
};

#endif  // STACK_H_