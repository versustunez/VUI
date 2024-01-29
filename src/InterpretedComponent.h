#pragma once

#include "TypeDefs.h"

#include <variant>

namespace VUI {
struct Position {
  int X{0}, Y{0}, Width{0}, Height{0};
};

struct Color {
  int R{0}, G{0}, B{0};
  float A{0};
};

struct Property {
  std::string Name{};
  std::variant<std::string, double> Value{};
};

struct InterpretedComponent {
  InterpretedComponent() = default;
  InterpretedComponent(const InterpretedComponent &) = delete;
  InterpretedComponent(InterpretedComponent &&) = default;

  std::vector<InterpretedComponent> Children{};
  std::vector<Property> Properties{};
  Position ComponentPosition{};
  std::string ComponentName{};
  Color BackgroundColor{};
  bool NeedsColorLookup{false};
};
} // namespace VUI