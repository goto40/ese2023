/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include <functional>
#include <iostream>

#include "Simple.pb.h"
#include "meta/grpc.h"
#include "meta/meta.h"
#include "meta/print.h"

namespace example_grpc {
namespace meta {

struct Simple {
  static constexpr const char* name() { return "Simple"; }
  struct attributes {
    struct s {
      using STRUCT = example_grpc::Simple;
      using ATTR_TYPE = std::string;
      static constexpr const char* name() { return "s"; }
      static constexpr std::array<const char, 2> ct_name() { return {"s"}; }
      static auto& get(STRUCT& s) { return *s.mutable_s(); }
      static const auto& get(const STRUCT& s) { return s.s(); }
    };
    struct i {
      using STRUCT = example_grpc::Simple;
      using ATTR_TYPE = int64_t;
      static constexpr const char* name() { return "i"; }
      static constexpr std::array<const char, 2> ct_name() { return {"i"}; }
      static auto get(STRUCT& s) {
        return ::meta::grpc::FieldWrapper(s, &STRUCT::i, &STRUCT::set_i);
      }
      static const auto get(const STRUCT& s) {
        return ::meta::grpc::FieldWrapper(s, &STRUCT::i);
      }
    };
    struct f {
      using STRUCT = example_grpc::Simple;
      using ATTR_TYPE = ::meta::grpc::FieldWrapper<double, STRUCT>;

      static constexpr const char* name() { return "f"; }
      static constexpr std::array<const char, 2> ct_name() { return {"f"}; }
      static auto get(STRUCT& s) {
        return ::meta::grpc::FieldWrapper(s, &STRUCT::f, &STRUCT::set_f);
      }
      static const auto get(const STRUCT& s) {
        return ::meta::grpc::FieldWrapper(s, &STRUCT::f);
      }
    };
  };
  template <class... T>
  struct List;
  using ListOfAttributeMetaClasses =
      List<attributes::s, attributes::i, attributes::f>;
};

}  // namespace meta
}  // namespace example_grpc

template <>
struct meta::meta_of<example_grpc::Simple>
    : std::type_identity<example_grpc::meta::Simple> {};

template <meta::attribute_meta Attribute>
struct Foo {};

int main() {
  std::cout << "Hello, world\n";

  example_grpc::Simple simple_grpc;
  simple_grpc.set_s("foo bar baz");

  meta::print(simple_grpc);
}
