#include <gtest/gtest.h>
#include <unordered_map>

#include "FuzzySets/fuzzy_set.hpp"

TEST(FuzzySetTestSuite, SetCreationEmptyTest) {
  FuzzySet<int> set;
  ASSERT_TRUE(set.GetFuzzySet().empty());
}

TEST(FuzzySetTestSuite, SetCreationTest) {
  FuzzySet<int> set({{1, 0.5}, {3, 0.1}, {4, 0.5}, {5, 0.9}});
  std::unordered_map<int, double> map(
      {{1, 0.5}, {3, 0.1}, {4, 0.5}, {5, 0.9}});
  ASSERT_EQ(set.GetFuzzySet(), map);
}

TEST(FuzzySetTestSuite, SetCreationErrorTest) {
  FuzzySet<int> set({{1, 0.4}, {3, 1.1}, {4, 0.5}, {5, 0.9}});
  std::unordered_map<int, double> map({{1, 0.4}, {4, 0.5}, {5, 0.9}});
  ASSERT_EQ(set.GetFuzzySet(), map);
}

TEST(FuzzySetTestSuite, SetMoveTest) {
  FuzzySet<int> set({{1, 0.4}, {4, 0.5}, {5, 0.9}});
  FuzzySet<int> set2 = std::move(set);
  std::unordered_map<int, double> map({{1, 0.4}, {4, 0.5}, {5, 0.9}});
  ASSERT_EQ(set2.GetFuzzySet(), map);
}

TEST(FuzzySetTestSuite, SetCopyTest) {
  FuzzySet<int> set1({{1, 0.4}, {4, 0.5}, {5, 0.9}});
  FuzzySet<int> set2(set1);
  ASSERT_EQ(set1, set2);
}

