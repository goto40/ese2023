/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include "meta/json.h"

#include <algorithm>
#include <catch2/catch.hpp>
#include <cctype>
#include <optional>
#include <stdexcept>

#include "fruits/Basket.h"

TEST_CASE("json read", "[meta::json]") {
  std::string text = {R"(
        {
            "apples": [
                {"color": "green", "size_cm": 3.1},
                {"color": "red", "size_cm": 1.2},
                {"color": "yellow", "size_cm": 3.3}
            ]
        }
    )"};
  auto basket = meta::from_json<fruits::Basket>(text);

  CHECK(basket.apples.size() == 3);
  CHECK(basket.apples[0].color == fruits::Color::green);
  CHECK(basket.apples[1].color == fruits::Color::red);
  CHECK(basket.apples[2].color == fruits::Color::yellow);
  CHECK(basket.apples[0].size_cm == 3.1);
  CHECK(basket.apples[1].size_cm == 1.2);
  CHECK(basket.apples[2].size_cm == 3.3);

  std::string back = meta::to_json(basket);
  std::string reference = text;
  auto end = std::remove_if(reference.begin(), reference.end(),
                            [](auto c) { return std::isspace(c); });
  reference.erase(end, reference.end());
  CHECK(back == reference);
}
