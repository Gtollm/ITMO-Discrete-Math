#include <gtest/gtest.h>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "FuzzyRelation/fuzzy_relation.hpp"
#include "FuzzySets/fuzzy_set.hpp"

template <typename T>
FuzzySet<T> CreateFuzzySet(const std::vector<T>& elements,
                           const std::vector<double>& memberships) {
  FuzzySet<T> fuzzySet;
  for (size_t i = 0; i < elements.size(); ++i) {
    fuzzySet.insert(elements[i], memberships[i]);
  }
  return fuzzySet;
}

TEST(FuzzyRelationTestSuite, ConstructorMatrixTest) {
  std::vector<std::vector<double>> data = {{0.1, 0.2}, {0.3, 0.4}};
  std::vector<double> row = {1.0, 2.0};
  std::vector<double> column = {3.0, 4.0};

  FuzzyRelation<double> fuzzyRelation(data, row, column);

  auto relation = fuzzyRelation.GetRelation();
  ASSERT_EQ(relation.size(), 4);
  ASSERT_EQ(relation[std::make_pair(1.0, 3.0)], 0.1);
  ASSERT_EQ(relation[std::make_pair(1.0, 4.0)], 0.2);
  ASSERT_EQ(relation[std::make_pair(2.0, 3.0)], 0.3);
  ASSERT_EQ(relation[std::make_pair(2.0, 4.0)], 0.4);
}

TEST(FuzzyRelationTestSuite, ConstructorUnorderedMapTest) {
  std::unordered_map<std::pair<double, double>, double, pair_hash,
                     pair_equal>
      data = {{std::make_pair(1.0, 3.0), 0.1},
              {std::make_pair(1.0, 4.0), 0.2},
              {std::make_pair(2.0, 3.0), 0.3},
              {std::make_pair(2.0, 4.0), 0.4}};

  FuzzyRelation<double> fuzzyRelation(data);

  auto relation = fuzzyRelation.GetRelation();
  ASSERT_EQ(relation.size(), 4);
  ASSERT_EQ(relation[std::make_pair(1.0, 3.0)], 0.1);
  ASSERT_EQ(relation[std::make_pair(1.0, 4.0)], 0.2);
  ASSERT_EQ(relation[std::make_pair(2.0, 3.0)], 0.3);
  ASSERT_EQ(relation[std::make_pair(2.0, 4.0)], 0.4);
}

TEST(FuzzyRelationTestSuite, ConstructorFuzzySetsTest) {
  FuzzySet<double> leftSet = CreateFuzzySet<double>({1.0, 2.0}, {0.5, 0.6});
  FuzzySet<double> rightSet = CreateFuzzySet<double>({3.0, 4.0}, {0.7, 0.8});

  FuzzyRelation<double> fuzzyRelation(leftSet, rightSet);

  auto relation = fuzzyRelation.GetRelation();
  ASSERT_EQ(relation.size(), 4);
  ASSERT_EQ(relation[std::make_pair(1.0, 3.0)], 0.5);
  ASSERT_EQ(relation[std::make_pair(1.0, 4.0)], 0.5);
  ASSERT_EQ(relation[std::make_pair(2.0, 3.0)], 0.6);
  ASSERT_EQ(relation[std::make_pair(2.0, 4.0)], 0.6);
}

TEST(FuzzyRelationTestSuite, UnionTest) {
  FuzzySet<double> leftSet = CreateFuzzySet<double>({1.0, 2.0}, {0.5, 0.6});
  FuzzySet<double> rightSet = CreateFuzzySet<double>({3.0, 4.0}, {0.7, 0.8});

  FuzzyRelation<double> leftRelation(leftSet, rightSet);
  FuzzyRelation<double> rightRelation(rightSet, leftSet);

  ASSERT_THROW(leftRelation.Union(rightRelation), std::runtime_error);
}

