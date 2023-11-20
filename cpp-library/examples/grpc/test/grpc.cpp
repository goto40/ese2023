/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include <catch2/catch.hpp>
#include <iostream>
#include <sstream>

#include "HelloWorld.meta.h"
#include "Simple.meta.h"
#include "Simple.pb.h"
#include "example_com_schemas_examples/Complex.h"
#include "example_com_schemas_examples/HelloWorld.h"
#include "example_com_schemas_examples/Simple.h"
#include "meta/convert.h"
#include "meta/json.h"
#include "meta/print.h"
#include "meta/program_options.h"

TEST_CASE("grpc print") {
  grpc_examples::Hello grpc_simple;
  grpc_simple.set_f(3.14);
  grpc_simple.set_s("Hello ESE");

  std::ostringstream output;
  meta::print(grpc_simple, output);

  CHECK(output.str() == R"(Hello:
  f=3.14
  s="Hello ESE"
)");
}

TEST_CASE("grpc convert proto to proto") {
  grpc_examples::Hello grpc_simple[2];
  grpc_simple[0].set_f(3.14);
  grpc_simple[0].set_s("Hello ESE");
  grpc_simple[1].set_f(-100);
  grpc_simple[1].set_s("Hello Freunde");

  {
    std::ostringstream output[2];
    meta::print(grpc_simple[0], output[0]);
    meta::print(grpc_simple[1], output[1]);
    CHECK(output[0].str() != output[1].str());
  }

  meta::convert_from_other_message(grpc_simple[0], grpc_simple[1]);

  {
    std::ostringstream output[2];
    meta::print(grpc_simple[0], output[0]);
    meta::print(grpc_simple[1], output[1]);
    CHECK(output[0].str() == output[1].str());
  }
}

TEST_CASE("grpc convert internal to proto") {
  grpc_examples::Complex complex_grpc;
  example_com_schemas_examples::Complex complex_internal;

  complex_internal.s.i = 10;
  complex_internal.s.j = 11;
  complex_internal.s.f = 1.23;
  for (size_t i = 0; i < 3; i++) {
    complex_internal.a[i].i = 10 + 1000 * i;
    complex_internal.a[i].j = 11 + 1000 * i;
    complex_internal.a[i].f = 1.23 + 1000 * i;
  }

  meta::convert_from_other_message(complex_internal, complex_grpc);

  CHECK(complex_grpc.s().i() == 10);
  CHECK(complex_grpc.s().j() == 11);
  CHECK(complex_grpc.s().f() == 1.23);
  for (size_t i = 0; i < 3; i++) {
    CHECK(complex_grpc.a()[i].i() == 10 + i * 1000);
    CHECK(complex_grpc.a()[i].j() == 11 + i * 1000);
    CHECK(complex_grpc.a()[i].f() == 1.23 + i * 1000);
  }
}

TEST_CASE("grpc convert proto to internal") {
  grpc_examples::Complex complex_grpc;
  example_com_schemas_examples::Complex complex_internal;

  complex_grpc.mutable_s()->set_i(10);
  complex_grpc.mutable_s()->set_j(11);
  complex_grpc.mutable_s()->set_f(1.23);
  for (size_t i = 0; i < 3; i++) {
    auto* new_entry = complex_grpc.mutable_a()->Add();
    new_entry->set_i(10 + 1000 * i);
    new_entry->set_j(11 + 1000 * i);
    new_entry->set_f(1.23 + 1000 * i);
  }

  meta::convert_from_other_message(complex_grpc, complex_internal);

  CHECK(complex_internal.s.i == 10);
  CHECK(complex_internal.s.j == 11);
  CHECK(complex_internal.s.f == 1.23);
  for (size_t i = 0; i < 3; i++) {
    CHECK(complex_internal.a[i].i == 10 + 1000 * i);
    CHECK(complex_internal.a[i].j == 11 + 1000 * i);
    CHECK(complex_internal.a[i].f == 1.23 + 1000 * i);
  }
}

TEST_CASE("grpc json serialization using the meta-api") {
  grpc_examples::Complex complex_grpc;

  complex_grpc.mutable_s()->set_i(10);
  complex_grpc.mutable_s()->set_j(11);
  complex_grpc.mutable_s()->set_f(1.23);
  for (size_t i = 0; i < 3; i++) {
    auto* new_entry = complex_grpc.mutable_a()->Add();
    new_entry->set_i(10 + 1000 * i);
    new_entry->set_j(11 + 1000 * i);
    new_entry->set_f(1.23 + 1000 * i);
  }

  std::string jsonText = meta::to_json(complex_grpc);

  CHECK(
      jsonText ==
      R"({"a":[{"f":1.23,"i":10,"j":11},{"f":1001.23,"i":1010,"j":1011},{"f":2001.23,"i":2010,"j":2011}],"s":{"f":1.23,"i":10,"j":11}})");
}

