/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include <catch2/catch.hpp>
#include <concepts>
#include <iostream>
#include <sstream>
#include <type_traits>

#include "dds_examples/HelloWorld.h"
#include "dds_examples/HelloWorld.meta.h"
#include "dds_examples/Simple.h"
#include "dds_examples/Simple.meta.h"
#include "example_com_schemas_examples/Color.h"
#include "example_com_schemas_examples/Complex.h"
#include "example_com_schemas_examples/HelloWorld.h"
#include "example_com_schemas_examples/Payload.h"
#include "example_com_schemas_examples/Simple.h"
#include "meta/convert.h"
#include "meta/json.h"
#include "meta/meta.h"
#include "meta/print.h"
#include "meta/program_options.h"
#include "meta/type_traits.h"

// regression

static_assert(
    !meta::enum_attribute_meta<dds_examples::meta::HelloWorldT::attributes::id>,
    "id is not an enum");

static_assert(!meta::dynamic_array_attribute_meta<
                  dds_examples::meta::ComplexT::attributes::a>,
              "a is not dynamic");
static_assert(meta::fixed_sized_array_attribute_meta<
                  dds_examples::meta::ComplexT::attributes::a>,
              "a is not dynamic");

static_assert(!meta::struct_attribute_meta<
                  dds_examples::meta::HelloWorldT::attributes::color>,
              "ColorT is an enum, not a struct");

// normal tests

void init(dds_examples::HelloWorldT& data) {
  data.index(10);
  data.header().id(123);
  data.header().color(dds_examples::ColorT::RED);
  data.message("Hello ESE");
  data.color(dds_examples::ColorT::BLUE);
  data.id(99);
  data.values()[0] = 11;
  data.values()[1] = 22;
  data.values()[2] = 33;
  data.colors().resize(4);
  data.colors().at(0) = dds_examples::ColorT::RED;
  data.colors().at(1) = dds_examples::ColorT::GREEN;
  data.colors().at(2) = dds_examples::ColorT::BLUE;
  data.colors().at(3) = dds_examples::ColorT::RED;
  data.arrayOfStructs().resize(2);
  data.arrayOfStructs()[0].id(1);
  data.arrayOfStructs()[0].color(dds_examples::ColorT::RED);
  data.arrayOfStructs()[1].id(2);
  data.arrayOfStructs()[1].color(dds_examples::ColorT::GREEN);
}

void init2(dds_examples::HelloWorldT& data) {
  data.index(11);
  data.header().id(124);
  data.header().color(dds_examples::ColorT::GREEN);
  data.message("Hello ESE2");
  data.color(dds_examples::ColorT::RED);
  data.id(90);
  data.values()[0] = 1;
  data.values()[1] = 2;
  data.values()[2] = 3;
  data.colors().resize(3);
  data.colors().at(0) = dds_examples::ColorT::GREEN;
  data.colors().at(1) = dds_examples::ColorT::RED;
  data.colors().at(2) = dds_examples::ColorT::BLUE;
  data.arrayOfStructs().resize(2);
  data.arrayOfStructs()[0].id(3);
  data.arrayOfStructs()[0].color(dds_examples::ColorT::BLUE);
  data.arrayOfStructs()[1].id(4);
  data.arrayOfStructs()[1].color(dds_examples::ColorT::RED);
}

TEST_CASE("fastdds print", "[fastdds::meta::print]") {
  dds_examples::HelloWorldT data;
  init(data);

  // show how a common lib function (print) can be applied to protobuf elements
  // compare `cpp-library/examples/grpc/test/grpc.cpp`,
  // `grpc convert enums and print`

  std::ostringstream output;
  meta::print(data, output);

  CHECK(output.str() == R"(HelloWorldT:
  id=99
  header=
    PayloadT:
      id=123
      color=RED
  index=10
  message="Hello ESE"
  color=BLUE
  colors=[ RED GREEN BLUE RED ]
  values=[ 11 22 33 ]
  arrayOfStructs=[
    PayloadT:
      id=1
      color=RED
    PayloadT:
      id=2
      color=GREEN
  ]
)");
}

TEST_CASE("fastdds convert dds to dds", "[fastdds::meta::convert]") {
  dds_examples::HelloWorldT data1;
  dds_examples::HelloWorldT data2;
  init(data1);
  init2(data2);

  {
    std::ostringstream output1;
    std::ostringstream output2;
    meta::print(data1, output1);
    meta::print(data2, output2);
    CHECK(output2.str() != output1.str());
    CHECK(data2.colors().size() == 3);
  }

  meta::convert_from_other_message(data1, data2);

  {
    std::ostringstream output1;
    std::ostringstream output2;
    meta::print(data1, output1);
    meta::print(data2, output2);
    CHECK(output2.str() == output1.str());
    CHECK(data2.colors().size() == 4);
  }
}

