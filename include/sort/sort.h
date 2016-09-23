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
 * Create time: 2016-09-09
 * Email: waizungtaam@gmail.com
 */


#ifndef SORT_H_
#define SORT_H_

#include <cmath>
#include <iterator>
#include <vector>


namespace detail {

template <typename Tp>
void swap(Tp& a, Tp& b) {
  Tp tmp(a);
  a = b;
  b = tmp;
}

template <typename Iterator>
void bubble_sort(Iterator iter_begin, Iterator iter_end) {
  for (Iterator it_i = iter_begin; it_i != iter_end - 1; ++it_i) {
    for (Iterator it_j = iter_begin; 
         it_j != iter_end - 1 - (it_i - iter_begin); ++it_j) {
      if (*it_j > *(it_j + 1)) {
        swap(*it_j, *(it_j + 1));
      }
    }
  }
}

template <typename Iterator>
void insertion_sort(Iterator iter_begin, Iterator iter_end) {
  for (Iterator it_i = iter_begin + 1; it_i != iter_end; ++it_i) {
    auto x = *it_i;
    Iterator it_j = it_i;
    for (it_j = it_i; it_j != iter_begin && *(it_j - 1) > x; --it_j) {
      *it_j = *(it_j - 1);
    }
    *it_j = x;
  }
}

template <typename Iterator>
void selection_sort(Iterator iter_begin, Iterator iter_end) {
  for (Iterator it_i = iter_begin; it_i != iter_end - 1; ++it_i) {
    Iterator it_min = it_i;
    for (Iterator it_j = it_i + 1; it_j != iter_end; ++it_j) {
      if (*it_j < *it_min) {
        it_min = it_j;
      }
    }
    if (it_min != it_i) {
      swap(*it_i, *it_min);
    }
  }
}

template <typename Tp>
void generate_gap(Tp& gap_value, const Tp& size = 0) {
  // Adopting gap sequence 2^k - 1
  if (gap_value == 0) {
    auto iexp = static_cast<Tp>(std::log2(size + 1));
    gap_value = static_cast<Tp>(std::pow(2, iexp) - 1);
  } else {
    gap_value = static_cast<Tp>(gap_value / 2);
  }
}
template <typename Iterator>
void shell_sort(Iterator iter_begin, Iterator iter_end) {
  typename std::iterator_traits<Iterator>::difference_type gap = 0;
  generate_gap(gap, iter_end - iter_begin);
  while (gap >= 1) {
    for (Iterator it_i = iter_begin + gap; it_i != iter_end; ++it_i) {
      auto x = *it_i;
      Iterator it_j = it_i;
      for (it_j = it_i; it_j >= iter_begin + gap && *(it_j - gap) > x; 
           it_j -= gap) {
        *it_j = *(it_j - gap);
      }
      *it_j = x;
    }
    generate_gap(gap);
  }
}

template <typename Iterator>
void merge(Iterator iter_begin, Iterator iter_middle, Iterator iter_end) {
  Iterator it_l = iter_begin, it_r = iter_middle;
  std::vector<typename std::iterator_traits<Iterator>::value_type> extra_space;
  while (it_l != iter_middle && it_r != iter_end) {
    if (*it_l <= *it_r) {
      extra_space.push_back(*it_l);
      ++it_l;
    } else {
      extra_space.push_back(*it_r);
      ++it_r;
    }
  }
  while (it_l != iter_middle) {
    extra_space.push_back(*it_l);
    ++it_l;
  }
  while (it_r != iter_end) {
    extra_space.push_back(*it_r);
    ++it_r;
  }
  for (Iterator it_res = iter_begin, it_tmp = extra_space.begin(); 
       it_res != iter_end, it_tmp != extra_space.end(); ++it_res, ++it_tmp) {
    *it_res = *it_tmp;
  }
}
template <typename Iterator>
void merge_sort(Iterator iter_begin, Iterator iter_end) {
  if (iter_end - iter_begin <= 1) {
    return;
  }
  auto middle_index = static_cast<
    typename std::iterator_traits<Iterator>::difference_type>(
    (iter_end - iter_begin) / 2);
  Iterator iter_middle = iter_begin + middle_index;
  merge_sort(iter_begin, iter_middle);
  merge_sort(iter_middle, iter_end);
  merge(iter_begin, iter_middle, iter_end);
}

template <typename Iterator>
void quick_sort(Iterator iter_begin, Iterator iter_end) {
  if (iter_end - iter_begin > 1) {  
    swap(*iter_begin, *(iter_begin + (iter_end - iter_begin) / 2));
    Iterator iter_pivot = iter_begin;
    for (Iterator it = iter_begin + 1; it != iter_end; ++it) {
      if (*it < *iter_begin) {
        ++iter_pivot;
        swap(*iter_pivot, *it);
      }
    }
    swap(*iter_begin, *iter_pivot);
    quick_sort(iter_begin, iter_pivot);
    quick_sort(iter_pivot + 1, iter_end);
  }
}

template <typename Iterator>
void sift_down(Iterator iter_begin, Iterator iter_end, Iterator iter_root) {
  while (iter_begin + 2 * (iter_root - iter_begin) + 1 < iter_end) {
    Iterator it_lchild = iter_begin + 2 * (iter_root - iter_begin) + 1, 
             it_rchild = it_lchild + 1, it_max = iter_root;
    if (*it_lchild > *it_max) {
      it_max = it_lchild;
    }
    if (it_rchild < iter_end && *it_rchild > *it_max) {
      it_max = it_rchild;
    }
    if (it_max == iter_root) {
      return;
    } else {
      swap(*iter_root, *it_max);
      iter_root = it_max;
    }
  }
}
template <typename Iterator>
void heap_sort(Iterator iter_begin, Iterator iter_end) {
  auto it_root = iter_begin + static_cast<
    typename std::iterator_traits<Iterator>::difference_type>(
    (iter_end - iter_begin - 2) / 2);
  for (it_root; it_root >= iter_begin; --it_root) {
    sift_down(iter_begin, iter_end, it_root);
  }
  for (Iterator it = iter_end - 1; it != iter_begin; --it) {
    swap(*iter_begin, *it);
    sift_down(iter_begin, it, iter_begin);
  }
}

template <typename Iterator>
void intro_sort(Iterator iter_begin, Iterator iter_end, 
                typename std::iterator_traits<
                Iterator>::difference_type max_depth) {
  auto size = iter_end - iter_begin;
  if (size <= 1) {
    return;
  } else if (max_depth == 0) {
    heap_sort(iter_begin, iter_end);
  } else {
    swap(*iter_begin, *(iter_begin + (iter_end - iter_begin) / 2));
    Iterator iter_pivot = iter_begin;
    for (Iterator it = iter_begin + 1; it != iter_end; ++it) {
      if (*it < *iter_begin) {
        ++iter_pivot;
        swap(*iter_pivot, *it);
      }
    }
    swap(*iter_begin, *iter_pivot);
    intro_sort(iter_begin, iter_pivot, max_depth - 1);
    intro_sort(iter_pivot + 1, iter_end, max_depth - 1);
  }
}
template <typename Iterator>
void intro_sort(Iterator iter_begin, Iterator iter_end) {
  auto max_depth = static_cast<
    typename std::iterator_traits<Iterator>::difference_type>(
    std::log2(iter_end - iter_begin)) * 2;
  intro_sort(iter_begin, iter_end, max_depth);
}

}  // namespace detail


