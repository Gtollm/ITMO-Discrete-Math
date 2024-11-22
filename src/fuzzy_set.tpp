#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "FuzzySets/fuzzy_set.hpp"

inline double Tap(double a, double b) { return a * b; }

inline double Tmin(double a, double b) { return std::min(a, b); }

template <typename T>
typename FuzzySet<T>::FuzMap CRelation(const FuzzySet<T> &left,
                                       const FuzzySet<T> &right) {
  FuzzySet<T> A_complement = left.Complementation();

  typename FuzzySet<T>::FuzMap A_B_relation;
  for (const auto &[a_key, a_value] : left.GetFuzzySet()) {
    for (const auto &[b_key, b_value] : right.GetFuzzySet()) {
      A_B_relation.insert({{a_key, b_key}, std::min(a_value, b_value)});
    }
  }

  typename FuzzySet<T>::FuzMap A_complement_B_relation;
  for (const auto &[a_key, a_value] : A_complement.GetFuzzySet()) {
    for (const auto &[b_key, b_value] : right.GetFuzzySet()) {
      A_complement_B_relation.insert(
          {{a_key, b_key}, std::min(a_value, 1.0)});
    }
  }

  typename FuzzySet<T>::FuzMap R_relation;
  for (auto &[key, value] : A_B_relation) {
    R_relation[key] = std::max(value, A_complement_B_relation[key]);
  }
  return R_relation;
}

template <typename T>
FuzzySet<T>::FuzzySet() = default;

template <typename T>
FuzzySet<T>::FuzzySet(const FuzzySet &other) : data_(other.data_) {}

template <typename T>
FuzzySet<T>::FuzzySet(FuzzySet &&other) noexcept
    : data_(std::move(other.data_)) {}

template <typename T>
FuzzySet<T>::FuzzySet(
    const std::initializer_list<std::pair<T, double>> &data) {
  for (const auto &pair : data) {
    if (pair.second < 0.0 || pair.second > 1.0 ||
        this->data_.contains(pair.first)) {
      std::cerr << "Invalid membership value for " << pair.first << "."
                << std::endl;
      continue;
    }
    this->data_[pair.first] = pair.second;
  }
}

template <typename T>
FuzzySet<T>::FuzzySet(const std::unordered_map<T, double> &data)
    : data_(data) {}

template <typename T>
FuzzySet<T> &FuzzySet<T>::operator=(const FuzzySet &other) {
  this->data_ = std::copy(other.data_);
  return *this;
}

template <typename T>
FuzzySet<T> &FuzzySet<T>::operator=(FuzzySet &&other) noexcept {
  this->data_ = std::move(other.data_);
  return *this;
}

template <typename T>
auto FuzzySet<T>::operator==(const FuzzySet<T> &other) const {
  if (this->data_.size() != other.data_.size()) {
    return false;
  }
  for (const auto &pair : this->data_) {
    const T &key = pair.first;
    const double &value = pair.second;
    if (!other.data_.contains(key) ||
        abs(other.data_.at(key) - value) > 10e-12) {
      return false;
    }
  }
  return true;
}

template <typename T>
auto FuzzySet<T>::operator!=(const FuzzySet<T> &other) const {
  return !(*this == other);
}

template <typename T>
const double &FuzzySet<T>::at(const T &element) const {
  return this->data_.at(element);
}

template <typename T>
double &FuzzySet<T>::operator[](const T &element) {
  return this->data_[element];
}

template <typename T>
void FuzzySet<T>::insert(const std::pair<T, double> &element) {
  this->insert(element.first, element.second);
}

template <typename T>
void FuzzySet<T>::insert(const T &key, double value) {
  if (value < 0.0 || value > 1.0 || this->data_.contains(key)) {
    std::cerr << "Invalid membership value for " << key << "." << std::endl;
    return;
  }
  this->data_.insert(std::make_pair(key, value));
}

template <typename T>
void FuzzySet<T>::erase(const std::pair<T, double> &element) {
  this->erase(element.first);
}

template <typename T>
void FuzzySet<T>::erase(const T &key) {
  this->data_.erase(key);
}

template <typename T>
FuzzySet<T> &FuzzySet<T>::Unite(const FuzzySet<T> &other) {
  for (const auto &pair : other.data_) {
    if (this->data_.contains(pair.first)) {
      double max_member = std::max(this->at(pair.first), pair.second);
      (*this)[pair.first] = max_member;
    } else {
      (*this)[pair.first] = pair.second;
    }
  }
  return *this;
}

