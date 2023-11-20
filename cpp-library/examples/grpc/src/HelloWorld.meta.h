// clang-format off
#pragma once

#include "meta/grpc.h"
#include "meta/meta.h"
#include "HelloWorld.pb.h"

namespace grpc_examples {
namespace meta {
struct Color {
static const char* enum2string(int i) { return grpc_examples::Color_Name(static_cast<grpc_examples::Color>(i)).c_str(); }
static const char* enum2string(grpc_examples::Color c) { return grpc_examples::Color_Name(c).c_str(); }
static grpc_examples::Color string2enum(const char* enumAsText) {if (strcmp("RED", enumAsText) == 0) return grpc_examples::Color::RED;
else if (strcmp("GREEN", enumAsText) == 0) return grpc_examples::Color::GREEN;
else if (strcmp("BLUE", enumAsText) == 0) return grpc_examples::Color::BLUE;
else throw new std::runtime_error("grpc_examples::Color: string for string2enum");};
};
} // namespace meta
std::ostream& operator<<(std::ostream& os, Color color) { return os << Color_Name(color); }} // namespace grpc_examples
template <> struct meta::meta_of<grpc_examples::Color> : std::type_identity<grpc_examples::meta::Color> {};
template <typename Message> struct meta::meta_of<meta::grpc::FieldWrapper<grpc_examples::Color, Message>> : std::type_identity<grpc_examples::meta::Color> {};

namespace grpc_examples {
namespace meta {
struct Payload {
static constexpr const char* name() { return "Payload"; }
static constexpr const char* description() { return "Payload"; }
struct attributes {
struct id {
static constexpr std::array<const char, 3> ct_name() { return {"id"}; }
static constexpr const char* name() { return "id"; }
static constexpr const char* description() { return "id"; }
using STRUCT = grpc_examples::Payload;
using ATTR_TYPE = uint64_t;
static decltype(auto) get(STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::id, &STRUCT::set_id);}
static decltype(auto) get(const STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::id);}
};
struct color {
static constexpr std::array<const char, 6> ct_name() { return {"color"}; }
static constexpr const char* name() { return "color"; }
static constexpr const char* description() { return "color"; }
using STRUCT = grpc_examples::Payload;
using ATTR_TYPE = ::grpc_examples::Color;
static decltype(auto) get(STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::color, &STRUCT::set_color);}
static decltype(auto) get(const STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::color);}
};
};
template <class...T> struct List;
using ListOfAttributeMetaClasses = List<attributes::id, attributes::color>;
};
} // namespace meta
} // namespace grpc_examples
template <> struct meta::meta_of<grpc_examples::Payload> : std::type_identity<grpc_examples::meta::Payload> {};
template <> struct meta::value_type<grpc_examples::meta::Payload::attributes::id> {using type = uint64_t;};
template <> struct meta::value_type<grpc_examples::meta::Payload::attributes::color> {using type = ::grpc_examples::Color;};

