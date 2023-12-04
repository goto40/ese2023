#pragma once
#include <type_traits>
#include <string>
#include <cstdint>
#include <array>

#include "meta/type_traits.h"

#include "example_com_include2_schema/MyObj2.h"

namespace example_com_include1_schema {
    struct MyObj1 {
        std::string s; // attribute s
        int64_t i; // attribute i
        example_com_include2_schema::MyObj2 o; // attribute o
    };

    namespace meta {
        struct MyObj1 {
            static constexpr const char* name() { return "MyObj1"; }
            static constexpr const char* description() { return ""; }
            struct attributes {
                struct s {
                    using STRUCT = example_com_include1_schema::MyObj1;
                    using ATTR_TYPE = std::string;
                    static constexpr const char* name() { return "s"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,2> ct_name() { return {"s"}; }
                    static constexpr auto& get(STRUCT &s) { return s.s; }
                    static constexpr const auto& get(const STRUCT &s) { return s.s; }
                };
                struct i {
                    using STRUCT = example_com_include1_schema::MyObj1;
                    using ATTR_TYPE = int64_t;
                    static constexpr const char* name() { return "i"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,2> ct_name() { return {"i"}; }
                    static constexpr auto& get(STRUCT &s) { return s.i; }
                    static constexpr const auto& get(const STRUCT &s) { return s.i; }
                };
                struct o {
                    using STRUCT = example_com_include1_schema::MyObj1;
                    using ATTR_TYPE = example_com_include2_schema::MyObj2;
                    static constexpr const char* name() { return "o"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,2> ct_name() { return {"o"}; }
                    static constexpr auto& get(STRUCT &s) { return s.o; }
                    static constexpr const auto& get(const STRUCT &s) { return s.o; }
                };
            };

            template<class ...T> struct List;
            using ListOfAttributeMetaClasses = List<
                attributes::s,
                attributes::i,
                attributes::o
            >;
        };
    }
}

template<> struct meta::meta_of<example_com_include1_schema::MyObj1>: std::type_identity<example_com_include1_schema::meta::MyObj1> {};