template <typename T>
FuzzySet<T> &FuzzySet<T>::Intersect(const FuzzySet<T> &other) {
  for (auto it = this->data_.begin(); it != this->data_.end();) {
    auto pair = *it;
    if (other.data_.contains(pair.first)) {
      double min_member = std::min(pair.second, other.at(pair.first));
      (*this)[pair.first] = min_member;
      ++it;
    } else {
      it = this->data_.erase(it);
    }
  }
  return *this;
}
template <typename T>
FuzzySet<T> &FuzzySet<T>::Complement() {
  for (const auto &pair : this->data_) {
    double comp_member = 1 - pair.second;
    (*this)[pair.first] = comp_member;
  }
  return *this;
}

template <typename T>
FuzzySet<T> &FuzzySet<T>::Implicate(const FuzzySet<T> &other,
                                    ImplicationType type) {
  for (auto &[key, _] : ::Union(*this, other).data_) {
    double value = this->data_.contains(key) ? this->data_.at(key) : -1;
    double other_value =
        other.data_.contains(key) ? other.data_.at(key) : 0.0;
    switch (type) {
      case ImplicationType::ALGPRODUCT:
        if (value == -1) {
          this->data_[key] = 0;
        } else {
          this->data_[key] = value * other_value;
        }
        break;
      case ImplicationType::MINIMUM:
        if (value == -1) {
          this->data_[key] = 0;
        } else {
          this->data_[key] = std::min(value, other_value);
        }
        break;
      case ImplicationType::ZADEHARITHMETIC:
        if (value == -1) {
          this->data_[key] = 0;
        } else {
          this->data_[key] = std::min(1.0, 1.0 - value + other_value);
        }
        break;
      case ImplicationType::ZADEHMAXMIN:
        if (value == -1) {
          this->data_[key] = 0;
        } else {
          this->data_[key] = std::max(1.0 - value, other_value);
        }
        break;
      default:
        throw std::invalid_argument("Invalid ImplicationType");
    }
  }
  return *this;
}

template <typename T>
FuzzySet<T> FuzzySet<T>::Union(const FuzzySet &other) const {
  FuzzySet result = *this;
  result.Unite(other);
  return result;
}

template <typename T>
FuzzySet<T> FuzzySet<T>::Intersection(const FuzzySet &other) const {
  FuzzySet result = *this;
  result.Intersect(other);
  return result;
}

template <typename T>
FuzzySet<T> FuzzySet<T>::Complementation() const {
  FuzzySet result = *this;
  result.Complement();
  return result;
}
template <typename T>
FuzzySet<T> FuzzySet<T>::Implication(const FuzzySet<T> &other,
                                     ImplicationType type) const {
  FuzzySet result(*this);
  result.Implicate(other, type);
  return result;
}

template <typename T>
FuzzySet<T> FuzzySet<T>::GeneralizedModusPonens(
    const FuzzySet<T> &right, const FuzzySet<T> &left_prime) const {
  FuzMap R_relation = CRelation(*this, right);
  FuzzySet result;

  for (const auto &[b_key, _] : right.data_) {
    double max_val = 0.0;
    for (const auto &[a_prime_key, a_prime_value] : left_prime.data_) {
      std::pair<T, T> relation_key = std::make_pair(a_prime_key, b_key);
      if (R_relation.count(relation_key)) {
        double min_val =
            std::min(a_prime_value, R_relation.at(relation_key));
        max_val = std::max(max_val, min_val);
      }
    }
    result.insert(b_key, max_val);
  }
  return result;
}
template <typename T>
FuzzySet<T> FuzzySet<T>::GeneralizedModusTollens(
    const FuzzySet &right, const FuzzySet &right_prime) const {
  FuzMap R_relation = CRelation(*this, right);

  FuzzySet result;

  for (const auto &[b_key, _] : this->data_) {
    double max_val = 0.0;
    for (const auto &[a_prime_key, a_prime_value] : right_prime.data_) {
      std::pair<T, T> relation_key = std::make_pair(b_key, a_prime_key);
      std::cout << relation_key.first << " " << relation_key.second
                << std::endl;
      if (R_relation.count(relation_key)) {
        double min_val =
            std::min(a_prime_value, R_relation.at(relation_key));
        max_val = std::max(max_val, min_val);
      }
    }
    result.insert(b_key, max_val);
  }
  return result;
}