TEST(FuzzyRelationTestSuite, IntersectionTest) {
  FuzzySet<double> leftSet = CreateFuzzySet<double>({1.0, 2.0}, {0.5, 0.6});
  FuzzySet<double> rightSet = CreateFuzzySet<double>({3.0, 4.0}, {0.7, 0.8});

  FuzzyRelation<double> leftRelation(leftSet, rightSet);
  FuzzyRelation<double> rightRelation(rightSet, leftSet);

  ASSERT_THROW(leftRelation.Intersection(rightRelation), std::runtime_error);
}

TEST(FuzzyRelationTestSuite, ComplementationTest) {
  FuzzySet<double> set = CreateFuzzySet<double>({1.0, 2.0}, {0.5, 0.6});
  FuzzyRelation<double> relation(set, set);

  FuzzyRelation<double> complementRelation = relation.Complementation();

  auto relationMap = complementRelation.GetRelation();
  ASSERT_EQ(relationMap.size(), 4);
  ASSERT_EQ(relationMap[std::make_pair(1.0, 1.0)], 0.5);
  ASSERT_EQ(relationMap[std::make_pair(2.0, 2.0)], 0.4);
}

TEST(FuzzyRelationTestSuite, CompositionTest) {
  std::vector<double> X = {1.0, 2.0};
  std::vector<double> Y = {3.0, 4.0};
  std::vector<double> Z = {5.0, 6.0};

  FuzzySet<double> setX = CreateFuzzySet<double>(X, {1.0, 1.0});
  FuzzySet<double> setY = CreateFuzzySet<double>(Y, {1.0, 1.0});
  FuzzySet<double> setZ = CreateFuzzySet<double>(Z, {1.0, 1.0});

  std::unordered_map<std::pair<double, double>, double, pair_hash,
                     pair_equal>
      R_data = {{std::make_pair(1.0, 3.0), 0.5},
                {std::make_pair(1.0, 4.0), 0.6},
                {std::make_pair(2.0, 3.0), 0.7},
                {std::make_pair(2.0, 4.0), 0.8}};

  std::unordered_map<std::pair<double, double>, double, pair_hash,
                     pair_equal>
      S_data = {{std::make_pair(3.0, 5.0), 0.9},
                {std::make_pair(3.0, 6.0), 0.8},
                {std::make_pair(4.0, 5.0), 0.7},
                {std::make_pair(4.0, 6.0), 0.6}};

  FuzzyRelation<double> R(R_data);
  FuzzyRelation<double> S(S_data);

  FuzzyRelation<double> compositionRelation = R.Composition(S);

  std::unordered_map<std::pair<double, double>, double, pair_hash,
                     pair_equal>
      expectedComposition = {{std::make_pair(1.0, 5.0), 0.6},
                             {std::make_pair(1.0, 6.0), 0.6},
                             {std::make_pair(2.0, 5.0), 0.7},
                             {std::make_pair(2.0, 6.0), 0.7}};

  auto relation = compositionRelation.GetRelation();
  ASSERT_EQ(relation.size(), expectedComposition.size());
  for (const auto& pair : expectedComposition) {
    ASSERT_EQ(relation.at(pair.first), pair.second);
  }
}

TEST(FuzzyRelationTestSuite, EqualityOperatorTest) {
  FuzzySet<double> set1 = CreateFuzzySet<double>({1.0, 2.0}, {0.5, 0.6});
  FuzzySet<double> set2 = CreateFuzzySet<double>({3.0, 4.0}, {0.7, 0.8});

  FuzzyRelation<double> relation1(set1, set2);
  FuzzyRelation<double> relation2(set1, set2);

  ASSERT_TRUE(relation1 == relation2);
}

TEST(FuzzyRelationTestSuite, InequalityOperatorTest) {
  FuzzySet<double> set1 = CreateFuzzySet<double>({1.0, 2.0}, {0.5, 0.6});
  FuzzySet<double> set2 = CreateFuzzySet<double>({3.0, 4.0}, {0.7, 0.8});
  FuzzySet<double> set3 = CreateFuzzySet<double>({5.0, 6.0}, {0.9, 1.0});

  FuzzyRelation<double> relation1(set1, set2);
  FuzzyRelation<double> relation2(set1, set3);

  ASSERT_TRUE(relation1 != relation2);
}

