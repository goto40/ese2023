#ifndef __EXAMPLES_FASTDDS_SRC_DDS_EXAMPLES_HELLOWORLD_IDL
#define __EXAMPLES_FASTDDS_SRC_DDS_EXAMPLES_HELLOWORLD_IDL
#include <utility>
#include <type_traits>
#include "meta/meta.h"
namespace dds_examples {
  // enum ColorT { RED,GREEN,BLUE }
  namespace meta {
    struct ColorT {
      static const char* enum2string(dds_examples::ColorT e) {
        if (e == dds_examples::ColorT::RED) return "RED";
        else if (e == dds_examples::ColorT::GREEN) return "GREEN";
        else if (e == dds_examples::ColorT::BLUE) return "BLUE";
        else throw new std::runtime_error("dds_examples::ColorT: unmapped enum value");
      }
      static dds_examples::ColorT string2enum(const char* enumAsText) {
        if (strcmp("RED",enumAsText)==0) return dds_examples::ColorT::RED;
        else if (strcmp("GREEN",enumAsText)==0) return dds_examples::ColorT::GREEN;
        else if (strcmp("BLUE",enumAsText)==0) return dds_examples::ColorT::BLUE;
        else throw new std::runtime_error("dds_examples::ColorT: string for string2enum");
      }
    }; // struct
  }
  inline std::ostream& operator<< (std::ostream& os, const dds_examples::ColorT& e) {
    os << meta::ColorT::enum2string(e);
    return os;
  }
  namespace meta {
    struct PayloadT {
      static constexpr const char* name() { return "PayloadT"; }
      static constexpr const char* description() { return "description not implemented for dds"; }
      struct attributes {
        // raw id: IDL::Type::ULongLong
        struct id {
          using STRUCT = dds_examples::PayloadT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::PayloadT>().id())>;
          static constexpr const char* name() { return "id"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 3> ct_name() { return {"id"}; }
          static auto& get(STRUCT& s) { return s.id(); }
          static auto get(const STRUCT& s) { return s.id(); }
        }; // struct id
        // color: dds_examples::ColorT
        struct color {
          using STRUCT = dds_examples::PayloadT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::PayloadT>().color())>;
          static constexpr const char* name() { return "color"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 6> ct_name() { return {"color"}; }
          static auto& get(STRUCT& s) { return s.color(); }
          static auto get(const STRUCT& s) { return s.color(); }
        }; // struct color
      }; // struct attributes
      template <class... T>
      struct List;
      using ListOfAttributeMetaClasses =
        List<attributes::id,attributes::color>;
    }; // struct PayloadT
  } // namespace meta
  namespace meta {
    struct HelloWorldT {
      static constexpr const char* name() { return "HelloWorldT"; }
      static constexpr const char* description() { return "description not implemented for dds"; }
      struct attributes {
        // raw id: IDL::Type::ULongLong
        struct id {
          using STRUCT = dds_examples::HelloWorldT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::HelloWorldT>().id())>;
          static constexpr const char* name() { return "id"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 3> ct_name() { return {"id"}; }
          static auto& get(STRUCT& s) { return s.id(); }
          static auto get(const STRUCT& s) { return s.id(); }
        }; // struct id
        // header: dds_examples::PayloadT
        struct header {
          using STRUCT = dds_examples::HelloWorldT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::HelloWorldT>().header())>;
          static constexpr const char* name() { return "header"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 7> ct_name() { return {"header"}; }
          static auto& get(STRUCT& s) { return s.header(); }
          static auto get(const STRUCT& s) { return s.header(); }
        }; // struct header
        // raw index: IDL::Type::ULongLong
        struct index {
          using STRUCT = dds_examples::HelloWorldT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::HelloWorldT>().index())>;
          static constexpr const char* name() { return "index"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 6> ct_name() { return {"index"}; }
          static auto& get(STRUCT& s) { return s.index(); }
          static auto get(const STRUCT& s) { return s.index(); }
        }; // struct index
        // raw message: IDL::Type::String
        struct message {
          using STRUCT = dds_examples::HelloWorldT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::HelloWorldT>().message())>;
          static constexpr const char* name() { return "message"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 8> ct_name() { return {"message"}; }
          static auto& get(STRUCT& s) { return s.message(); }
          static auto get(const STRUCT& s) { return s.message(); }
        }; // struct message
        // color: dds_examples::ColorT
        struct color {
          using STRUCT = dds_examples::HelloWorldT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::HelloWorldT>().color())>;
          static constexpr const char* name() { return "color"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 6> ct_name() { return {"color"}; }
          static auto& get(STRUCT& s) { return s.color(); }
          static auto get(const STRUCT& s) { return s.color(); }
        }; // struct color
        // colors: dds_examples::ColorSequenceT
        struct colors {
          using STRUCT = dds_examples::HelloWorldT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::HelloWorldT>().colors())>;
          static constexpr const char* name() { return "colors"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 7> ct_name() { return {"colors"}; }
          static auto& get(STRUCT& s) { return s.colors(); }
          static auto get(const STRUCT& s) { return s.colors(); }
        }; // struct colors
        // values: dds_examples::ThreeValuesT
        struct values {
          using STRUCT = dds_examples::HelloWorldT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::HelloWorldT>().values())>;
          static constexpr const char* name() { return "values"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 7> ct_name() { return {"values"}; }
          static auto& get(STRUCT& s) { return s.values(); }
          static auto get(const STRUCT& s) { return s.values(); }
        }; // struct values
        // arrayOfStructs: dds_examples::PayloadSequenceT
        struct arrayOfStructs {
          using STRUCT = dds_examples::HelloWorldT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::HelloWorldT>().arrayOfStructs())>;
          static constexpr const char* name() { return "arrayOfStructs"; }
          static constexpr const char* description() { return "description not implemented for dds"; }
          static constexpr std::array<const char, 15> ct_name() { return {"arrayOfStructs"}; }
          static auto& get(STRUCT& s) { return s.arrayOfStructs(); }
          static auto get(const STRUCT& s) { return s.arrayOfStructs(); }
        }; // struct arrayOfStructs
      }; // struct attributes
      template <class... T>
      struct List;
      using ListOfAttributeMetaClasses =
        List<attributes::id,attributes::header,attributes::index,attributes::message,attributes::color,attributes::colors,attributes::values,attributes::arrayOfStructs>;
    }; // struct HelloWorldT
  } // namespace meta
} // close namespace meta::dds_examples
template <>
struct meta::meta_of<dds_examples::ColorT>:
  std::type_identity<dds_examples::meta::ColorT> {};
template <>
struct meta::meta_of<dds_examples::PayloadT>:
  std::type_identity<dds_examples::meta::PayloadT> {};
template <>
struct meta::meta_of<dds_examples::HelloWorldT>:
  std::type_identity<dds_examples::meta::HelloWorldT> {};
#endif
