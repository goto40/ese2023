#pragma once
#include <type_traits>
#include <string>
#include <cstdint>
#include <array>
#include <vector>

#include "meta/type_traits.h"

#include "example_com_schemas_defs/MyEnum1.h"
#include "example_com_schemas_defs/MyObj1.h"

namespace example_com_schemas_defs {
    struct MyObj2 {
        bool b; // attribute b
        std::string s; // attribute s
        int8_t i8; // attribute i8
        int16_t i16a; // attribute i16a
        int16_t i16b; // attribute i16b
        int16_t i16; // attribute i16
        uint16_t u16; // attribute u16
        uint32_t u32; // attribute u32
        double f; // attribute f
        example_com_schemas_defs::MyObj1 o; // attribute o
        example_com_schemas_defs::MyEnum1 e; // attribute e
        std::vector<uint16_t> a1; // attribute a1
        std::vector<std::string> a2; // attribute a2
        std::array<example_com_schemas_defs::MyEnum1,10> a3; // attribute a3
        std::vector<example_com_schemas_defs::MyObj1> a4; // attribute a4
    };

    namespace meta {
        struct MyObj2 {
            static constexpr const char* name() { return "MyObj2"; }
            static constexpr const char* description() { return ""; }
            struct attributes {
                struct b {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = bool;
                    static constexpr const char* name() { return "b"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,2> ct_name() { return {"b"}; }
                    static constexpr auto& get(STRUCT &s) { return s.b; }
                    static constexpr const auto& get(const STRUCT &s) { return s.b; }
                };
                struct s {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = std::string;
                    static constexpr const char* name() { return "s"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,2> ct_name() { return {"s"}; }
                    static constexpr auto& get(STRUCT &s) { return s.s; }
                    static constexpr const auto& get(const STRUCT &s) { return s.s; }
                };
                struct i8 {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = int8_t;
                    static constexpr const char* name() { return "i8"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,3> ct_name() { return {"i8"}; }
                    static constexpr auto& get(STRUCT &s) { return s.i8; }
                    static constexpr const auto& get(const STRUCT &s) { return s.i8; }
                };
                struct i16a {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = int16_t;
                    static constexpr const char* name() { return "i16a"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,5> ct_name() { return {"i16a"}; }
                    static constexpr auto& get(STRUCT &s) { return s.i16a; }
                    static constexpr const auto& get(const STRUCT &s) { return s.i16a; }
                };
                struct i16b {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = int16_t;
                    static constexpr const char* name() { return "i16b"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,5> ct_name() { return {"i16b"}; }
                    static constexpr auto& get(STRUCT &s) { return s.i16b; }
                    static constexpr const auto& get(const STRUCT &s) { return s.i16b; }
                };
                struct i16 {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = int16_t;
                    static constexpr const char* name() { return "i16"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,4> ct_name() { return {"i16"}; }
                    static constexpr auto& get(STRUCT &s) { return s.i16; }
                    static constexpr const auto& get(const STRUCT &s) { return s.i16; }
                };
                struct u16 {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = uint16_t;
                    static constexpr const char* name() { return "u16"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,4> ct_name() { return {"u16"}; }
                    static constexpr auto& get(STRUCT &s) { return s.u16; }
                    static constexpr const auto& get(const STRUCT &s) { return s.u16; }
                };
                struct u32 {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = uint32_t;
                    static constexpr const char* name() { return "u32"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,4> ct_name() { return {"u32"}; }
                    static constexpr auto& get(STRUCT &s) { return s.u32; }
                    static constexpr const auto& get(const STRUCT &s) { return s.u32; }
                };
                struct f {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = double;
                    static constexpr const char* name() { return "f"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,2> ct_name() { return {"f"}; }
                    static constexpr auto& get(STRUCT &s) { return s.f; }
                    static constexpr const auto& get(const STRUCT &s) { return s.f; }
                };
                struct o {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = example_com_schemas_defs::MyObj1;
                    static constexpr const char* name() { return "o"; }
                    static constexpr const char* description() { return "information for a ref!"; }
                    static constexpr std::array<const char,2> ct_name() { return {"o"}; }
                    static constexpr auto& get(STRUCT &s) { return s.o; }
                    static constexpr const auto& get(const STRUCT &s) { return s.o; }
                };
                struct e {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = example_com_schemas_defs::MyEnum1;
                    static constexpr const char* name() { return "e"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,2> ct_name() { return {"e"}; }
                    static constexpr auto& get(STRUCT &s) { return s.e; }
                    static constexpr const auto& get(const STRUCT &s) { return s.e; }
                };
                struct a1 {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = std::vector<uint16_t>;
                    static constexpr const char* name() { return "a1"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,3> ct_name() { return {"a1"}; }
                    static constexpr auto& get(STRUCT &s) { return s.a1; }
                    static constexpr const auto& get(const STRUCT &s) { return s.a1; }
                };
                struct a2 {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = std::vector<std::string>;
                    static constexpr const char* name() { return "a2"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,3> ct_name() { return {"a2"}; }
                    static constexpr auto& get(STRUCT &s) { return s.a2; }
                    static constexpr const auto& get(const STRUCT &s) { return s.a2; }
                };
                struct a3 {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = std::array<example_com_schemas_defs::MyEnum1,10>;
                    static constexpr const char* name() { return "a3"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,3> ct_name() { return {"a3"}; }
                    static constexpr auto& get(STRUCT &s) { return s.a3; }
                    static constexpr const auto& get(const STRUCT &s) { return s.a3; }
                };
                struct a4 {
                    using STRUCT = example_com_schemas_defs::MyObj2;
                    using ATTR_TYPE = std::vector<example_com_schemas_defs::MyObj1>;
                    static constexpr const char* name() { return "a4"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,3> ct_name() { return {"a4"}; }
                    static constexpr auto& get(STRUCT &s) { return s.a4; }
                    static constexpr const auto& get(const STRUCT &s) { return s.a4; }
                };
            };

            template<class ...T> struct List;
            using ListOfAttributeMetaClasses = List<
                attributes::b,
                attributes::s,
                attributes::i8,
                attributes::i16a,
                attributes::i16b,
                attributes::i16,
                attributes::u16,
                attributes::u32,
                attributes::f,
                attributes::o,
                attributes::e,
                attributes::a1,
                attributes::a2,
                attributes::a3,
                attributes::a4
            >;
        };
    }
}

template<> struct meta::meta_of<example_com_schemas_defs::MyObj2>: std::type_identity<example_com_schemas_defs::meta::MyObj2> {};
