/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#pragma once
#include <algorithm>
#include <concepts>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

#include "type_traits.h"

namespace meta {

/** Visitor concept */
template <class VISITOR, class META, class... STRUCTS>
concept IsVisitor = requires(VISITOR& v, STRUCTS&... values) {
  requires attribute_meta<META>;
  { v.template visitScalarString<META>(values...) } -> std::same_as<void>;
  { v.template visitScalarEnum<META>(values...) } -> std::same_as<void>;
  { v.template visitScalarStruct<META>(values...) } -> std::same_as<void>;
  { v.template visitScalarRawType<META>(values...) } -> std::same_as<void>;
  { v.template visitArrayString<META>(values...) } -> std::same_as<void>;
  { v.template visitArrayEnum<META>(values...) } -> std::same_as<void>;
  { v.template visitArrayStruct<META>(values...) } -> std::same_as<void>;
  { v.template visitArrayRawType<META>(values...) } -> std::same_as<void>;
};

/** Helper enum for visitor pattern: unpack optionals while visiting? */
enum class Unpack : bool { No = false, Yes = true };

/** Helper function for visitor pattern: unpack optionals while visiting */
template <Unpack do_unpack, class T>
T unpack(T x) {  // T can be a reference or not here!
  return x;
}

/** Helper function for visitor pattern: unpack optionals while visiting */
template <Unpack do_unpack, class, class T>
decltype(auto) unpack(std::optional<T>& x) {
  if constexpr (do_unpack == Unpack::Yes)
    return *x;
  else
    return x;
}

/** Helper function for visitor pattern: unpack optionals while visiting */
template <Unpack do_unpack, class, class T>
decltype(auto) unpack(const std::optional<T>& x) {
  if constexpr (do_unpack == Unpack::Yes)
    return *x;
  else
    return x;
}

/** Visitor Wrapper to provide high-level callbacks (visitScalar... etc.) */
template <class VISITOR, Unpack do_unpack = Unpack::No,
          class = typename std::enable_if<
              !std::is_lvalue_reference<VISITOR>::value>::type>
struct BasicVisitor {
  VISITOR v;
  BasicVisitor(VISITOR&& v) : v{v} {}
  template <class META, class... STRUCTS>
    requires(IsVisitor<VISITOR, META, STRUCTS...>)
  void visit(STRUCTS&... s) {
    // std::cout << "DEBUG " << META::name() << "\n";
    static_assert(meta::IsVisitor<VISITOR, META, STRUCTS...>,
                  "you must pass a visitor!");
    if constexpr (do_unpack == Unpack::Yes) {
      if constexpr (!required_attribute_meta<META>) {
        bool all = ((META::get(s) != std::nullopt) && ...);
        bool some = ((META::get(s) != std::nullopt) || ...);
        if (some ^ all)
          throw std::runtime_error(
              std::string("you cannot use unpack=true, when passing objects "
                          "with different optional elements: ") +
              META::name());
        if (!all) return;
      }
    }
    std::tuple<decltype(unpack<do_unpack, decltype(META::get(s))>(
        META::get(s)))...>
        values = {unpack<do_unpack, decltype(META::get(s))>(META::get(s))...};
    if constexpr (scalar_attribute_meta<META>) {
      if constexpr (string_attribute_meta<META>) {
        std::apply(
            [this](auto&... args) {
              v.template visitScalarString<META>(args...);
            },
            values);
      } else if constexpr (enum_attribute_meta<META>) {
        std::apply(
            [this](auto&... args) {
              v.template visitScalarEnum<META>(args...);
            },
            values);
      } else if constexpr (struct_attribute_meta<META>) {
        std::apply(
            [this](auto&... args) {
              v.template visitScalarStruct<META>(args...);
            },
            values);
      } else {
        std::apply(
            [this](auto&... args) {
              v.template visitScalarRawType<META>(args...);
            },
            values);
      }
    } else if constexpr (array_attribute_meta<META>) {
      if constexpr (string_attribute_meta<META>) {
        std::apply(
            [this](auto&... args) {
              v.template visitArrayString<META>(args...);
            },
            values);
      } else if constexpr (enum_attribute_meta<META>) {
        std::apply(
            [this](auto&... args) { v.template visitArrayEnum<META>(args...); },
            values);
      } else if constexpr (struct_attribute_meta<META>) {
        std::apply(
            [this](auto&... args) {
              v.template visitArrayStruct<META>(args...);
            },
            values);
      } else {
        std::apply(
            [this](auto&... args) {
              v.template visitArrayRawType<META>(args...);
            },
            values);
      }
    } else {
      throw std::runtime_error("unexpected");
    }
  }
};

namespace details {
template <size_t index, class List>
struct GetMetaByIndex;

template <size_t index, template <class...> class List, class T, class... R>
struct GetMetaByIndex<index, List<T, R...>> {
  static_assert(index < sizeof...(R) + 1, "Index out of range");
  using type = typename GetMetaByIndex<index - 1, List<R...>>::type;
};

template <template <class...> class List, class T, class... R>
struct GetMetaByIndex<0, List<T, R...>> {
  using type = T;
};

template <class List>
struct GetNumEl;

template <template <class...> class List, class... R>
struct GetNumEl<List<R...>> {
  static constexpr size_t value = sizeof...(R);
};

constexpr bool areStringsEqual(const char* s1, const char* s2) {
  if (*s1 != *s2) return false;
  if (*s1 == 0 || *s2 == 0)
    return true;
  else
    return areStringsEqual(s1 + 1, s2 + 1);
}
}  // namespace details

/** get MetaClass of an attribute by index */
template <size_t index, class List>
using meta_by_index_t = typename details::GetMetaByIndex<index, List>::type;

/** get the number of attributes of a list of meta attributes */
template <class List>
static constexpr size_t num_el_v = details::GetNumEl<List>::value;

namespace details {
/**
 * visit one attribute, for each meta in list (starting at idx)
 */
template <size_t idx, class MetaList>
struct AcceptHelper {
  template <class VISITOR, class... MESSAGES>
  static void acceptAttributeByIndex(VISITOR&& v, MESSAGES&&... m) {
    v.template visit<meta_by_index_t<idx, MetaList>>(
        std::forward<MESSAGES>(m)...);
    if constexpr ((idx + 1) < num_el_v<MetaList>) {
      AcceptHelper<idx + 1, MetaList>::acceptAttributeByIndex(
          std::forward<VISITOR>(v), std::forward<MESSAGES>(m)...);
    }
  }
};

}  // namespace details

/** call a visitor for each attribute:
    For each attribute, call v.template visit<META>(struct1, struct2, struct3,
   ...) // at least one struct, Examples:
    - one struct: e.g. serialize one object (see json.h)
    - two structs: e.g. convert one object into another (see convert.h)
 */
template <class VISITOR, class MESSAGE, class... OTHER_MESSAGES>
void accept(VISITOR&& v, MESSAGE&& m, OTHER_MESSAGES&&... other) {
  using STRUCT_META = meta_of_t<std::remove_cvref_t<MESSAGE>>;
  static_assert(struct_meta<STRUCT_META>,
                "a struct with correct META info is required!");
  using MetaList = typename STRUCT_META::ListOfAttributeMetaClasses;
  static_assert(num_el_v<MetaList> > 0, "you need at lease on attribute");
  details::AcceptHelper<0, MetaList>::acceptAttributeByIndex(
      std::forward<VISITOR>(v), std::forward<MESSAGE>(m),
      std::forward<OTHER_MESSAGES>(other)...);
}

/** call a visitor for each attribute
    For each attribute, call v.template visit<META>() // no struct (e.g. compute
   max size of object for given type)
*/
template <class MESSAGE, class VISITOR>
void accept(VISITOR&& v) {
  using STRUCT_META = meta_of_t<MESSAGE>;
  static_assert(struct_meta<STRUCT_META>,
                "a struct with correct META info is required!");
  using MetaList = typename STRUCT_META::ListOfAttributeMetaClasses;
  static_assert(num_el_v<MetaList> > 0, "you need at lease on attribute");
  details::AcceptHelper<0, MetaList>::acceptAttributeByIndex(
      std::forward<VISITOR>(v));
}

namespace details {
// string as template parameter
// see: https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/
template <size_t N>
struct String {
  constexpr String(const char (&textIn)[N]) { std::copy_n(textIn, N, text); }
  constexpr String(const std::array<const char, N>& textIn) {
    std::copy(textIn.begin(), textIn.end(), text);
  }
  char text[N];
  constexpr operator const char*() const { return text; }
  constexpr bool isSame(const char* n) const {
    return areStringsEqual(n, text);
  }
};

template <
    String name, size_t idx, class MetaList,
    bool valid = idx<
        num_el_v<MetaList>,
        bool match = valid && areStringsEqual(
                                  name, meta_by_index_t<idx, MetaList>::
                                            name())> struct FindAttributeByName;
template <String name, size_t idx, class MetaList>
struct FindAttributeByName<name, idx, MetaList, true, true> {
  using type = meta_by_index_t<idx, MetaList>;
};
template <String name, size_t idx, class MetaList, bool valid>
struct FindAttributeByName<name, idx, MetaList, valid, false> {
  static_assert(valid, "attribute not found");
  using type = FindAttributeByName<name, idx + 1, MetaList>::type;
};

template <String name, size_t idx, class MetaList,
          bool valid = idx<num_el_v<MetaList>> struct HasAttributeByName;
template <String name, size_t idx, class MetaList>
struct HasAttributeByName<name, idx, MetaList, true> {
  static constexpr bool value =
      areStringsEqual(name, meta_by_index_t<idx, MetaList>::name()) ||
      HasAttributeByName<name, idx + 1, MetaList>::value;
};
template <String name, size_t idx, class MetaList>
struct HasAttributeByName<name, idx, MetaList, false> {
  static constexpr bool value = false;
};
}  // namespace details

/** access META of attributes by name */
template <details::String name, class MESSAGE>
using meta_by_name_t = details::FindAttributeByName<
    name, 0, typename meta_of_t<MESSAGE>::ListOfAttributeMetaClasses>::type;

/** access META of attributes by name */
template <details::String name, class MESSAGE>
constexpr bool has_meta_by_name_v = details::HasAttributeByName<
    name, 0, typename meta_of_t<MESSAGE>::ListOfAttributeMetaClasses>::value;

/** access META of attributes by name (const or non-const) */
template <details::String name, class MESSAGE>
constexpr auto& access_by_name(MESSAGE& m) {
  return meta_by_name_t<name, MESSAGE>::get(m);
}

// optional value tools/abstractions:

template <optional_type T>
void clear_optional(T& x) {
  x = std::nullopt;
}
template <optional_type T>
bool has_value(const T& x) {
  return x.has_value();
}
template <optional_type T>
void construct_optional(T& x) {
  x = meta::value_type_t<T>{};
}
template <optional_type T>
auto& unpack_optional(T& x) {
  return *x;
}

}  // namespace meta