template <typename Iterator>
void bubble_sort(Iterator iter_begin, Iterator iter_end) {
  if (iter_begin >= iter_end) return;
  detail::bubble_sort(iter_begin, iter_end);
}
template <typename Iterator>
void insertion_sort(Iterator iter_begin, Iterator iter_end) {
  if (iter_begin >= iter_end) return;
  detail::insertion_sort(iter_begin, iter_end);
}
template <typename Iterator>
void selection_sort(Iterator iter_begin, Iterator iter_end) {
  if (iter_begin >= iter_end) return;
  detail::selection_sort(iter_begin, iter_end);
}
template <typename Iterator>
void shell_sort(Iterator iter_begin, Iterator iter_end) {
  if (iter_begin >= iter_end) return;
  detail::shell_sort(iter_begin, iter_end);
}
template <typename Iterator>
void merge_sort(Iterator iter_begin, Iterator iter_end) {
  if (iter_begin >= iter_end) return;
  detail::merge_sort(iter_begin, iter_end);
}
template <typename Iterator>
void quick_sort(Iterator iter_begin, Iterator iter_end) {
  if (iter_begin >= iter_end) return;
  detail::quick_sort(iter_begin, iter_end);
}
template <typename Iterator>
void heap_sort(Iterator iter_begin, Iterator iter_end) {
  if (iter_begin >= iter_end) return;
  detail::heap_sort(iter_begin, iter_end);
}
template <typename Iterator>
void intro_sort(Iterator iter_begin, Iterator iter_end) {
  if (iter_begin >= iter_end) return;
  detail::intro_sort(iter_begin, iter_end);
}

#endif  // SORT_H_