TEST(FuzzyRelationTestSuite, ImplicationMinimumTest) {
  FuzzySet<double> leftSet = CreateFuzzySet<double>({1.0, 2.0}, {0.5, 0.6});
  FuzzySet<double> rightSet = CreateFuzzySet<double>({3.0, 4.0}, {0.7, 0.8});

  FuzzyRelation<double> implicationRelation =
      Implication(leftSet, rightSet, ImplicationType::MINIMUM);

  auto relation = implicationRelation.GetRelation();
  ASSERT_EQ(relation.size(), 4);
  ASSERT_EQ(relation.at(std::make_pair(1.0, 3.0)), 0.5);
  ASSERT_EQ(relation.at(std::make_pair(1.0, 4.0)), 0.5);
  ASSERT_EQ(relation.at(std::make_pair(2.0, 3.0)), 0.6);
  ASSERT_EQ(relation.at(std::make_pair(2.0, 4.0)), 0.6);
}

TEST(FuzzyRelationTestSuite, ImplicationAlgProductTest) {
  FuzzySet<double> leftSet = CreateFuzzySet<double>({1.0, 2.0}, {0.5, 0.6});
  FuzzySet<double> rightSet = CreateFuzzySet<double>({3.0, 4.0}, {0.7, 0.8});

  FuzzyRelation<double> implicationRelation =
      Implication(leftSet, rightSet, ImplicationType::ALGPRODUCT);

  auto relation = implicationRelation.GetRelation();
  ASSERT_EQ(relation.size(), 4);
  ASSERT_EQ(relation.at(std::make_pair(1.0, 3.0)), 0.35);
  ASSERT_EQ(relation.at(std::make_pair(1.0, 4.0)), 0.4);
  ASSERT_EQ(relation.at(std::make_pair(2.0, 3.0)), 0.42);
  ASSERT_EQ(relation.at(std::make_pair(2.0, 4.0)), 0.48);
}

TEST(FuzzyRelationTestSuite, ImplicationZadehMaxMinTest) {
  FuzzySet<double> leftSet = CreateFuzzySet<double>({1.0, 2.0}, {0.5, 0.6});
  FuzzySet<double> rightSet = CreateFuzzySet<double>({3.0, 4.0}, {0.7, 0.8});

  FuzzyRelation<double> implicationRelation =
      Implication(leftSet, rightSet, ImplicationType::ZADEHMAXMIN);

  auto relation = implicationRelation.GetRelation();
  ASSERT_EQ(relation.size(), 4);
  ASSERT_EQ(relation.at(std::make_pair(1.0, 3.0)), 0.7);
  ASSERT_EQ(relation.at(std::make_pair(1.0, 4.0)), 0.8);
  ASSERT_EQ(relation.at(std::make_pair(2.0, 3.0)), 0.7);
  ASSERT_EQ(relation.at(std::make_pair(2.0, 4.0)), 0.8);
}

TEST(FuzzyRelationTestSuite, ImplicationZadehArithmeticTest) {
  FuzzySet<double> leftSet = CreateFuzzySet<double>({1.0, 2.0}, {0.5, 0.6});
  FuzzySet<double> rightSet = CreateFuzzySet<double>({3.0, 4.0}, {0.7, 0.8});

  FuzzyRelation<double> implicationRelation =
      Implication(leftSet, rightSet, ImplicationType::ZADEHARITHMETIC);

  auto relation = implicationRelation.GetRelation();
  ASSERT_EQ(relation.size(), 4);
  ASSERT_EQ(relation.at(std::make_pair(1.0, 3.0)), 0.5);
  ASSERT_EQ(relation.at(std::make_pair(1.0, 4.0)), 0.5);
  ASSERT_EQ(relation.at(std::make_pair(2.0, 3.0)), 0.6);
  ASSERT_EQ(relation.at(std::make_pair(2.0, 4.0)), 0.6);
}