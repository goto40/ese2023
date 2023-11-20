/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include "meta/program_options.h"

#include <catch2/catch.hpp>
#include <optional>
#include <type_traits>

#include "fruits/Apple.h"
#include "fruits/Basket.h"
#include "fruits/Color.h"
#include "meta/meta.h"
#include "meta/print.h"
#include "meta/type_traits.h"

TEST_CASE("po1", "[meta::program_options]") {
  {
    int argc = 5;
    const char* argv[] = {"app.exe", "--color", "red", "--size_cm", "1.2"};
    auto apple = meta::from_program_options<fruits::Apple>(argc, argv);
    CHECK(apple.color == fruits::Color::red);
    CHECK(apple.size_cm == 1.2);
  }
  {
    int argc = 3;
    const char* argv[] = {"app.exe", "--size_cm", "1.2"};
    CHECK_THROWS(meta::from_program_options<fruits::Apple>(argc, argv));
  }
  {
    int argc = 4;
    const char* argv[] = {"app.exe", "--color", "green", "--size_cm"};
    CHECK_THROWS(meta::from_program_options<fruits::Apple>(argc, argv));
  }
}