TEST_CASE("fastdds convert internal struct to dds",
          "[fastdds::meta::convert2]") {
  dds_examples::SimpleT simple_dds;
  example_com_schemas_examples::Simple simple_internal;

  simple_internal.i = 10;
  simple_internal.j = 11;
  simple_internal.f = 1.23;

  meta::convert_from_other_message(simple_internal, simple_dds);

  CHECK(simple_dds.i() == 10);
  CHECK(simple_dds.j() == 11);
  CHECK(simple_dds.f() == 1.23);
}

TEST_CASE("fastdds convert dds to internal struct and back",
          "[fastdds::meta::convert2b]") {
  dds_examples::ComplexT complex_dds;
  example_com_schemas_examples::Complex complex_internal;

  complex_internal.s.i = 10;
  complex_internal.s.j = 11;
  complex_internal.s.f = 1.23;
  for (size_t i = 0; i < 3; i++) {
    complex_internal.a[i].i = 10 + 1000 * i;
    complex_internal.a[i].j = 11 + 1000 * i;
    complex_internal.a[i].f = 1.23 + 1000 * i;
  }

  meta::convert_from_other_message(complex_internal, complex_dds);

  CHECK(complex_dds.s().i() == 10);
  CHECK(complex_dds.s().j() == 11);
  CHECK(complex_dds.s().f() == 1.23);
  for (size_t i = 0; i < 3; i++) {
    CHECK(complex_dds.a()[i].i() == 10 + i * 1000);
    CHECK(complex_dds.a()[i].j() == 11 + i * 1000);
    CHECK(complex_dds.a()[i].f() == 1.23 + i * 1000);
  }

  complex_dds.s().i() = 20;
  complex_dds.s().j() = 21;
  complex_dds.s().f() = 2.23;
  for (size_t i = 0; i < 3; i++) {
    complex_dds.a()[i].i() = 20 + 1000 * i;
    complex_dds.a()[i].j() = 21 + 1000 * i;
    complex_dds.a()[i].f() = 2.23 + 1000 * i;
  }

  meta::convert_from_other_message(complex_dds, complex_internal);

  CHECK(complex_internal.s.i == 20);
  CHECK(complex_internal.s.j == 21);
  CHECK(complex_internal.s.f == 2.23);
  for (size_t i = 0; i < 3; i++) {
    CHECK(complex_internal.a[i].i == 20 + i * 1000);
    CHECK(complex_internal.a[i].j == 21 + i * 1000);
    CHECK(complex_internal.a[i].f == 2.23 + i * 1000);
  }

  // fails compiling, as expected (test with concept + static_assert?)
  // example_com_schemas_examples::Simple other1;
  // meta::convert_from_other_message(other1, complex_internal);
  // dds_examples::SimpleT other2;
  // meta::convert_from_other_message(other2, complex_internal);
}

TEST_CASE(
    "fastdds json serialization using the meta-api directly (w/o internal json "
    "struct)",
    "[fastdds::meta::json]") {
  dds_examples::HelloWorldT data;
  init(data);

  std::string jsonText = meta::to_json(data);

  CHECK(
      jsonText ==
      R"({"arrayOfStructs":[{"color":"RED","id":1},{"color":"GREEN","id":2}],"color":"BLUE","colors":["RED","GREEN","BLUE","RED"],"header":{"color":"RED","id":123},"id":99,"index":10,"message":"Hello ESE","values":[11,22,33]})");
}

TEST_CASE("fastdds convert enums", "[convert enums]") {
  static_assert(meta::is_enum_v<example_com_schemas_examples::Color>,
                "unittest");
  static_assert(meta::is_enum_v<dds_examples::ColorT>, "unittest");

  dds_examples::HelloWorldT data1;
  example_com_schemas_examples::HelloWorld data2;

  init(data1);
  meta::convert_from_other_message(data1, data2);

  CHECK(meta::to_json(data1) == meta::to_json(data2));

  data2.index++;
  CHECK(meta::to_json(data1) != meta::to_json(data2));

  meta::convert_from_other_message(data2, data1);
  CHECK(meta::to_json(data1) == meta::to_json(data2));
}

TEST_CASE("fast_dds_po1", "[meta::program_options]") {
  {
    int argc = 7;
    const char* argv[] = {"app.exe", "--i", "1", "--j", "2", "--f", "3.3"};
    auto data = meta::from_program_options<dds_examples::SimpleT>(argc, argv);
    CHECK(data.i() == 1);
    CHECK(data.j() == 2);
    CHECK(data.f() == 3.3);
  }
}
