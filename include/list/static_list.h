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

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>

template <typename Tp>
class List {
public:
  typedef Tp value_type;
  typedef Tp& reference;
  typedef const Tp& const_reference;
  typedef std::size_t size_type;

  List() : size_(0), container_{Tp()} {}
  List(const std::initializer_list<Tp>& il) : size_(0), container_{Tp()} {
    require_not_out_of_capacity(il.size());
    size_type index = 0;
    for (const auto& element : il) {
      container_[index] = element;
      ++index;
    }
    size_ = il.size();
  }
  List& operator=(const std::initializer_list<Tp>& il) {
    require_not_out_of_capacity(il.size());
    size_type index = 0;
    for (const auto& element : il) {
      container_[index] = element;
      ++index;
    }
    size_ = il.size();    
    return *this;
  }
  virtual ~List() {}

  bool empty() const { return size_ == 0; }
  bool full() const { return size_ >= capacity_; }

  size_type size() const { return size_; }
  size_type capacity() const { return capacity_; }  

  void clear() { size_ = 0; }

  const_reference retrieve(const size_type& index) const {
    require_nonempty("List::retrieve()");
    check_range_inclusive_exclusive(index);
    return container_[index];
  }
  void replace(const size_type& index, const value_type& value) {
    require_nonempty("List::replace()");
    check_range_inclusive_exclusive(index);
    container_[index] = value;
  }
  value_type remove(const size_type& index) {
    require_nonempty("List::remove()");
    check_range_inclusive_exclusive(index);
    value_type removed_value = container_[index];
    for (size_type i = index; i < size_ - 1; ++i) {
      container_[i] = container_[i + 1];
    }
    --size_;
    return removed_value;
  }
  void insert(const size_type& index, const value_type& value) {
    require_nonfull("List::insert()");
    check_range_inclusive_inclusive(index);
    if (size_ >= 1) {
      for (size_type i = size_ - 1; i >= index; --i) {
        container_[i + 1] = container_[i];
      }
    }
    container_[index] = value;
    ++size_;
  }
  void traverse(void (*f)(value_type&)) {
    for (size_type i = 0; i < size_; ++i) {
      (*f)(container_[i]);
    }
  }

protected:
  void require_nonempty(const std::string& function_name) const {
    if (empty()) {
      throw std::out_of_range(
        function_name + " is undefined when the stack is empty.");
    }
  }
  void require_nonfull(const std::string& function_name) const {
    if (full()) {
      throw std::length_error(
        function_name + " is invalid when the stack is full.");
    }
  }
  void require_not_out_of_capacity(const size_type& other_container_size) {
    if (other_container_size > capacity_) {
      throw std::length_error(
        "The capacity (" + std::to_string(capacity_) + ") is not enough.");
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

  static constexpr size_type capacity_ = 64;
  size_type size_;
  Tp container_[capacity_];
};

#endif  // LIST_H_