#include "avl/bst/NodeFactory.hpp"

namespace avl::test {

struct Node {
  using Key = int;

  Key key = 0;
  Node* parent = nullptr;
  Node* left = nullptr;
  Node* right = nullptr;
};

/**
 * Example
 * ---
 *      8
 *   3   10
 * 1  6     14
 *   4 7  13
 */
inline std::tuple<NodeFactory<Node>, Node*> Example() {
  NodeFactory<Node> bst;
  Node* root =
      bst(8,
          bst(3, bst(1), bst(6, bst(4), bst(7))),
          bst(10, nullptr, bst(14, bst(13))));
  return {std::move(bst), root};
}

}  // namespace avl::test
