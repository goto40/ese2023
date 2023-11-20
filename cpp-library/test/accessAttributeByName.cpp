/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include <catch2/catch.hpp>

#include "example_com_schemas_examples/MyObj1.h"
#include "example_com_schemas_examples/MyObj2.h"
#include "meta/convert.h"
#include "meta/meta.h"

TEST_CASE("access_by_name", "experiment_with_access_by_name") {
  example_com_schemas_examples::MyObj1 o1;
  example_com_schemas_examples::MyObj2 o2;
  o1.i = 1;
  o2.i = 2;

  CHECK(meta::access_by_name<"i">(o1) == 1);
  CHECK(meta::access_by_name<"i">(o2) == 2);

  // also works for given meta (e.g., in a visitor)
  using META_i = example_com_schemas_examples::meta::MyObj1::attributes::i;
  CHECK(meta::access_by_name<META_i::ct_name()>(o1) == 1);
}
