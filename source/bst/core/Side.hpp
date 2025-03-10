#pragma once

#include <compare>
#include <cstdint>

namespace bst {

enum class Side : std::int8_t { LEFT = -1, RIGHT = +1 };

Side operator-(Side side);

Side SideOf(std::weak_ordering order);

}  // namespace bst