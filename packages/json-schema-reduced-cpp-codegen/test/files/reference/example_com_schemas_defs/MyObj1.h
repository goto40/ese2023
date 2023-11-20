#pragma once
#include <type_traits>
#include <string>
#include <cstdint>
#include <array>

#include "meta/type_traits.h"

namespace example_com_schemas_defs {
    struct MyObj1 {
        std::string s; // attribute s
        int64_t i; // attribute i
        double f; // attribute f
    };

    namespace meta {
        struct MyObj1 {
            static constexpr const char* name() { return "MyObj1"; }
            static constexpr const char* description() { return "MyObj1 information"; }
            struct attributes {
                struct s {
                    using STRUCT = example_com_schemas_defs::MyObj1;
                    using ATTR_TYPE = std::string;
                    static constexpr const char* name() { return "s"; }
                    static constexpr const char* description() { return "s information"; }
                    static constexpr std::array<const char,2> ct_name() { return {"s"}; }
                    static constexpr auto& get(STRUCT &s) { return s.s; }
                    static constexpr const auto& get(const STRUCT &s) { return s.s; }
                };
                struct i {
                    using STRUCT = example_com_schemas_defs::MyObj1;
                    using ATTR_TYPE = int64_t;
                    static constexpr const char* name() { return "i"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,2> ct_name() { return {"i"}; }
                    static constexpr auto& get(STRUCT &s) { return s.i; }
                    static constexpr const auto& get(const STRUCT &s) { return s.i; }
                };
                struct f {
                    using STRUCT = example_com_schemas_defs::MyObj1;
                    using ATTR_TYPE = double;
                    static constexpr const char* name() { return "f"; }
                    static constexpr const char* description() { return ""; }
                    static constexpr std::array<const char,2> ct_name() { return {"f"}; }
                    static constexpr auto& get(STRUCT &s) { return s.f; }
                    static constexpr const auto& get(const STRUCT &s) { return s.f; }
                };
            };

            template<class ...T> struct List;
            using ListOfAttributeMetaClasses = List<
                attributes::s,
                attributes::i,
                attributes::f
            >;
        };
    }
}

template<> struct meta::meta_of<example_com_schemas_defs::MyObj1>: std::type_identity<example_com_schemas_defs::meta::MyObj1> {};
