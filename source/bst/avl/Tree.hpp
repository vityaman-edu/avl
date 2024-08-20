#pragma once

#include <cassert>
#include <cmath>
#include <compare>
#include <cstdlib>
#include <tuple>

#include "bst/algo/Adjacent.hpp"
#include "bst/algo/Rotate.hpp"
#include "bst/algo/Search.hpp"
#include "bst/avl/Bias.hpp"
#include "bst/avl/Height.hpp"
#include "bst/avl/Node.hpp"
#include "bst/core/Node.hpp"
#include "bst/core/Side.hpp"
#include "bst/support/Defer.hpp"

namespace bst::avl {

template <WeaklyOrdered K>
struct AVLTree {
public:
  using Node = AVLNode<K>;

  AVLTree() = default;

  AVLTree(const AVLTree&) = delete;
  AVLTree& operator=(const AVLTree&) = delete;

  AVLTree(AVLTree&& that) noexcept {
    Swap(that);
  }

  AVLTree& operator=(AVLTree&& that) noexcept {
    Swap(that);
    return *this;
  }

  ~AVLTree() = default;

  bool Insert(Node* node) {
    Before();
    Defer after([&] { After(); });

    if (Root() == nullptr) {
      LinkChild(Nil(), Side::LEFT, node);
      return true;
    }

    auto [parent, order] = SearchParent(Root(), node->Key());
    if (order == std::weak_ordering::equivalent) {
      return false;
    }

    auto side = SideOf(order);
    LinkChild(parent, side, node);
    OnInsertFixup(parent, side);

    return true;
  }

  void Remove(Node* node) {
    Before();
    Defer after([&] { After(); });

    struct {
      Node* node;
      Side side;
    } shrinked;

    if (node->Child(Side::LEFT) == nullptr && node->Child(Side::RIGHT) == nullptr) {
      shrinked = {.node = node->Parent(), .side = SideOf(node)};
      node->Parent()->SetChild(SideOf(node), nullptr);
    } else if (node->Child(Side::LEFT) == nullptr || node->Child(Side::RIGHT) == nullptr) {
      shrinked = {.node = node->Parent(), .side = SideOf(node)};
      Node* child =
          node->Child(Side::LEFT) == nullptr ? node->Child(Side::RIGHT) : node->Child(Side::LEFT);
      LinkChild(node->Parent(), SideOf(node), child);
    } else {
      Node* successor = Successor(node);

      shrinked = {.node = successor->Parent(), .side = SideOf(successor)};
      if (shrinked.node == node) {
        shrinked.node = successor;
      }

      LinkChild(successor->Parent(), SideOf(successor), successor->Child(Side::RIGHT));

      successor->SetBias(node->Bias());
      LinkChild(node->Parent(), SideOf(node), successor);
      for (auto side : {Side::LEFT, Side::RIGHT}) {
        LinkChild(successor, side, node->Child(side));
      }
    }

    OnRemoveFixup(shrinked.node, shrinked.side);
  }

  auto* Root(this auto& self) {
    return self.Nil()->Child(Side::LEFT);
  }

private:
  void OnInsertFixup(Node* parent, Side side) {
    AdjustBias(parent, side);
    if (parent->Bias() == Bias::NONE) {
      return;
    }

    for (                                               //
        Node *prev = parent, *next = parent->Parent();  //
        next != Nil();                                  //
        prev = next, next = next->Parent()              //
    ) {
      if (OnChildGrowthFixup(SideOf(prev), next)) {
        break;
      }
    }
  }

  bool OnChildGrowthFixup(Side side, Node* parent) {
    if (parent->Bias() != BiasOf(side)) {
      AdjustBias(parent, side);
      return parent->Bias() == Bias::NONE;
    }

    if (parent->Child(side)->Bias() == BiasOf(side)) {
      AdjustBias(parent, -side);
      AdjustBias(parent->Child(side), -side);
      Rotate(-side, parent);
    } else {
      BiasedDoubleRotate(-side, parent);
    }

    return true;
  }

  void OnRemoveFixup(Node* parent, Side side) {
    while (parent != Nil()) {
      auto next = std::tuple(parent->Parent(), SideOf(parent));
      if (OnChildShrinkedFixup(side, parent)) {
        break;
      }
      std::tie(parent, side) = next;
    }
  }

  bool OnChildShrinkedFixup(Side side, Node* parent) {
    if (parent->Bias() != BiasOf(-side)) {
      AdjustBias(parent, -side);
      return parent->Bias() != Bias::NONE;
    }

    Node* node = parent->Child(-side);

    if (node->Bias() != BiasOf(side)) {
      Rotate(side, parent);
      if (node->Bias() != Bias::NONE) {
        AdjustBias(parent, side);
      }
      AdjustBias(node, side);
      return node->Bias() != Bias::NONE;
    }

    BiasedDoubleRotate(side, parent);
    return false;
  }

  void BiasedDoubleRotate(Side side, Node* upper) {
    Node* midle = upper->Child(-side);
    Node* lower = midle->Child(side);

    upper->SetBias(((BiasOf(-side) == lower->Bias()) ? (-lower->Bias()) : (Bias::NONE)));
    midle->SetBias(((BiasOf(side) == lower->Bias()) ? (-lower->Bias()) : (Bias::NONE)));
    lower->SetBias(Bias::NONE);

    DoubleRotate(side, upper);
  }

  static void AdjustBias(Node* node, Side side) {
    node->SetBias(node->Bias() + BiasOf(side));
  }

  void Before() {
    EnsureSanity();
    if (Root() != nullptr) {
      Root()->SetParent(Nil());
    }
  }

  void After() {
    if (Root() != nullptr) {
      Root()->SetParent(nullptr);
    }
    EnsureSanity();
  }

  auto* Nil(this auto& self) {
    return &self.nil_;
  }

  void Swap(AVLTree& that) {
    const Side root_side = Side::LEFT;
    Node* this_root = this->Nil()->Child(root_side);
    Node* that_root = that.Nil()->Child(root_side);
    this->Nil()->SetChild(root_side, that_root);
    that.Nil()->SetChild(root_side, this_root);
  }

  void EnsureSanity() {
    assert(Root() == nullptr || Root()->Parent() == nullptr);
    assert(Height(Root()) >= 0);
  }

  Node nil_ = {};
};

}  // namespace bst::avl