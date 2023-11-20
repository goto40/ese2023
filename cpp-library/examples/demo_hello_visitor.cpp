/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include <iostream>

#include "example_com_schemas_defs/MyObj2.h"
#include "meta/meta.h"

struct Visitor {
  template <class META>
  void visit() {
    std::cout << META::name() << "\n";
  }
};

template <class T>
using MetaOf = std::remove_pointer_t<decltype(get_meta_class_adl_helper(
    static_cast<T*>(nullptr)))>;

int main() {
  std::cout << "demo\n";
  example_com_schemas_defs::MyObj2 obj;
  Visitor v{};
  meta::accept<example_com_schemas_defs::MyObj2>(v);
}
