/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

#include "meta/virtual_meta.h"

#include <catch2/catch.hpp>
#include <sstream>

#include "example_com_schemas_examples/Color.h"
#include "example_com_schemas_examples/HelloWorld.h"
#include "example_com_schemas_examples/MyObj1.h"
#include "example_com_schemas_examples/MyObj1Complex.h"
#include "meta/meta.h"
#include "meta/type_traits.h"

TEST_CASE("virtual1 with own data", "[meta::virtual]") {
  // str, int double
  auto obj = meta::Struct<example_com_schemas_examples::MyObj1>::create();

  obj->getStructRef().s = "Hello World";
  obj->getStructRef().i = 99;
  obj->getStructRef().f = 1.2;

  CHECK(obj->size() == 3);
  CHECK(obj->at(0)->name() == "s");
  CHECK(obj->at(1)->name() == "i");
  CHECK(obj->at(2)->name() == "f");
  CHECK(obj->at(0)->to_string() == "Hello World");
  CHECK(obj->at(1)->to_string() == "99");
  CHECK(obj->at(2)->to_string() == "1.2");
}

TEST_CASE("virtual1 with a ptr", "[meta::virtual]") {
  // str, int double
  auto raw = std::make_shared<example_com_schemas_examples::MyObj1>();
  auto obj = meta::StructPtr<example_com_schemas_examples::MyObj1>::create(raw);

  obj->getStructRef().s = "Hello World";
  obj->getStructRef().i = 99;
  obj->getStructRef().f = 1.2;

  CHECK(obj->size() == 3);
  CHECK(obj->at(0)->name() == "s");
  CHECK(obj->at(1)->name() == "i");
  CHECK(obj->at(2)->name() == "f");
  CHECK(obj->at(0)->to_string() == "Hello World");
  CHECK(obj->at(1)->to_string() == "99");
  CHECK(obj->at(2)->to_string() == "1.2");
}

TEST_CASE("virtual1 with a ref", "[meta::virtual]") {
  // str, int double
  example_com_schemas_examples::MyObj1 raw;
  auto obj =
      meta::StructReference<example_com_schemas_examples::MyObj1, void>::create(
          raw, nullptr);  // passing a nullptr --> you are responsible to handle
                          // lifetime correctly!

  raw.s = "Hello World";
  raw.i = 99;
  raw.f = 1.2;

  CHECK(obj->size() == 3);
  CHECK(obj->at(0)->name() == "s");
  CHECK(obj->at(1)->name() == "i");
  CHECK(obj->at(2)->name() == "f");
  CHECK(obj->at(0)->to_string() == "Hello World");
  CHECK(obj->at(1)->to_string() == "99");
  CHECK(obj->at(2)->to_string() == "1.2");
}

TEST_CASE("virtual1 with a ref and a real owner", "[meta::virtual]") {
  auto raw = std::make_shared<example_com_schemas_examples::MyObj1>();
  auto obj =
      meta::StructReference<example_com_schemas_examples::MyObj1,
                            example_com_schemas_examples::MyObj1>::create(*raw,
                                                                          raw);

  obj->getStructRef().s = "Hello World";
  obj->getStructRef().i = 99;
  obj->getStructRef().f = 1.2;

  CHECK(obj->size() == 3);
  CHECK(obj->at(0)->name() == "s");
  CHECK(obj->at(1)->name() == "i");
  CHECK(obj->at(2)->name() == "f");
  CHECK(obj->at(0)->to_string() == "Hello World");
  CHECK(obj->at(1)->to_string() == "99");
  CHECK(obj->at(2)->to_string() == "1.2");
}

TEST_CASE("virtual2 with own data", "[meta::virtual]") {
  // str, int double
  auto obj =
      meta::Struct<example_com_schemas_examples::MyObj1Complex>::create();

  obj->getStructRef().s = "Hello Complex";
  obj->getStructRef().o.s = "Hello World";
  obj->getStructRef().o.i = 99;
  obj->getStructRef().o.f = 1.2;

  CHECK(obj->size() == 2);
  CHECK(obj->at(0)->name() == "s");
  auto inner_struct = obj->at(1)->get_struct();
  CHECK(inner_struct->size() == 3);
  CHECK(inner_struct->at(0)->name() == "s");
  CHECK(inner_struct->at(1)->name() == "i");
  CHECK(inner_struct->at(2)->name() == "f");

  CHECK(obj->at(0)->to_string() == "Hello Complex");
  CHECK(inner_struct->at(0)->to_string() == "Hello World");
  CHECK(inner_struct->at(1)->to_string() == "99");
  CHECK(inner_struct->at(2)->to_string() == "1.2");
}

TEST_CASE("virtual3  with own data", "[meta::virtual]") {
  auto obj = meta::Struct<example_com_schemas_examples::HelloWorld>::create();
  obj->getStructRef().values.resize(3);
  obj->getStructRef().values[0] = 11;
  obj->getStructRef().values[1] = 12;
  obj->getStructRef().values[2] = 13;
  obj->getStructRef().arrayOfStructs.resize(2);
  obj->getStructRef().arrayOfStructs[0].color =
      example_com_schemas_examples::Color::GREEN;
  obj->getStructRef().arrayOfStructs[1].color =
      example_com_schemas_examples::Color::BLUE;
  obj->getStructRef().colors.resize(4);
  obj->getStructRef().colors[3] = example_com_schemas_examples::Color::BLUE;

  CHECK(obj->at("values")->to_string(0) == "11");
  CHECK(obj->at("values")->to_string(1) == "12");
  CHECK(obj->at("values")->to_string(2) == "13");
  CHECK_THROWS(obj->at("values")->to_string(3));
  CHECK_THROWS(obj->at("badname")->to_string());
  CHECK(obj->at("arrayOfStructs")->is_array());
  CHECK(obj->at("arrayOfStructs")->is_struct());
  CHECK(obj->at("arrayOfStructs")->get_struct(0)->at("color")->to_string() ==
        "GREEN");
  CHECK(obj->at("arrayOfStructs")->get_struct(1)->at("color")->to_string() ==
        "BLUE");
  CHECK(obj->at("colors")->is_array());
  CHECK(not obj->at("colors")->is_struct());
  CHECK_THROWS(obj->at("colors")->get_struct());
  CHECK(obj->at("colors")->to_string(3) == "BLUE");
  CHECK_NOTHROW(obj->at("colors")->from_string("RED", 3));
  CHECK(obj->at("colors")->to_string(3) == "RED");
  CHECK(obj->getStructRef().colors[3] ==
        example_com_schemas_examples::Color::RED);
  CHECK_THROWS(obj->at("colors")->from_string("NOCOLOR", 3));
  CHECK(obj->getStructRef().colors[3] ==
        example_com_schemas_examples::Color::RED);  // unchanged

  std::ostringstream text;
  for (auto x : *obj) {
    text << x->name() << "(" << x->is_array() << "," << x->is_struct() << ")"
         << "\n";
  }
  CHECK(text.str() == R"(header(0,1)
id(0,0)
index(0,0)
message(0,0)
color(0,0)
colors(1,0)
values(1,0)
arrayOfStructs(1,1)
)");
}
