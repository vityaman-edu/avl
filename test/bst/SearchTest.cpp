#include <gtest/gtest.h>

#include "bst/Example.hpp"
#include "bst/algo/Search.hpp"
#include "bst/naive/Node.hpp"

namespace bst {

TEST(BSTSearch, Simple) {
  NaiveNodeFactory<int> bst;

  auto* root = bst(5, bst(4, bst(3)), bst(8, bst(6), bst(9)));

  ASSERT_EQ(Search(root, 3)->Key(), 3);
  ASSERT_EQ(Search(root, 4)->Key(), 4);
  ASSERT_EQ(Search(root, 5)->Key(), 5);
  ASSERT_EQ(Search(root, 6)->Key(), 6);
  ASSERT_EQ(Search(root, 8)->Key(), 8);
  ASSERT_EQ(Search(root, 9)->Key(), 9);

  ASSERT_EQ(Search(root, 0), nullptr);
  ASSERT_EQ(Search(root, 2), nullptr);
  ASSERT_EQ(Search(root, 7), nullptr);
  ASSERT_EQ(Search(root, 10), nullptr);
}

TEST(BSTSearch, Singleton) {
  NaiveNode<int> node(5);
  ASSERT_EQ(Search(&node, 5), &node);
  ASSERT_EQ(Search(&node, 0), nullptr);
  ASSERT_EQ(Search(&node, 10), nullptr);
}

}  // namespace bst
