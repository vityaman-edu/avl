#pragma once

#include <cassert>
#include <iterator>

#include "bst/algo/Adjacent.hpp"
#include "bst/core/Node.hpp"

namespace bst::set {

template <BSTNode Node>
class MutIterator final {
public:
  using iterator_category = std::bidirectional_iterator_tag;  // NOLINT
  using value_type = Node::Key;                               // NOLINT
  using difference_type = std::ptrdiff_t;                     // NOLINT
  using pointer = Node::Key*;                                 // NOLINT
  using reference = Node::Key&;                               // NOLINT

  MutIterator() = default;

  explicit MutIterator(Node* node) : node_(node) {
    assert(node != nullptr);
  }

  Node::Key& operator*() const {
    return node_->key;
  }

  MutIterator& operator++() {
    node_ = Successor(node_);
    return *this;
  }

  MutIterator operator++(int) {  // NOLINT
    auto prev = *this;
    ++*this;
    return prev;
  }

  MutIterator& operator--() {
    node_ = Predecessor(node_);
    return *this;
  }

  MutIterator operator--(int) {  // NOLINT
    auto next = *this;
    --*this;
    return next;
  }

  bool operator==(const MutIterator& rhs) const {
    return this->node_ == rhs.node_;
  }

  bool operator!=(const MutIterator& rhs) const {
    return !(*this == rhs);
  }

private:
  Node* node_ = nullptr;
};

}  // namespace bst::set