/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include "meta/print.h"

#include <catch2/catch.hpp>
#include <optional>
#include <type_traits>

#include "fruits/Apple.h"
#include "fruits/Basket.h"
#include "meta/meta.h"
#include "meta/type_traits.h"

TEST_CASE("print1", "[meta::print]") {
  fruits::Apple apple;
  fruits::Basket basket;

  static_assert(
      std::is_same_v<decltype(fruits::meta::Apple::attributes::size_cm::get(
                         apple)),
                     double &>,
      "regression");

  apple.color = fruits::Color::red;
  apple.size_cm = 4.5;
  basket.apples.push_back(apple);

  apple.color = fruits::Color::yellow;
  apple.size_cm = 1.2;
  basket.apples.push_back(apple);

  apple.color = fruits::Color::green;
  apple.size_cm = 5.0;
  basket.apples.push_back(apple);

  std::ostringstream out;
  meta::print(basket, out);

  CHECK(out.str() == R"(Basket:
  apples=[
    Apple:
      color=red
      size_cm=4.5
    Apple:
      color=yellow
      size_cm=1.2
    Apple:
      color=green
      size_cm=5
  ]
)");
}
