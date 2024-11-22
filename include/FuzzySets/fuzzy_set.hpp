#pragma once

#include <cstddef>
#include <initializer_list>
#include <unordered_map>
#include <unordered_set>

enum class ImplicationType {
  ALGPRODUCT,
  MINIMUM,
  ZADEHARITHMETIC,
  ZADEHMAXMIN,
};

struct pair_hash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2>& pair) const {
    auto hash1 = std::hash<T1>{}(pair.first);
    auto hash2 = std::hash<T2>{}(pair.second);
    return hash1 ^ hash2;
  }
};

struct pair_equal {
  template <class T1, class T2>
  bool operator()(const std::pair<T1, T2>& lhs,
                  const std::pair<T1, T2>& rhs) const {
    return lhs.first == rhs.first && lhs.second == rhs.second;
  }
};

template <typename T>
class FuzzySet {
 public:
  using FuzMap =
      std::unordered_map<std::pair<T, T>, double, pair_hash, pair_equal>;
  FuzzySet();
  FuzzySet(const FuzzySet<T>& other);
  FuzzySet(FuzzySet<T>&& other) noexcept;
  FuzzySet(const std::initializer_list<std::pair<T, double>>& data);
  explicit FuzzySet(const std::unordered_map<T, double>& data);
  FuzzySet& operator=(const FuzzySet<T>& other);
  FuzzySet& operator=(FuzzySet<T>&& other) noexcept;
  ~FuzzySet() = default;

  auto operator<=>(const FuzzySet<T>& other) const;
  auto operator==(const FuzzySet<T>& other) const;
  auto operator!=(const FuzzySet<T>& other) const;

  const double& at(const T& element) const;
  double& operator[](const T& element);

  void insert(const std::pair<T, double>& element);
  void insert(const T& key, double value);

  void erase(const std::pair<T, double>& element);
  void erase(const T& key);

  FuzzySet& Unite(const FuzzySet<T>& other);
  FuzzySet& Intersect(const FuzzySet<T>& other);
  FuzzySet& Complement();
  FuzzySet& Implicate(const FuzzySet<T>& other,
                      ImplicationType type = ImplicationType::MINIMUM);

  FuzzySet Union(const FuzzySet& other) const;

  FuzzySet Intersection(const FuzzySet& other) const;

  FuzzySet Complementation() const;

  FuzzySet Implication(
      const FuzzySet<T>& other,
      ImplicationType type = ImplicationType::MINIMUM) const;

  FuzzySet GeneralizedModusPonens(const FuzzySet& right,
                                  const FuzzySet& left_prime) const;
  FuzzySet GeneralizedModusTollens(const FuzzySet& right,
                                   const FuzzySet& right_prime) const;

  const std::unordered_map<T, double>& GetFuzzySet() [[nodiscard]];

  FuzzySet& operator+=(const FuzzySet& other);
  FuzzySet& operator*=(const FuzzySet& other);
  FuzzySet& operator-();

  FuzzySet operator+(const FuzzySet& other) [[nodiscard]];
  FuzzySet operator*(const FuzzySet& other) [[nodiscard]];

  template <bool Const>
  class Iterator {
   public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::pair<const T, double>;
    using pointer =
        std::conditional_t<Const, const value_type*, value_type*>;
    using reference =
        std::conditional_t<Const, const value_type&, value_type&>;
    using iterator_category = std::bidirectional_iterator_tag;
    using iterator_concept = iterator_category;

    using map_iterator = std::conditional_t<
        Const, typename std::unordered_map<T, double>::const_iterator,
        typename std::unordered_map<T, double>::iterator>;

    explicit Iterator(map_iterator it);
    explicit Iterator(const Iterator<!Const>& other);
    reference operator*() const;
    pointer operator->() const;

    Iterator& operator++();

    Iterator operator++(int);

    Iterator& operator--();

    Iterator operator--(int);

    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;

   private:
    map_iterator it_;
    friend class Iterator<!Const>;
  };

  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;

  [[nodiscard]] iterator begin();
  [[nodiscard]] const_iterator begin() const;
  [[nodiscard]] const_iterator cbegin() const;

  [[nodiscard]] const_iterator end() const;
  [[nodiscard]] const_iterator cend() const;
  [[nodiscard]] iterator end();

 private:
  std::unordered_map<T, double> data_;
};

template <typename T>
FuzzySet<T> Union(const FuzzySet<T>& left, const FuzzySet<T>& right);
template <typename T>
FuzzySet<T> Intersection(const FuzzySet<T>& left, const FuzzySet<T>& right);
template <typename T>
FuzzySet<T> Complementation(const FuzzySet<T>& elem);

#include "fuzzy_set.tpp"