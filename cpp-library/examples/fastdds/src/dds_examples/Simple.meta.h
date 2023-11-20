#ifndef __EXAMPLES_FASTDDS_SRC_DDS_EXAMPLES_SIMPLE_IDL
#define __EXAMPLES_FASTDDS_SRC_DDS_EXAMPLES_SIMPLE_IDL
#include <utility>
#include <type_traits>
#include "meta/meta.h"
namespace dds_examples {
  namespace meta {
    struct SimpleT {
      static constexpr const char* name() { return "SimpleT"; }
      static constexpr const char* description() { return "description not implemented for dds"; }
      struct attributes {
        // raw j: IDL::Type::LongLong
        struct j {
          using STRUCT = dds_examples::SimpleT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::SimpleT>().j())>;
          static constexpr const char* name() { return "j"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 2> ct_name() { return {"j"}; }
          static auto& get(STRUCT& s) { return s.j(); }
          static auto get(const STRUCT& s) { return s.j(); }
        }; // struct j
        // raw i: IDL::Type::LongLong
        struct i {
          using STRUCT = dds_examples::SimpleT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::SimpleT>().i())>;
          static constexpr const char* name() { return "i"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 2> ct_name() { return {"i"}; }
          static auto& get(STRUCT& s) { return s.i(); }
          static auto get(const STRUCT& s) { return s.i(); }
        }; // struct i
        // raw f: IDL::Type::Double
        struct f {
          using STRUCT = dds_examples::SimpleT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::SimpleT>().f())>;
          static constexpr const char* name() { return "f"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 2> ct_name() { return {"f"}; }
          static auto& get(STRUCT& s) { return s.f(); }
          static auto get(const STRUCT& s) { return s.f(); }
        }; // struct f
      }; // struct attributes
      template <class... T>
      struct List;
      using ListOfAttributeMetaClasses =
        List<attributes::j,attributes::i,attributes::f>;
    }; // struct SimpleT
  } // namespace meta
  namespace meta {
    struct ComplexT {
      static constexpr const char* name() { return "ComplexT"; }
      static constexpr const char* description() { return "description not implemented for dds"; }
      struct attributes {
        // s: dds_examples::SimpleT
        struct s {
          using STRUCT = dds_examples::ComplexT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::ComplexT>().s())>;
          static constexpr const char* name() { return "s"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 2> ct_name() { return {"s"}; }
          static auto& get(STRUCT& s) { return s.s(); }
          static auto get(const STRUCT& s) { return s.s(); }
        }; // struct s
        // a: dds_examples::SimpleArray3T
        struct a {
          using STRUCT = dds_examples::ComplexT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::ComplexT>().a())>;
          static constexpr const char* name() { return "a"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 2> ct_name() { return {"a"}; }
          static auto& get(STRUCT& s) { return s.a(); }
          static auto get(const STRUCT& s) { return s.a(); }
        }; // struct a
      }; // struct attributes
      template <class... T>
      struct List;
      using ListOfAttributeMetaClasses =
        List<attributes::s,attributes::a>;
    }; // struct ComplexT
  } // namespace meta
} // close namespace meta::dds_examples
template <>
struct meta::meta_of<dds_examples::SimpleT>:
  std::type_identity<dds_examples::meta::SimpleT> {};
template <>
struct meta::meta_of<dds_examples::ComplexT>:
  std::type_identity<dds_examples::meta::ComplexT> {};
#endif