namespace grpc_examples {
namespace meta {
struct HelloWorld {
static constexpr const char* name() { return "HelloWorld"; }
static constexpr const char* description() { return "HelloWorld"; }
struct attributes {
struct id {
static constexpr std::array<const char, 3> ct_name() { return {"id"}; }
static constexpr const char* name() { return "id"; }
static constexpr const char* description() { return "id"; }
using STRUCT = grpc_examples::HelloWorld;
using ATTR_TYPE = uint64_t;
static decltype(auto) get(STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::id, &STRUCT::set_id);}
static decltype(auto) get(const STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::id);}
};
struct header {
static constexpr std::array<const char, 7> ct_name() { return {"header"}; }
static constexpr const char* name() { return "header"; }
static constexpr const char* description() { return "header"; }
using STRUCT = grpc_examples::HelloWorld;
using ATTR_TYPE = ::grpc_examples::Payload;
static decltype(auto) get(STRUCT& s) { return *s.mutable_header();}
static decltype(auto) get(const STRUCT& s) { return s.header();}
};
struct index {
static constexpr std::array<const char, 6> ct_name() { return {"index"}; }
static constexpr const char* name() { return "index"; }
static constexpr const char* description() { return "index"; }
using STRUCT = grpc_examples::HelloWorld;
using ATTR_TYPE = uint64_t;
static decltype(auto) get(STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::index, &STRUCT::set_index);}
static decltype(auto) get(const STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::index);}
};
struct message {
static constexpr std::array<const char, 8> ct_name() { return {"message"}; }
static constexpr const char* name() { return "message"; }
static constexpr const char* description() { return "message"; }
using STRUCT = grpc_examples::HelloWorld;
using ATTR_TYPE = std::string;
static decltype(auto) get(STRUCT& s) { return *s.mutable_message();}
static decltype(auto) get(const STRUCT& s) { return s.message();}
};
struct color {
static constexpr std::array<const char, 6> ct_name() { return {"color"}; }
static constexpr const char* name() { return "color"; }
static constexpr const char* description() { return "color"; }
using STRUCT = grpc_examples::HelloWorld;
using ATTR_TYPE = ::grpc_examples::Color;
static decltype(auto) get(STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::color, &STRUCT::set_color);}
static decltype(auto) get(const STRUCT& s) { return ::meta::grpc::FieldWrapper(s, &STRUCT::color);}
};
struct colors {
static constexpr std::array<const char, 7> ct_name() { return {"colors"}; }
static constexpr const char* name() { return "colors"; }
static constexpr const char* description() { return "colors"; }
using STRUCT = grpc_examples::HelloWorld;
using ATTR_TYPE = ::meta::grpc::DynamicArrayWrapper<int, ::google::protobuf::RepeatedField>;
static decltype(auto) get(STRUCT& s) { return ATTR_TYPE(*s.mutable_colors());}
static decltype(auto) get(const STRUCT& s) { return ::meta::grpc::ConstDynamicArrayWrapper(s.colors());}
};
struct values {
static constexpr std::array<const char, 7> ct_name() { return {"values"}; }
static constexpr const char* name() { return "values"; }
static constexpr const char* description() { return "values"; }
using STRUCT = grpc_examples::HelloWorld;
using ATTR_TYPE = ::meta::grpc::DynamicArrayWrapper<uint64_t, ::google::protobuf::RepeatedField>;
static decltype(auto) get(STRUCT& s) { return ATTR_TYPE(*s.mutable_values());}
static decltype(auto) get(const STRUCT& s) { return ::meta::grpc::ConstDynamicArrayWrapper(s.values());}
};
struct arrayOfStructs {
static constexpr std::array<const char, 15> ct_name() { return {"arrayOfStructs"}; }
static constexpr const char* name() { return "arrayOfStructs"; }
static constexpr const char* description() { return "arrayOfStructs"; }
using STRUCT = grpc_examples::HelloWorld;
using ATTR_TYPE = ::meta::grpc::DynamicArrayWrapper<::grpc_examples::Payload, ::google::protobuf::RepeatedPtrField>;
static decltype(auto) get(STRUCT& s) { return ATTR_TYPE(*s.mutable_arrayofstructs());}
static decltype(auto) get(const STRUCT& s) { return ::meta::grpc::ConstDynamicArrayWrapper(s.arrayofstructs());}
};
};
template <class...T> struct List;
using ListOfAttributeMetaClasses = List<attributes::id, attributes::header, attributes::index, attributes::message, attributes::color, attributes::colors, attributes::values, attributes::arrayOfStructs>;
};
} // namespace meta
} // namespace grpc_examples
template <> struct meta::meta_of<grpc_examples::HelloWorld> : std::type_identity<grpc_examples::meta::HelloWorld> {};
template <> struct meta::value_type<grpc_examples::meta::HelloWorld::attributes::id> {using type = uint64_t;};
template <> struct meta::value_type<grpc_examples::meta::HelloWorld::attributes::index> {using type = uint64_t;};
template <> struct meta::value_type<grpc_examples::meta::HelloWorld::attributes::color> {using type = ::grpc_examples::Color;};
template <> struct meta::value_type<grpc_examples::meta::HelloWorld::attributes::colors> { using type = ::grpc_examples::Color; };
template <> struct meta::value_type<grpc_examples::meta::HelloWorld::attributes::colors::ATTR_TYPE> { using type = ::grpc_examples::Color; };

// clang-format on
