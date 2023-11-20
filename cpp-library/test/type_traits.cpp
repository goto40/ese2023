/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include "meta/type_traits.h"

#include <optional>

#include "meta/meta.h"

// compile time tests only
namespace {

static_assert(meta::std_array_of_type<std::array<char, 6>, char>, "pos test");
static_assert(meta::std_array_of_type<std::array<char, 6>&, char>, "pos test");
static_assert(meta::std_array_of_type<const std::array<char, 6>&, char>,
              "pos test");
static_assert(!meta::std_array_of_type<std::array<int, 6>, char>, "neg test");
static_assert(!meta::std_array_of_type<int, char>, "neg test");

namespace scalar_test {
struct DemoData {
  int x;
};
struct DemoMetaForX {
  using STRUCT = DemoData;
  using ATTR_TYPE = int;
  static constexpr const char* name() { return "x"; }
  static constexpr std::array<const char, 6> ct_name() { return {"x"}; }
  static constexpr auto& get(STRUCT& s) { return s.x; }
  static constexpr const auto& get(const STRUCT& s) { return s.x; }
};

static_assert(meta::attribute_meta<DemoMetaForX>, "test");
static_assert(meta::scalar_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::optional_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::dynamic_array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::fixed_sized_array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::enum_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::string_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::struct_attribute_meta<DemoMetaForX>, "test");
}  // namespace scalar_test
namespace optional_test {
struct DemoData {
  std::optional<int> x;
};
struct DemoMetaForX {
  using STRUCT = DemoData;
  using ATTR_TYPE = std::optional<int>;
  static constexpr const char* name() { return "x"; }
  static constexpr std::array<const char, 6> ct_name() { return {"x"}; }
  static constexpr auto& get(STRUCT& s) { return s.x; }
  static constexpr const auto& get(const STRUCT& s) { return s.x; }
};

static_assert(meta::attribute_meta<DemoMetaForX>, "test");
static_assert(meta::scalar_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::optional_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::dynamic_array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::fixed_sized_array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::enum_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::string_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::struct_attribute_meta<DemoMetaForX>, "test");
}  // namespace optional_test
namespace array_test {
struct DemoData {
  std::vector<int> x;
};
struct DemoMetaForX {
  using STRUCT = DemoData;
  using ATTR_TYPE = std::vector<int>;
  static constexpr const char* name() { return "x"; }
  static constexpr std::array<const char, 6> ct_name() { return {"x"}; }
  static constexpr auto& get(STRUCT& s) { return s.x; }
  static constexpr const auto& get(const STRUCT& s) { return s.x; }
};

static_assert(meta::attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::scalar_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::optional_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::array_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::dynamic_array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::fixed_sized_array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::enum_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::string_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::struct_attribute_meta<DemoMetaForX>, "test");
}  // namespace array_test
namespace enum_test {
enum class E {};
struct DemoData {
  E x;
};
struct DemoMetaForX {
  using STRUCT = DemoData;
  using ATTR_TYPE = E;
  static constexpr const char* name() { return "x"; }
  static constexpr std::array<const char, 6> ct_name() { return {"x"}; }
  static constexpr auto& get(STRUCT& s) { return s.x; }
  static constexpr const auto& get(const STRUCT& s) { return s.x; }
};

static_assert(meta::attribute_meta<DemoMetaForX>, "test");
static_assert(meta::scalar_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::optional_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::dynamic_array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::fixed_sized_array_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::enum_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::string_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::struct_attribute_meta<DemoMetaForX>, "test");
}  // namespace enum_test
namespace enum_test_in_array {
enum class E {};
struct DemoData {
  std::array<E, 3> x;
};
struct DemoMetaForX {
  using STRUCT = DemoData;
  using ATTR_TYPE = std::array<E, 3>;
  static constexpr const char* name() { return "x"; }
  static constexpr std::array<const char, 6> ct_name() { return {"x"}; }
  static constexpr auto& get(STRUCT& s) { return s.x; }
  static constexpr const auto& get(const STRUCT& s) { return s.x; }
};

static_assert(meta::attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::scalar_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::optional_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::dynamic_array_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::fixed_sized_array_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::enum_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::string_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::struct_attribute_meta<DemoMetaForX>, "test");
}  // namespace enum_test_in_array
namespace string_test {
struct DemoData {
  std::string x;
};
struct DemoMetaForX {
  using STRUCT = DemoData;
  using ATTR_TYPE = std::string;
  static constexpr const char* name() { return "x"; }
  static constexpr std::array<const char, 6> ct_name() { return {"x"}; }
  static constexpr auto& get(STRUCT& s) { return s.x; }
  static constexpr const auto& get(const STRUCT& s) { return s.x; }
};

static_assert(meta::attribute_meta<DemoMetaForX>, "test");
static_assert(meta::scalar_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::optional_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::dynamic_array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::fixed_sized_array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::enum_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::string_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::struct_attribute_meta<DemoMetaForX>, "test");
}  // namespace string_test
namespace string_test_in_array {
struct DemoData {
  std::array<std::string, 3> x;
};
struct DemoMetaForX {
  using STRUCT = DemoData;
  using ATTR_TYPE = std::array<std::string, 3>;
  static constexpr const char* name() { return "x"; }
  static constexpr std::array<const char, 6> ct_name() { return {"x"}; }
  static constexpr auto& get(STRUCT& s) { return s.x; }
  static constexpr const auto& get(const STRUCT& s) { return s.x; }
};

static_assert(meta::attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::scalar_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::optional_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::dynamic_array_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::fixed_sized_array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::enum_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::string_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::struct_attribute_meta<DemoMetaForX>, "test");
}  // namespace string_test_in_array

namespace test_array_of_structs {

// definition of the "inner struct" + meta info
// (the meta info is required to identify the inner struct as struct)
struct OtherData {
  std::string x;
};
struct OtherMetaForX {
  using STRUCT = OtherData;
  using ATTR_TYPE = std::string;
  static constexpr const char* name() { return "x"; }
  static constexpr std::array<const char, 6> ct_name() { return {"x"}; }
  static constexpr auto& get(STRUCT& s) { return s.x; }
  static constexpr const auto& get(const STRUCT& s) { return s.x; }
};
struct OtherMeta {
  static constexpr const char* name() { return "OtherData"; }

  template <class... T>
  struct List;
  using ListOfAttributeMetaClasses = List<OtherMetaForX>;
};

}  // namespace test_array_of_structs
}  // namespace

template <>
struct meta::meta_of<test_array_of_structs::OtherData>
    : std::type_identity<test_array_of_structs::OtherMeta> {};

namespace {
namespace test_array_of_structs {
struct DemoData {
  std::vector<OtherData> x;
};
struct DemoMetaForX {
  using STRUCT = DemoData;
  using ATTR_TYPE = std::vector<OtherData>;
  static constexpr const char* name() { return "x"; }
  static constexpr std::array<const char, 6> ct_name() { return {"x"}; }
  static constexpr auto& get(STRUCT& s) { return s.x; }
  static constexpr const auto& get(const STRUCT& s) { return s.x; }
};

static_assert(meta::attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::scalar_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::optional_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::array_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::dynamic_array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::fixed_sized_array_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::enum_attribute_meta<DemoMetaForX>, "test");
static_assert(!meta::string_attribute_meta<DemoMetaForX>, "test");
static_assert(meta::struct_attribute_meta<DemoMetaForX>, "test");  // (!)
}  // namespace test_array_of_structs
}  // namespace
