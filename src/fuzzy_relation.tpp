#include <iostream>
#include <stdexcept>

#include "FuzzyRelation/fuzzy_relation.hpp"
#include "FuzzySets/fuzzy_set.hpp"

template <typename T>
bool have_same_keys(const std::unordered_map<std::pair<T, T>, double,
                                             pair_hash, pair_equal>& left,
                    const std::unordered_map<std::pair<T, T>, double,
                                             pair_hash, pair_equal>& right) {
  if (left.size() != right.size()) {
    return false;
  }

  for (const auto& [key, _] : left) {
    if (!right.contains(key)) {
      return false;
    }
  }

  return true;
}

template <typename T>
bool composable(const std::unordered_map<std::pair<T, T>, double, pair_hash,
                                         pair_equal>& left,
                const std::unordered_map<std::pair<T, T>, double, pair_hash,
                                         pair_equal>& right) {
  std::unordered_set<T> left_second;
  std::unordered_set<T> right_first;

  for (auto& [key, _] : left) {
    left_second.insert(key.second);
  }

  for (auto& [key, _] : right) {
    right_first.insert(key.first);
  }
  return left_second == right_first;
}

template <typename T>
FuzzyRelation<T>::FuzzyRelation(const std::vector<std::vector<T>>& data,
                                const std::vector<T>& row,
                                const std::vector<T>& collumn) {
  if (data.size() != collumn.size() ||
      (data.size() > 0 && data[0].size() != row.size())) {
    throw std::runtime_error(
        "Relation matrix doesn't have size compatable with element's "
        "vectors");
  }
  for (auto i = 0; i < row.size(); ++i) {
    for (auto j = 0; j < collumn.size(); ++j) {
      this->data_.insert({std::pair<T, T>(row[i], collumn[j]), data[i][j]});
    }
  }
}
template <typename T>
FuzzyRelation<T>::FuzzyRelation(
    const std::unordered_map<std::pair<T, T>, double, pair_hash, pair_equal>&
        data)
    : data_(data) {}

template <typename T>
FuzzyRelation<T>::FuzzyRelation(const FuzzySet<T>& left,
                                const FuzzySet<T>& right) {
  for (const auto& [left_key, left_value] : left) {
    for (const auto& [right_key, right_value] : right) {
      this->data_.insert(
          {{left_key, right_key}, std::min(left_value, right_value)});
    }
  }
}

template <typename T>
FuzzyRelation<T>& FuzzyRelation<T>::Unite(const FuzzyRelation& other) {
  if (!have_same_keys(this->data_, other.data_)) {
    throw std::runtime_error(
        "Can't unite relations, where relational matrices are of the "
        "different order");
  }
  for (auto& [key, value] : this->data_) {
    value = std::max(value, other.data_.at(key));
  }

  return *this;
}
template <typename T>
FuzzyRelation<T>& FuzzyRelation<T>::Intersect(const FuzzyRelation& other) {
  if (!have_same_keys(this->data_, other.data_)) {
    throw std::runtime_error(
        "Can't intersect relations, where relational matrices are of the "
        "different order");
  }
  for (auto& [key, value] : this->data_) {
    value = std::min(value, other.data_.at(key));
  }
  return *this;
}
template <typename T>
FuzzyRelation<T>& FuzzyRelation<T>::Complement() {
  for (auto& [key, value] : this->data_) {
    value = 1 - value;
  }
  return *this;
}
template <typename T>
FuzzyRelation<T> FuzzyRelation<T>::Union(const FuzzyRelation& other) const {
  FuzzyRelation result(*this);
  result.Unite(other);
  return result;
}
template <typename T>
FuzzyRelation<T> FuzzyRelation<T>::Intersection(
    const FuzzyRelation& other) const {
  FuzzyRelation result(*this);
  result.Intersect(other);
  return result;
}
template <typename T>
FuzzyRelation<T> FuzzyRelation<T>::Complementation() const {
  FuzzyRelation result(*this);
  result.Complement();
  return result;
}

template <typename T>
FuzzyRelation<T> FuzzyRelation<T>::Composition(
    const FuzzyRelation& other) const {
  if (!composable(this->data_, other.data_)) {
    throw std::runtime_error("Can't compose relations.");
  }
  typename FuzzySet<T>::FuzMap result_data;

  for (const auto& pair1 : data_) {
    T x = pair1.first.first;
    T y = pair1.first.second;
    double x_y = pair1.second;

    for (const auto& pair2 : other.data_) {
      if (y == pair2.first.first) {
        T z = pair2.first.second;
        double y_z = pair2.second;

        double min_val = std::min(x_y, y_z);

        std::pair<T, T> x_z_pair = std::make_pair(x, z);
        if (result_data.find(x_z_pair) == result_data.end()) {
          result_data[x_z_pair] = min_val;
        } else {
          result_data[x_z_pair] = std::max(result_data[x_z_pair], min_val);
        }
      }
    }
  }
  return FuzzyRelation(result_data);
}
template <typename T>
bool FuzzyRelation<T>::operator==(const FuzzyRelation& other) {
  if (this->data_.size() != other.data_.size()) {
    return false;
  }
  for (const auto& pair : this->data_) {
    const std::pair<T, T>& key = pair.first;
    const double& value = pair.second;
    if (!other.data_.contains(key) ||
        abs(other.data_.at(key) - value) > 10e-12) {
      return false;
    }
  }
  return true;
}
template <typename T>
bool FuzzyRelation<T>::operator!=(const FuzzyRelation& other) {
  return !(*this == other);
}
template <typename T>
typename FuzzySet<T>::FuzMap FuzzyRelation<T>::GetRelation() {
  return this->data_;
}
template <typename T>
FuzzyRelation<T> Union(const FuzzyRelation<T>& left,
                       const FuzzyRelation<T>& right) {
  return left.Union(right);
}
template <typename T>
FuzzyRelation<T> Intersection(const FuzzyRelation<T>& left,
                              const FuzzyRelation<T>& right) {
  return left.Intersection(right);
}
template <typename T>
FuzzyRelation<T> Complementation(const FuzzyRelation<T>& object) {
  return object.Complementation();
}
template <typename T>
FuzzyRelation<T> Composition(const FuzzyRelation<T>& left,
                             const FuzzyRelation<T>& right) {
  return left.Composition(right);
}
