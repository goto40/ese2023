// clang-format off
#pragma once

#include "meta/grpc.h"
#include "meta/meta.h"
#include "Simple.pb.h"


namespace grpc_examples {
namespace meta {
struct Hello {
static constexpr const char* name() { return "Hello"; }
static constexpr const char* description() { return "Hello"; }
struct attributes {
struct f {
static constexpr std::array<const char, 2> ct_name() { return {"f"}; }
static constexpr const char* name() { return "f"; }
static constexpr const char* description() { return "f"; }
using STRUCT = grpc_examples::Hello;
using ATTR_TYPE = double;
static decltype(auto) get(STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::f, &STRUCT::set_f);}
static decltype(auto) get(const STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::f);}
};
struct s {
static constexpr std::array<const char, 2> ct_name() { return {"s"}; }
static constexpr const char* name() { return "s"; }
static constexpr const char* description() { return "s"; }
using STRUCT = grpc_examples::Hello;
using ATTR_TYPE = std::string;
static decltype(auto) get(STRUCT& s) { return *s.mutable_s();}
static decltype(auto) get(const STRUCT& s) { return s.s();}
};
};
template <class...T> struct List;
using ListOfAttributeMetaClasses = List<attributes::f, attributes::s>;
};
} // namespace meta
} // namespace grpc_examples
template <> struct meta::meta_of<grpc_examples::Hello> : std::type_identity<grpc_examples::meta::Hello> {};
template <> struct meta::value_type<grpc_examples::meta::Hello::attributes::f> {using type = double;};

namespace grpc_examples {
namespace meta {
struct Simple {
static constexpr const char* name() { return "Simple"; }
static constexpr const char* description() { return "Simple"; }
struct attributes {
struct i {
static constexpr std::array<const char, 2> ct_name() { return {"i"}; }
static constexpr const char* name() { return "i"; }
static constexpr const char* description() { return "i"; }
using STRUCT = grpc_examples::Simple;
using ATTR_TYPE = int64_t;
static decltype(auto) get(STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::i, &STRUCT::set_i);}
static decltype(auto) get(const STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::i);}
};
struct j {
static constexpr std::array<const char, 2> ct_name() { return {"j"}; }
static constexpr const char* name() { return "j"; }
static constexpr const char* description() { return "j"; }
using STRUCT = grpc_examples::Simple;
using ATTR_TYPE = int64_t;
static decltype(auto) get(STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::j, &STRUCT::set_j);}
static decltype(auto) get(const STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::j);}
};
struct f {
static constexpr std::array<const char, 2> ct_name() { return {"f"}; }
static constexpr const char* name() { return "f"; }
static constexpr const char* description() { return "f"; }
using STRUCT = grpc_examples::Simple;
using ATTR_TYPE = double;
static decltype(auto) get(STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::f, &STRUCT::set_f);}
static decltype(auto) get(const STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::f);}
};
};
template <class...T> struct List;
using ListOfAttributeMetaClasses = List<attributes::i, attributes::j, attributes::f>;
};
} // namespace meta
} // namespace grpc_examples
template <> struct meta::meta_of<grpc_examples::Simple> : std::type_identity<grpc_examples::meta::Simple> {};
template <> struct meta::value_type<grpc_examples::meta::Simple::attributes::i> {using type = int64_t;};
template <> struct meta::value_type<grpc_examples::meta::Simple::attributes::j> {using type = int64_t;};
template <> struct meta::value_type<grpc_examples::meta::Simple::attributes::f> {using type = double;};

namespace grpc_examples {
namespace meta {
struct Complex {
static constexpr const char* name() { return "Complex"; }
static constexpr const char* description() { return "Complex"; }
struct attributes {
struct s {
static constexpr std::array<const char, 2> ct_name() { return {"s"}; }
static constexpr const char* name() { return "s"; }
static constexpr const char* description() { return "s"; }
using STRUCT = grpc_examples::Complex;
using ATTR_TYPE = ::grpc_examples::Simple;
static decltype(auto) get(STRUCT& s) { return *s.mutable_s();}
static decltype(auto) get(const STRUCT& s) { return s.s();}
};
struct a {
static constexpr std::array<const char, 2> ct_name() { return {"a"}; }
static constexpr const char* name() { return "a"; }
static constexpr const char* description() { return "a"; }
using STRUCT = grpc_examples::Complex;
using ATTR_TYPE = ::meta::grpc::DynamicArrayWrapper<::grpc_examples::Simple, ::google::protobuf::RepeatedPtrField>;
static decltype(auto) get(STRUCT& s) { return ATTR_TYPE(*s.mutable_a());}
static decltype(auto) get(const STRUCT& s) { return ::meta::grpc::ConstDynamicArrayWrapper(s.a());}
};
};
template <class...T> struct List;
using ListOfAttributeMetaClasses = List<attributes::s, attributes::a>;
};
} // namespace meta
} // namespace grpc_examples
template <> struct meta::meta_of<grpc_examples::Complex> : std::type_identity<grpc_examples::meta::Complex> {};

// clang-format on
