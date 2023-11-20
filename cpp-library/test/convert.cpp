/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include "meta/convert.h"

#include <catch2/catch.hpp>

#include "convert_helper.h"  // for unittests + assemby analysis
#include "example_com_schemas_examples/Complex.h"
#include "example_com_schemas_examples/Complex4.h"
#include "example_com_schemas_examples/ComplexD.h"
#include "example_com_schemas_examples/MyObj1.h"
#include "example_com_schemas_examples/MyObj2.h"
#include "example_com_schemas_examples/Simple.h"
#include "meta/meta.h"

namespace {
auto createExamples() {
  example_com_schemas_examples::MyObj1 o1;
  o1.f = 1.5;
  o1.i = 1;
  o1.s = "One";

  example_com_schemas_examples::MyObj2 o2;
  o2.f = 2.5;
  o2.i = 2;
  o2.s = "Two";
  o2.x = 2;

  return std::make_tuple(o1, o2);
}
}  // namespace

TEST_CASE("experiment_with_meta_info1", "[code gen]") {
  using Meta1_0 = meta::meta_by_index_t<
      0,
      example_com_schemas_examples::meta::MyObj1::ListOfAttributeMetaClasses>;
  using Meta1_1 = meta::meta_by_index_t<
      1,
      example_com_schemas_examples::meta::MyObj1::ListOfAttributeMetaClasses>;
  using Meta1_2 = meta::meta_by_index_t<
      2,
      example_com_schemas_examples::meta::MyObj1::ListOfAttributeMetaClasses>;

  CHECK(Meta1_0::name() == "s");
  CHECK(Meta1_1::name() == "i");
  CHECK(Meta1_2::name() == "f");

  using META_i_s =
      meta::meta_by_name_t<"s", example_com_schemas_examples::MyObj1>;
  using META_i_i =
      meta::meta_by_name_t<"i", example_com_schemas_examples::MyObj1>;
  using META_i_f =
      meta::meta_by_name_t<"f", example_com_schemas_examples::MyObj1>;

  static_assert(
      meta::has_meta_by_name_v<"s", example_com_schemas_examples::MyObj1>,
      "unittest");
  static_assert(
      meta::has_meta_by_name_v<"i", example_com_schemas_examples::MyObj1>,
      "unittest");
  static_assert(
      meta::has_meta_by_name_v<"f", example_com_schemas_examples::MyObj1>,
      "unittest");
  static_assert(
      !meta::has_meta_by_name_v<"error", example_com_schemas_examples::MyObj1>,
      "unittest");

  CHECK(META_i_s::name() == std::string("s"));
  CHECK(META_i_i::name() == std::string("i"));
  CHECK(META_i_f::name() == std::string("f"));
}

static_assert(meta::attribute_meta<
                  example_com_schemas_examples::meta::MyObj2::attributes::f>,
              "testing the concept IsMeta...");

TEST_CASE("experiment_with_meta_info2", "[code gen]") {
  auto [o1, o2] = createExamples();
  CHECK(o1.i != o2.i);
  CHECK(o1.s != o2.s);

  meta::convert_from_other_message(o2, o1);
  // meta::convert_from_other_message(o1, o2); // does not compile: opt<double>
  // does not convert to double

  constexpr auto oneConverterForS =
      meta::createCustomAttributeConverter<example_com_schemas_examples::MyObj1,
                                           "s">([](auto x) { return "text"; });
  constexpr bool sisS = oneConverterForS.getName().isSame("s");
  constexpr bool sisNotX = oneConverterForS.getName().isSame("x");
  CHECK(sisS);
  CHECK(!sisNotX);

  int64_t runtime_value = 42;
  meta::CustomStructConverter converter{
      // decide at runtime how
      // V-------------------------------V--------------------V
      meta::createCustomAttributeConverter<example_com_schemas_examples::MyObj1,
                                           "i">(
          [&](const auto&, auto& dest) { dest = runtime_value; }),
      meta::createCustomAttributeConverter<example_com_schemas_examples::MyObj1,
                                           "s">([](const auto&, auto& dest) {
        dest = "text";
      })};  // decide at compile time what ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            // ^^^

  constexpr bool ok1 = converter.hasConverterWithMessageTypeAndAttributeName<
      example_com_schemas_examples::MyObj1>("i");
  constexpr bool ok2 = converter.hasConverterWithMessageTypeAndAttributeName<
      example_com_schemas_examples::MyObj1>("s");
  constexpr bool bad1 = converter.hasConverterWithMessageTypeAndAttributeName<
      example_com_schemas_examples::MyObj1>("bad");

  CHECK(ok1);
  CHECK(ok2);
  CHECK(!bad1);

  constexpr meta::CustomStructConverter empty_converter{};

  constexpr bool bad2 =
      empty_converter.hasConverterWithMessageTypeAndAttributeName<
          example_com_schemas_examples::MyObj1>("i");
  CHECK(!bad2);

  o1.i = 9;
  meta::convert_from_other_message(o2, o1, converter);
  CHECK(o2.i == 2);  // o2.i is 2, not copied because of custom rule for i
  CHECK(o1.i == 42);

  runtime_value = 99;
  meta::convert_from_other_message(o2, o1, converter);
  CHECK(o1.i == 99);
}