TEST(FuzzySetTestSuite, SetGMTTest) {
  const FuzzySet<std::string> A = {
      {"x1", 0.5},
      {"x2", 1.0},
      {"x3", 0.6},
  };
  const FuzzySet<std::string> B = {{"y1", 1}, {"y2", 0.4}};
  const FuzzySet<std::string> B_prime = {
      {"y1", 0.9},
      {"y2", 0.7},

  };

  auto res = A.GeneralizedModusTollens(B, B_prime);
  ASSERT_EQ(res,
            FuzzySet<std::string>({{"x1", 0.5}, {"x2", 0.9}, {"x3", 0.6}}));
}
TEST(FuzzySetTestSuite, SetGMPTest) {
  const FuzzySet<std::string> A = {
      {"x1", 0.5},
      {"x2", 1.0},
      {"x3", 0.6},
  };
  const FuzzySet<std::string> B = {{"y1", 1}, {"y2", 0.4}};

  const FuzzySet<std::string> A_prime = {
      {"x1", 0.6},
      {"x2", 0.9},
      {"x3", 0.7},
  };

  auto res = A.GeneralizedModusPonens(B, A_prime);
  ASSERT_EQ(res, FuzzySet<std::string>({{"y1", 0.9}, {"y2", 0.5}}));
}
TEST(FuzzySetTestSuite, UnionTest) {
  FuzzySet<int> set1({{1, 0.5}, {2, 0.3}, {3, 0.7}});
  FuzzySet<int> set2({{2, 0.4}, {3, 0.8}, {4, 0.6}});

  FuzzySet<int> unionSet = set1.Union(set2);
  std::unordered_map<int, double> expectedUnion = {
      {1, 0.5}, {2, 0.4}, {3, 0.8}, {4, 0.6}};

  ASSERT_EQ(unionSet.GetFuzzySet(), expectedUnion);
}
TEST(FuzzySetTestSuite, IntersectionTest) {
  FuzzySet<int> set1({{1, 0.5}, {2, 0.3}, {3, 0.7}});
  FuzzySet<int> set2({{2, 0.4}, {3, 0.8}, {4, 0.6}});

  FuzzySet<int> intersectionSet = set1.Intersection(set2);
  std::unordered_map<int, double> expectedIntersection = {{2, 0.3},
                                                          {3, 0.7}};

  ASSERT_EQ(intersectionSet.GetFuzzySet(), expectedIntersection);
}
TEST(FuzzySetTestSuite, ComplementationTest) {
  FuzzySet<int> set({{1, 0.5}, {2, 0.3}, {3, 0.7}});

  FuzzySet<int> complementSet = set.Complementation();
  std::unordered_map<int, double> expectedComplement = {
      {1, 0.5}, {2, 0.7}, {3, 0.3}};

  ASSERT_EQ(complementSet, FuzzySet(expectedComplement));
}
TEST(FuzzySetTestSuite, ImplicateAlgProductTest) {
  FuzzySet<int> set1({{1, 0.5}, {2, 0.3}, {3, 0.7}});
  FuzzySet<int> set2({{2, 0.4}, {3, 0.8}, {4, 0.6}});

  set1.Implicate(set2, ImplicationType::ALGPRODUCT);
  FuzzySet<int> expectedImplication(
      {{1, 0.0}, {2, 0.12}, {3, 0.56}, {4, 0}});

  ASSERT_EQ(set1, expectedImplication);
}
TEST(FuzzySetTestSuite, ImplicateMinimumTest) {
  FuzzySet<int> set1({{1, 0.5}, {2, 0.3}, {3, 0.7}});
  FuzzySet<int> set2({{2, 0.4}, {3, 0.8}, {4, 0.6}});

  set1.Implicate(set2, ImplicationType::MINIMUM);
  FuzzySet<int> expectedImplication(
      {{1, 0.0}, {2, 0.3}, {3, 0.7}, {4, 0.0}});

  ASSERT_EQ(set1, expectedImplication);
}
TEST(FuzzySetTestSuite, ImplicateZadehArithmeticTest) {
  FuzzySet<int> set1({{1, 0.5}, {2, 0.3}, {3, 0.7}});
  FuzzySet<int> set2({{2, 0.4}, {3, 0.8}, {4, 0.6}});

  set1.Implicate(set2, ImplicationType::ZADEHARITHMETIC);
  std::unordered_map<int, double> expectedImplication = {
      {1, 0.5}, {2, 1.0}, {3, 1.0}, {4, 0}};

  ASSERT_EQ(set1.GetFuzzySet(), expectedImplication);
}
TEST(FuzzySetTestSuite, ImplicateZadehMaxMinTest) {
  FuzzySet<int> set1({{1, 0.5}, {2, 0.3}, {3, 0.7}});
  FuzzySet<int> set2({{2, 0.4}, {3, 0.8}, {4, 0.6}});

  set1.Implicate(set2, ImplicationType::ZADEHMAXMIN);
  std::unordered_map<int, double> expectedImplication = {
      {1, 0.5}, {2, 0.7}, {3, 0.8}, {4, 0}};

  ASSERT_EQ(set1, FuzzySet(expectedImplication));
}
TEST(FuzzySetTestSuite, InsertEraseTest) {
  FuzzySet<int> set;

  set.insert({1, 0.5});
  set.insert({2, 0.3});
  set.insert({3, 0.7});

  std::unordered_map<int, double> expectedInsert = {
      {1, 0.5}, {2, 0.3}, {3, 0.7}};
  ASSERT_EQ(set.GetFuzzySet(), expectedInsert);

  set.erase(2);
  std::unordered_map<int, double> expectedErase = {{1, 0.5}, {3, 0.7}};
  ASSERT_EQ(set.GetFuzzySet(), expectedErase);
}
TEST(FuzzySetTestSuite, ImplicateIdenticalSetsTest) {
  FuzzySet<int> set1({{1, 0.5}, {2, 0.3}, {3, 0.7}});
  FuzzySet<int> set2({{1, 0.5}, {2, 0.3}, {3, 0.7}});

  set1.Implicate(set2, ImplicationType::MINIMUM);
  std::unordered_map<int, double> expectedImplication = {
      {1, 0.5}, {2, 0.3}, {3, 0.7}};

  ASSERT_EQ(set1.GetFuzzySet(), expectedImplication);
}
TEST(FuzzySetTestSuite, OperatorOverloadsTest) {
  FuzzySet<int> set1({{1, 0.5}, {2, 0.3}, {3, 0.7}});
  FuzzySet<int> set2({{2, 0.4}, {3, 0.8}, {4, 0.6}});

  FuzzySet<int> sumSet = set1 + set2;
  std::unordered_map<int, double> expectedSum = {
      {1, 0.5}, {2, 0.4}, {3, 0.8}, {4, 0.6}};
  ASSERT_EQ(sumSet, FuzzySet<int>(expectedSum));

  FuzzySet<int> productSet = set1 * set2;
  std::unordered_map<int, double> expectedProduct = {{2, 0.3}, {3, 0.7}};
  ASSERT_EQ(productSet.GetFuzzySet(), expectedProduct);
}

TEST(FuzzySetTestSuite, IteratorTest) {
  FuzzySet<int> set({{1, 0.5}, {2, 0.3}, {3, 0.7}});

  std::unordered_map<int, double> iteratedMap;
  for (auto it = set.begin(); it != set.end(); ++it) {
    iteratedMap[it->first] = it->second;
  }

  std::unordered_map<int, double> expectedMap = {
      {1, 0.5}, {2, 0.3}, {3, 0.7}};
  ASSERT_EQ(iteratedMap, expectedMap);
}

TEST(FuzzySetTestSuite, EqualityInequalityTest) {
  FuzzySet<int> set1({{1, 0.5}, {2, 0.3}, {3, 0.7}});
  FuzzySet<int> set2({{1, 0.5}, {2, 0.3}, {3, 0.7}});
  FuzzySet<int> set3({{1, 0.5}, {2, 0.4}, {3, 0.7}});

  ASSERT_TRUE(set1 == set2);
  ASSERT_FALSE(set1 == set3);
  ASSERT_TRUE(set1 != set3);
}
TEST(FuzzySetTestSuite, AccessOperatorsTest) {
  FuzzySet<int> set({{1, 0.5}, {2, 0.3}, {3, 0.7}});

  ASSERT_EQ(set.at(1), 0.5);
  ASSERT_EQ(set[2], 0.3);

  set[2] = 0.4;
  ASSERT_EQ(set[2], 0.4);
}
