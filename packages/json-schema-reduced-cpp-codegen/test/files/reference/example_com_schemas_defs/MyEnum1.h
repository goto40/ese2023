#pragma once
#include <cstring>
#include <stdexcept>
#include <ostream>

namespace example_com_schemas_defs {
    enum class MyEnum1 {
        a,
        b,
        c
    };

    namespace meta {
        struct MyEnum1 {
            static const char* enum2string(example_com_schemas_defs::MyEnum1 e) {
                if (e == example_com_schemas_defs::MyEnum1::a) return "a";
                else if (e == example_com_schemas_defs::MyEnum1::b) return "b";
                else if (e == example_com_schemas_defs::MyEnum1::c) return "c";
                else throw new std::runtime_error("example_com_schemas_defs::MyEnum1: unmapped enum value");
            }
            static example_com_schemas_defs::MyEnum1 string2enum(const char* enumAsText) {
                if (strcmp("a",enumAsText)==0) return example_com_schemas_defs::MyEnum1::a;
                else if (strcmp("b",enumAsText)==0) return example_com_schemas_defs::MyEnum1::b;
                else if (strcmp("c",enumAsText)==0) return example_com_schemas_defs::MyEnum1::c;
                else throw new std::runtime_error("example_com_schemas_defs::MyEnum1: unmapped string for string2enum");
            }
        };
    }

    inline std::ostream& operator<< (std::ostream& os, const example_com_schemas_defs::MyEnum1& e) {
        os << meta::MyEnum1::enum2string(e);
        return os;
    }

}
template<> struct meta::meta_of<example_com_schemas_defs::MyEnum1>: std::type_identity<example_com_schemas_defs::meta::MyEnum1> {};
