#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bst/Example.hpp"
#include "bst/Extreme.hpp"
#include "bst/Node.hpp"
#include "bst/naive/Node.hpp"
#include "bst/naive/Tree.hpp"
#include "bst/set/MutIterator.hpp"

namespace bst::set {

template <BSTNode Node>
class BSTKeyRange {
public:
  explicit BSTKeyRange(Node* node) : node_(node) {
  }

  MutIterator<Node> begin() {
    if (node_ == nullptr) {
      return end();
    }
    return MutIterator<Node>(Min(node_));
  }

  MutIterator<Node> end() {
    return MutIterator<Node>();
  }

private:
  Node* node_;
};

TEST(MutIterator, Empty) {
  NaiveTree<int> tree;
  auto range = BSTKeyRange<NaiveNode<int>>(tree.Root());
  ASSERT_EQ(range.begin(), range.end());
}

TEST(MutIterator, Forward) {
  using Tree = NaiveTree<int>;
  using Node = Tree::Node;

  static_assert(std::bidirectional_iterator<MutIterator<Node>>);
  static_assert(std::ranges::bidirectional_range<BSTKeyRange<Node>>);

  auto [nodes, tree] = Example<int>();

  std::vector<Node::Key> keys;
  for (const auto& key : BSTKeyRange(tree.Root())) {
    keys.emplace_back(key);
  }

  ASSERT_THAT(keys, testing::ElementsAreArray({1, 3, 4, 6, 7, 8, 10, 13, 14}));
  ASSERT_TRUE(std::ranges::is_sorted(BSTKeyRange(tree.Root())));
}

}  // namespace bst::set