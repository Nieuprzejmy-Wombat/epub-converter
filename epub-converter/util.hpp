#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <vector>

std::string now();

template <typename T> std::vector<T> &add(std::vector<T> &vec, T elem) {
  vec.push_back(elem);
  return vec;
}

template <typename T, typename U> std::vector<U> upcast(std::vector<T> items) {
  std::vector<U> res;
  for (auto it : items) {
    res.push_back(it);
  }
  return res;
}

#endif