TEST_CASE(
    "experiment_with_meta_info2_function_test_for_helper_file_used_in_assembly_"
    "analysis",
    "[code gen]") {
  example_com_schemas_examples::Simple a;
  a.j = 1;
  a.i = 2;
  a.f = 3.1;
  example_com_schemas_examples::Simple b1;
  simple_copy(a, b1);
  CHECK(b1.j == 1);
  CHECK(b1.i == 2);
  example_com_schemas_examples::Simple b2;
  convert_copy(a, b2);
  CHECK(b2.j == 1);
  CHECK(b2.i == 2);
  example_com_schemas_examples::Simple b3;
  convert_copy_but_set_i_to_0(a, b3);
  CHECK(b3.j == 1);
  CHECK(b3.i == 0);
}

TEST_CASE(
    "experiment_with_meta_info3_alternative_solution_sketch_pointers_instead_"
    "of_names",
    "[code gen]") {
  using T = example_com_schemas_examples::MyObj1;
  T o1, o2;
  o1.i = 9;
  o1.f = 2.0;
  o1.s = "hello";
  o2.i = 1;
  o2.f = 1.0;
  o2.s = "world";

  constexpr auto T::*rel_ptr_i = &T::i;
  constexpr auto T::*rel_ptr_s = &T::s;
  constexpr auto T::*rel_ptr_other_s = &T::s;
  static_assert(rel_ptr_other_s == rel_ptr_s, "expected");
}

TEST_CASE("test_with_arrays", "[code gen]") {
  example_com_schemas_examples::Complex c3;
  c3.a[2].i = 8;
  c3.a[2].j = 9;
  example_com_schemas_examples::Complex4 c4;
  c4.a[2].i = 8;
  c4.a[2].j = 9;

  example_com_schemas_examples::Complex c3_copy;

  meta::convert_from_other_message(c3, c3_copy);
  CHECK(c3_copy.a[2].i == 8);
  CHECK(c3_copy.a[2].j == 9);

  example_com_schemas_examples::ComplexD cD_copy;
  meta::convert_from_other_message(c3, cD_copy);
  CHECK(c3.a.size() == 3);  // fixed
  CHECK(cD_copy.a.size() == 3);
  CHECK(cD_copy.a[2].i == 8);
  CHECK(cD_copy.a[2].j == 9);

  meta::convert_from_other_message(c4, cD_copy);
  CHECK(c4.a.size() == 4);  // fixed
  CHECK(cD_copy.a.size() == 4);
  CHECK(cD_copy.a[2].i == 8);
  CHECK(cD_copy.a[2].j == 9);

  REQUIRE_THROWS_WITH(meta::convert_from_other_message(cD_copy, c3_copy),
                      "array sizes do not match: a");
  REQUIRE_THROWS_WITH(meta::convert_from_other_message(c4, c3_copy),
                      "array sizes do not match: a");

  cD_copy.a.resize(3);  // correct dyn. array
  cD_copy.a[1].i = 42;
  meta::convert_from_other_message(cD_copy, c3_copy);
  CHECK(c3_copy.a[1].i == 42);

  meta::CustomStructConverter converter{meta::createCustomAttributeConverter<
      example_com_schemas_examples::ComplexD, "a">(
      [](const auto& other, auto& dest) {
        dest.resize(9);
        for (auto& d : dest) {
          d = other.s;
        }
      })};
  c3.s.i = 11;
  meta::convert_from_other_message(c3, cD_copy, converter);
  CHECK(cD_copy.a.size() == 9);
  CHECK(cD_copy.s.i == 11);
  for (size_t i = 0; i < 9; i++) {
    CHECK(cD_copy.a[i].i == 11);
  }
}

static_assert(meta::is_raw_scalar_convertible<int, int>(), "unittest");
static_assert(meta::is_raw_scalar_convertible<float, double>(), "unittest");
static_assert(meta::is_raw_scalar_convertible<double, float>(), "unittest");
static_assert(!meta::is_raw_scalar_convertible<double, int>(), "unittest");
static_assert(meta::is_raw_scalar_convertible<int, double>(), "unittest");

static_assert(!meta::is_raw_scalar_convertible<std::uint16_t, std::uint8_t>(),
              "unittest");
static_assert(meta::is_raw_scalar_convertible<std::uint16_t, std::uint16_t>(),
              "unittest");
static_assert(meta::is_raw_scalar_convertible<std::uint16_t, std::uint32_t>(),
              "unittest");

static_assert(!meta::is_raw_scalar_convertible<std::int16_t, std::uint8_t>(),
              "unittest");
static_assert(!meta::is_raw_scalar_convertible<std::int16_t, std::uint16_t>(),
              "unittest");
static_assert(!meta::is_raw_scalar_convertible<std::int16_t, std::uint32_t>(),
              "unittest");

static_assert(!meta::is_raw_scalar_convertible<std::uint16_t, std::int8_t>(),
              "unittest");
static_assert(!meta::is_raw_scalar_convertible<std::uint16_t, std::int16_t>(),
              "unittest");
static_assert(meta::is_raw_scalar_convertible<std::uint16_t, std::int32_t>(),
              "unittest");