TEST_CASE("grpc json deserialization using the meta-api") {
  grpc_examples::Complex complex_grpc;

  CHECK(complex_grpc.s().f() != 1.23);
  CHECK(complex_grpc.s().i() != 10);
  CHECK(complex_grpc.s().j() != 11);

  meta::from_json(
      std::string(
          R"({"a":[{"f":1.23,"i":10,"j":11},{"f":1001.23,"i":1010,"j":1011},{"f":2001.23,"i":2010,"j":2011}],"s":{"f":1.23,"i":10,"j":11}})"),
      complex_grpc);

  CHECK(complex_grpc.s().i() == 10);
  CHECK(complex_grpc.s().j() == 11);
  CHECK(complex_grpc.s().f() == 1.23);
  for (size_t i = 0; i < 3; i++) {
    CHECK(complex_grpc.a()[i].i() == 10 + 1000 * i);
    CHECK(complex_grpc.a()[i].j() == 11 + 1000 * i);
    CHECK(complex_grpc.a()[i].f() == 1.23 + 1000 * i);
  }
}

TEST_CASE("grpc_po1", "[meta::program_options]") {
  {
    int argc = 7;
    const char* argv[] = {"app.exe", "--i", "1", "--j", "2", "--f", "3.3"};
    auto data = meta::from_program_options<grpc_examples::Simple>(argc, argv);
    CHECK(data.i() == 1);
    CHECK(data.j() == 2);
    CHECK(data.f() == 3.3);
  }
  {
    int argc = 7;
    const char* argv[] = {"app.exe", "--i", "11", "--j", "22", "--f", "33.3"};
    auto data = meta::from_program_options<grpc_examples::Simple>(argc, argv);
    CHECK(data.i() == 11);
    CHECK(data.j() == 22);
    CHECK(data.f() == 33.3);
  }
}

TEST_CASE("grpc convert enums and print", "[convert enums]") {
  static_assert(meta::is_enum_v<example_com_schemas_examples::Color>,
                "unittest");
  static_assert(meta::is_enum_v<meta::value_type_t<
                    grpc_examples::meta::HelloWorld::attributes::color>>,
                "unittest");

  grpc_examples::HelloWorld data1;
  data1.set_id(42);
  data1.mutable_header()->set_id(3);
  data1.mutable_header()->set_color(grpc_examples::BLUE);
  data1.set_index(5);
  data1.set_message("HelloWorld");
  data1.set_color(grpc_examples::RED);
  data1.mutable_colors()->Add(grpc_examples::RED);
  data1.mutable_colors()->Add(grpc_examples::GREEN);
  data1.mutable_colors()->Add(grpc_examples::BLUE);
  data1.mutable_values()->Add(100);
  data1.mutable_values()->Add(200);
  data1.mutable_values()->Add(300);
  auto* new_struct = data1.mutable_arrayofstructs()->Add();
  new_struct->set_id(1);
  new_struct = data1.mutable_arrayofstructs()->Add();
  new_struct->set_id(2);

  example_com_schemas_examples::HelloWorld data2;
  meta::convert_from_other_message(data1, data2);
  CHECK(meta::to_json(data1) == meta::to_json(data2));

  data2.index++;
  CHECK(meta::to_json(data1) != meta::to_json(data2));

  meta::convert_from_other_message(data2, data1);
  CHECK(meta::to_json(data1) == meta::to_json(data2));

  // show how a common lib function (print) can be applied to protobuf elements
  // compare `cpp-library/examples/fastdds/test/fastdds.cpp`, `fastdds print`

  std::ostringstream stream;
  meta::print(data1, stream);

  CHECK(stream.str() == R"(HelloWorld:
  id=42
  header=
    Payload:
      id=3
      color=BLUE
  index=6
  message="HelloWorld"
  color=RED
  colors=[ RED GREEN BLUE ]
  values=[ 100 200 300 ]
  arrayOfStructs=[
    Payload:
      id=1
      color=RED
    Payload:
      id=2
      color=RED
  ]
)");
}