template <typename T>
std::unordered_map<T, double> &FuzzySet<T>::GetFuzzySet() {
  return this->data_;
}
template <typename T>
const std::unordered_map<T, double> &FuzzySet<T>::GetFuzzySet() const {
  return this->data_;
}
template <typename T>
FuzzySet<T> &FuzzySet<T>::operator+=(const FuzzySet &other) {
  return Unite(other);
}
template <typename T>
FuzzySet<T> &FuzzySet<T>::operator*=(const FuzzySet &other) {
  return Intersect(*this, other);
}
template <typename T>
FuzzySet<T> &FuzzySet<T>::operator-() {
  return Complement();
}
template <typename T>
FuzzySet<T> FuzzySet<T>::operator+(const FuzzySet &other) {
  return Union(other);
}
template <typename T>
FuzzySet<T> FuzzySet<T>::operator*(const FuzzySet &other) {
  return Intersect(other);
}

template <typename T>
template <bool Const>
FuzzySet<T>::Iterator<Const>::Iterator(map_iterator it) : it_(it) {}

template <typename T>
template <bool Const>
FuzzySet<T>::Iterator<Const>::Iterator(const Iterator<!Const> &other)
    : it_(other.it_) {}
template <typename T>
template <bool Const>
typename FuzzySet<T>::template Iterator<Const>::reference
FuzzySet<T>::Iterator<Const>::operator*() const {
  return *it_;
}
template <typename T>
template <bool Const>
typename FuzzySet<T>::template Iterator<Const>::pointer
FuzzySet<T>::Iterator<Const>::operator->() const {
  return &(*it_);
}
template <typename T>
template <bool Const>
typename FuzzySet<T>::template Iterator<Const> &
FuzzySet<T>::Iterator<Const>::operator++() {
  ++it_;
  return *this;
}
template <typename T>
template <bool Const>
typename FuzzySet<T>::template Iterator<Const>
FuzzySet<T>::Iterator<Const>::operator++(int) {
  Iterator temp = *this;
  ++(*this);
  return temp;
}
template <typename T>
template <bool Const>
typename FuzzySet<T>::template Iterator<Const> &
FuzzySet<T>::Iterator<Const>::operator--() {
  --it_;
  return *this;
}
template <typename T>
template <bool Const>
typename FuzzySet<T>::template Iterator<Const>
FuzzySet<T>::Iterator<Const>::operator--(int) {
  Iterator temp = *this;
  --(*this);
  return temp;
}
template <typename T>
template <bool Const>
bool FuzzySet<T>::Iterator<Const>::operator==(const Iterator &other) const {
  return it_ == other.it_;
}
template <typename T>
template <bool Const>
bool FuzzySet<T>::Iterator<Const>::operator!=(const Iterator &other) const {
  return it_ != other.it_;
}
template <typename T>
typename FuzzySet<T>::iterator FuzzySet<T>::begin() {
  return iterator(data_.begin());
}
template <typename T>
typename FuzzySet<T>::const_iterator FuzzySet<T>::begin() const {
  return const_iterator(data_.cbegin());
}
template <typename T>
typename FuzzySet<T>::const_iterator FuzzySet<T>::cbegin() const {
  return const_iterator(data_.cbegin());
}
template <typename T>
typename FuzzySet<T>::const_iterator FuzzySet<T>::end() const {
  return const_iterator(data_.cend());
}
template <typename T>
typename FuzzySet<T>::const_iterator FuzzySet<T>::cend() const {
  return const_iterator(data_.cend());
}
template <typename T>
typename FuzzySet<T>::iterator FuzzySet<T>::end() {
  return iterator(data_.end());
}

template <typename T>
FuzzySet<T> Union(const FuzzySet<T> &left, const FuzzySet<T> &right) {
  return left.Union(right);
}
template <typename T>
FuzzySet<T> Intersection(const FuzzySet<T> &left, const FuzzySet<T> &right) {
  return left.Intersection(right);
}
template <typename T>
FuzzySet<T> Complementation(const FuzzySet<T> &elem) {
  return elem.Complement();
}