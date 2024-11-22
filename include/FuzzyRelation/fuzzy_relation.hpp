#pragma once

#include <functional>
#include <vector>

#include "FuzzySets/fuzzy_set.hpp"

template <typename T>
class FuzzyRelation {
 public:
  FuzzyRelation(const std::vector<std::vector<T>>& data,
                const std::vector<T>& row, const std::vector<T>& collumn);
  explicit FuzzyRelation(
      const std::unordered_map<std::pair<T, T>, double, pair_hash,
                               pair_equal>& data);

  FuzzyRelation(const FuzzySet<T>& left, const FuzzySet<T>& right);
  FuzzyRelation(const FuzzySet<T>& left, const FuzzySet<T>& right,
                std::function<double(double, double)> relation_rule);

  FuzzyRelation& Unite(const FuzzyRelation& other);
  FuzzyRelation& Intersect(const FuzzyRelation& other);
  FuzzyRelation& Complement();

  FuzzyRelation Union(const FuzzyRelation& other) const;
  FuzzyRelation Intersection(const FuzzyRelation& other) const;
  FuzzyRelation Complementation() const;
  FuzzyRelation Composition(const FuzzyRelation& other) const;

  bool operator==(const FuzzyRelation& other);
  bool operator!=(const FuzzyRelation& other);

  typename FuzzySet<T>::FuzMap GetRelation();

 private:
  typename FuzzySet<T>::FuzMap data_;
};

template <typename T>
FuzzyRelation<T> Union(const FuzzyRelation<T>& left,
                       const FuzzyRelation<T>& right);
template <typename T>
FuzzyRelation<T> Intersection(const FuzzyRelation<T>& left,
                              const FuzzyRelation<T>& right);
template <typename T>
FuzzyRelation<T> Complementation(const FuzzyRelation<T>& object);
template <typename T>
FuzzyRelation<T> Composition(const FuzzyRelation<T>& left,
                             const FuzzyRelation<T>& right);
template <typename T>
FuzzyRelation<T> Implication(const FuzzySet<T>& left,
                             const FuzzySet<T>& right, ImplicationType type);
#include "fuzzy_relation.tpp"