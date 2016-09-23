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
 * Create time: 2016-09-16
 * Email: waizungtaam@gmail.com
 */

#ifndef HEAP_H_
#define HEAP_H_

#include <cstddef>
#include <stdexcept>
#include <string>


template <typename Tp>
class Heap {
public:
  typedef Tp value_type;
  typedef Tp& reference;
  typedef const Tp& const_reference;
  typedef std::size_t size_type;

  Heap() : size_(0), container_{Tp()} {}
  virtual ~Heap() {}

  bool empty() const { return size_ == 0; }
  bool full() const { return size_ >= capacity_; }
  size_type size() const { return size_; }
  size_type capacity() const { return capacity_; }
  void clear() { size_ = 0; }

  size_type top() const {
    require_nonempty("Heap::top()");
    return container_[0];
  }
  void insert(const value_type& value) {
    require_nonfull("Heap::insert()");
    container_[size_] = value;
    shift_up(size_);
    ++size_;
  }
  void remove(const value_type& value) {
    require_nonempty("Heap::remove()");
    size_type index = search(value);
    if (index >= size_) {
      return;
    }
    container_[index] = container_[size_ - 1];
    shift_down(index);
    --size_;
  }
  void traverse(void (*func)(value_type&)) {
    for (size_type i = 0; i < size_; ++i) {
      (*func)(container_[i]);
    }
  }

protected:
  void shift_up(const size_type& index) {
    value_type value = container_[index];
    size_type iter_idx = index;
    while (iter_idx > 0) {
      size_type parent_idx = static_cast<size_type>((iter_idx - 1) / 2);
      if (container_[parent_idx] > value) {
        break;
      } else {
        container_[iter_idx] = container_[parent_idx];
        iter_idx = parent_idx;
      }
    }
    container_[iter_idx] = value;
  }
  void shift_down(const size_type& index) {
    size_type child_idx = 2 * index + 1;
    value_type value = container_[index];
    size_type iter_idx = index;
    while (child_idx < size_) {
      if (child_idx + 1 < size_ && 
          container_[child_idx] < container_[child_idx + 1]) {
        ++child_idx;
      }
      if (container_[child_idx] < value) {
        break;
      } else {
        container_[iter_idx] = container_[child_idx];
        iter_idx = child_idx;
        child_idx = 2 * iter_idx + 1;
      }
    }
    container_[iter_idx] = value;
  }
  size_type search(const value_type& value) {
    size_type index = 0;
    while (index < size_ && container_[index] != value) {
      ++index;
    }
    return index;
  }
  void require_nonempty(const std::string& function_name) {
    if (empty()) {
      throw std::out_of_range(
        function_name + " is undefined when the heap is empty.");
    }
  }
  void require_nonfull(const std::string& function_name) {
    if (full()) {
      throw std::length_error(
        function_name + " is invalid when the heap is full.");
    }
  }  

  static constexpr size_type capacity_ = 64;
  size_type size_;
  Tp container_[capacity_];
};

#endif  // HEAP_H_