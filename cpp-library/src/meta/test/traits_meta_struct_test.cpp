/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include "meta/type_traits.h"

static_assert(!meta::struct_meta<int>, "int is not a meta Struct");

struct A {};

static_assert(!meta::struct_meta<A>, "A is not a meta Struct");

struct A2 {
  static constexpr const char* name();
};

static_assert(!meta::struct_meta<A2>, "A2 is not a meta Struct");

struct A3 : A2 {
  template <class... T>
  struct List;
  using ListOfAttributeMetaClasses = List<>;
};

static_assert(meta::struct_meta<A3>, "A3 is a meta Struct");

struct A4 : A2 {
  template <class... T>
  struct List;

  struct F {};

  using ListOfAttributeMetaClasses = List<F>;
};

static_assert(!meta::attribute_meta<A4::F>, "A4:F is not a meta attribute");
static_assert(!meta::struct_meta<A4>, "A4 is not a meta Struct");

struct A5 : A2 {
  template <class... T>
  struct List;

  struct F {
    using STRUCT = A5;
    using ATTR_TYPE = int32_t;
    using ATTR_TYPE_WO_OPTIONAL = int32_t;

    static ATTR_TYPE& get(STRUCT&);
    static ATTR_TYPE get(const STRUCT&);

    static const char* name();
    static std::array<const char, 6> ct_name();
    static bool is_required();
    static bool is_scalar();
    static bool is_array();
    static bool is_fixed_sized_array();
    static bool is_enum();
    static bool is_string();
    static bool is_struct();
  };

  using ListOfAttributeMetaClasses = List<F>;
};

static_assert(meta::attribute_meta<A5::F>, "A5:F is a meta attribute");
static_assert(meta::struct_meta<A5>, "A5 is a meta class");

struct A6 : A2 {
  template <class... T>
  struct List;
  using ListOfAttributeMetaClasses = List<A5::F, A5::F>;
};

static_assert(meta::struct_meta<A5>, "A6 is a meta class");

struct A7 : A2 {
  template <class... T>
  struct List;
  using ListOfAttributeMetaClasses = List<A5::F, int32_t>;
};

static_assert(!meta::struct_meta<A7>, "A6 is not a meta class");
