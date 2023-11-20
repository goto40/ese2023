/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include <iostream>
#include <optional>

#include "fruits/Basket.h"
#include "meta/meta.h"
#include "meta/print.h"

int main() {
  static_assert(meta::struct_meta<fruits::meta::Basket>,
                "just checking... ;-)");

  fruits::Apple apple;
  fruits::Basket basket;

  apple.color = fruits::Color::red;
  apple.size_cm = 4.5;
  basket.apples.push_back(apple);

  apple.color = fruits::Color::yellow;
  apple.size_cm = 1.2;
  basket.apples.push_back(apple);

  apple.color = fruits::Color::green;
  apple.size_cm = 5.0;
  basket.apples.push_back(apple);

  for (double s = 10.0; s < 11.05; s += 0.1) {
    apple.color = fruits::Color::yellow;
    apple.size_cm = s;
    basket.apples.push_back(apple);
  }

  meta::print(basket);
}
