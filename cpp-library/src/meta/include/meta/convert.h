/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#pragma once

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#include "meta/meta.h"
#include "meta/type_traits.h"

/*
For "raw" ATTR_TYPE types
-------------------------

* meta::is_floating_point_v<T>  // std::is_floating_point_v of underlying type
* meta::is_signed_v<T>          // std::is_signed_v of underlying type
* meta::size_of<T>              // sizeof(underlying type)
* meta::value_type_t<T>         // get inner type of optional, array (better
meta::get_inner_type???)

For META classes
----------------

* meta::meta_of_t<T>                  // get meta class of enum or struct
* meta::enum_attribute_meta<META>     // check type of meta
* meta::string_attribute_meta<META>   // check type of meta
* optional_attribute_meta             // check type of meta

For optional aspects

* meta::construct_optional<VALUE_TYPE>(arg); // after this,
meta::unpack_optional returns a legal reference
* meta::unpack_optional<VALUE_TYPE>(arg),    // returns a reference to the
contents (can return the arg!)
* meta::clear_optional<VALUE_TYPE>(arg),     // clear the optional value (e.g.
with std::nullopt for standard std::optional)
*/

namespace meta {

template <class MESSAGE, meta::details::String name, class ConvertFunction>
struct CustomAttributeConverter {
  using MSG = MESSAGE;
  ConvertFunction convertFunction;  /// is called with (const SourceStruct&,
                                    /// DestAttribute&), see test/convert.cpp
                                    /// and FromOtherMessageVisitor::visitScalar
                                    /// and FromOtherMessageVisitor::visitArray
  constexpr static const auto& getName() { return name; }
  constexpr CustomAttributeConverter(ConvertFunction&& f)
      : convertFunction{f} {};
};
template <class MESSAGE, meta::details::String name, class ConvertFunction>
constexpr auto createCustomAttributeConverter(
    ConvertFunction&& convertFunction) {
  return CustomAttributeConverter<MESSAGE, name, ConvertFunction>{
      std::forward<ConvertFunction>(convertFunction)};
};

namespace details {
template <class C, size_t i = C::N>
struct CustomConverterHelper {
  template <class MESSAGE>
  constexpr static std::optional<size_t> converterIndex(const char* s) {
    if (std::is_same_v<MESSAGE, typename std::tuple_element<
                                    i - 1, typename C::TupleType>::type::MSG> &&
        std::tuple_element<i - 1, typename C::TupleType>::type::getName()
            .isSame(s)) {
      return i - 1;
    } else {
      return CustomConverterHelper<C, i - 1>::template converterIndex<MESSAGE>(
          s);
    }
  }
};
template <class C>
struct CustomConverterHelper<C, 0> {
  template <class MESSAGE>
  constexpr static std::optional<size_t> converterIndex(const char* s) {
    return std::nullopt;
  }
};
}  // namespace details

template <class... C>
struct CustomStructConverter {
  constexpr static size_t N = sizeof...(C);
  using TupleType = std::tuple<C...>;
  std::tuple<C...> converter;
  constexpr CustomStructConverter(C&&... c)
      : converter{std::make_tuple(c...)} {};
  template <class MESSAGE>
  constexpr static std::optional<size_t> converterIndex(const char* name) {
    return details::CustomConverterHelper<
        CustomStructConverter>::template converterIndex<MESSAGE>(name);
  }
  template <class MESSAGE>
  constexpr static bool hasConverterWithMessageTypeAndAttributeName(
      const char* name) {
    return converterIndex<MESSAGE>(name) != std::nullopt;
  }
};

template <>
struct CustomStructConverter<> {
  constexpr CustomStructConverter() = default;
  template <class MESSAGE>
  constexpr static std::optional<size_t> converterIndex(const char*) {
    return std::nullopt;
  }
  template <class MESSAGE>
  constexpr static bool hasConverterWithMessageTypeAndAttributeName(
      const char*) {
    return false;
  }
};

template <class FROM_VALUE_TYPE, class TO_VALUE_TYPE>
constexpr bool is_raw_scalar_convertible() {
  if constexpr (std::is_same_v<FROM_VALUE_TYPE, TO_VALUE_TYPE>) {
    return true;
  } else if constexpr (meta::is_floating_point_v<FROM_VALUE_TYPE>) {
    return meta::is_floating_point_v<TO_VALUE_TYPE>;
  } else if constexpr (meta::is_floating_point_v<TO_VALUE_TYPE>) {
    return true;  // allow int->double/float
  } else if constexpr (meta::is_signed_v<FROM_VALUE_TYPE>) {
    return meta::is_signed_v<TO_VALUE_TYPE> &&
           (meta::size_of<TO_VALUE_TYPE> >= meta::size_of<FROM_VALUE_TYPE>);
  } else if constexpr (!meta::is_signed_v<FROM_VALUE_TYPE> &&
                       meta::is_signed_v<TO_VALUE_TYPE>) {
    return meta::size_of<TO_VALUE_TYPE> > meta::size_of<FROM_VALUE_TYPE>;
  } else {
    return (meta::is_signed_v<FROM_VALUE_TYPE> ==
            meta::is_signed_v<TO_VALUE_TYPE>)&&(meta::size_of<TO_VALUE_TYPE> >=
                                                meta::size_of<FROM_VALUE_TYPE>);
  }
}

template <class OTHER_MESSAGE, class MESSAGE,
          class CONVERTER = CustomStructConverter<>>
void convert_from_other_message(
    const OTHER_MESSAGE& o, MESSAGE& m,
    CONVERTER converter = CustomStructConverter<>{});

template <class META_FROM, class META_TO, class FROM, class TO>
void convert_enums(const FROM& a, TO& b) {
  using META_A =
      meta::meta_of_t<meta::value_type_t<typename META_FROM::ATTR_TYPE>>;
  using META_B =
      meta::meta_of_t<meta::value_type_t<typename META_TO::ATTR_TYPE>>;
  b = META_B::string2enum(META_A::enum2string(a));
}

template <class FROM, class TO>
void convert_strings(const FROM& a, TO& b) {
  std::string copy = a;
  b = copy;
}

template <class META_TO, class FROM_VALUE_TYPE, class TO_VALUE_TYPE>
void adjust_and_check_array_size(const FROM_VALUE_TYPE& from,
                                 TO_VALUE_TYPE& to) {
  if constexpr (dynamic_array_attribute_meta<META_TO>) {
    to.resize(from.size());
  }
  if (from.size() != to.size()) {
    throw std::runtime_error(std::string("array sizes do not match: ") +
                             META_TO::name());
  }
}

template <class META_FROM, class META_TO, class FROM_VALUE_TYPE,
          class TO_VALUE_TYPE, class CONVERTER>
void convert_existing_values(const FROM_VALUE_TYPE& from, TO_VALUE_TYPE& to,
                             const CONVERTER& converter) {
  // -----------------------
  // if (attribute represents a struct) {
  //    call convert recursively (for arrays/non arrays)
  // }
  if constexpr (struct_attribute_meta<META_TO>) {
    // converting structs (recursively)
    static_assert(struct_attribute_meta<META_FROM>, "unexpected");
    static_assert(
        array_attribute_meta<META_TO> == array_attribute_meta<META_FROM>,
        "unexpected");
    if constexpr (array_attribute_meta<META_TO>) {
      adjust_and_check_array_size<META_TO>(from, to);
      for (size_t i = 0; i < to.size(); i++) {
        convert_from_other_message(from[i], to[i], converter);
      }
    } else {
      convert_from_other_message(from, to, converter);
    }
  } else {
    // converting non structs (enums, strings, values, ...)

    // -----------------------
    // if (attributes represents different enums) {
    //    convert enums
    // }
    if constexpr (!std::is_same_v<typename META_FROM::ATTR_TYPE,
                                  typename META_TO::ATTR_TYPE> &&
                  meta::enum_attribute_meta<META_FROM>) {
      // different enums
      static_assert(meta::enum_attribute_meta<META_TO>,
                    "unexpected, can only convert enum to enum");
      if constexpr (array_attribute_meta<META_TO>) {
        adjust_and_check_array_size<META_TO>(from, to);
        for (size_t i = 0; i < to.size(); i++) {
          convert_enums<META_FROM, META_TO>(from[i], to[i]);
        }
      } else {
        convert_enums<META_FROM, META_TO>(from, to);
      }
    }
    // -----------------------
    // if (attributes represent different strings) {
    //    convert strings
    // }
    else if constexpr (!std::is_same_v<typename META_FROM::ATTR_TYPE,
                                       typename META_TO::ATTR_TYPE> &&
                       meta::string_attribute_meta<META_FROM>) {
      // different strings
      static_assert(meta::string_attribute_meta<META_TO>,
                    "unexpected, can only convert strings to strings");
      if constexpr (array_attribute_meta<META_TO>) {
        adjust_and_check_array_size<META_TO>(from, to);
        for (size_t i = 0; i < to.size(); i++) {
          convert_strings(from[i], to[i]);
        }
      } else {
        convert_strings(from, to);
      }
    }
    // -----------------------
    // else attributes represent same type or different raw types
    else {  // normal "raw" values or equal types
      // unpack any arrays, optionals and check type
      static_assert(meta::is_raw_scalar_convertible<
                        meta::value_type_t<typename META_FROM::ATTR_TYPE>,
                        meta::value_type_t<typename META_TO::ATTR_TYPE>>,
                    "type compatibility check");
      if constexpr (array_attribute_meta<META_TO>) {
        adjust_and_check_array_size<META_TO>(from, to);
        for (size_t i = 0; i < to.size(); i++) {
          to[i] = from[i];
        }
      } else {
        to = from;
      }
    }
  }
}

template <class OTHER_MESSAGE, class CONVERTER>
struct FromOtherMessageVisitor {
  const OTHER_MESSAGE& other_message;
  const CONVERTER converter;
  using OTHER_STRUCT_META = meta::meta_of_t<OTHER_MESSAGE>;

  template <class META, class T>
  void visit(T& message) {
    // -------------------------------------
    // if(manual convert function present) {
    //   to = manual_convert function
    // }
    if constexpr (CONVERTER::
                      template hasConverterWithMessageTypeAndAttributeName<
                          typename META::STRUCT>(META::name())) {
      constexpr auto idx =
          CONVERTER::template converterIndex<typename META::STRUCT>(
              META::name());
      std::get<idx.value()>(converter.converter)
          .convertFunction(other_message, META::get(message));
    } else {
      // -------------------------------------
      // else if(dest is optional and attribute is not present in src) {
      //   to = None
      // }
      if constexpr (optional_attribute_meta<META> &&
                    !has_meta_by_name_v<META::ct_name(), OTHER_MESSAGE>) {
        meta::clear_optional(META::get(message));
      } else {
        // -------------------------------------
        // else attribute is present in dest and src) {...}
        static_assert(has_meta_by_name_v<META::ct_name(), OTHER_MESSAGE>,
                      "unexpected, scalar attribute not found");
        using OTHER_META =
            meta::meta_by_name_t<meta::details::String(META::ct_name()),
                                 OTHER_MESSAGE>;

        // -------------------------------------
        // if(attribute is optional in dest and src) {
        //   to = convert unpacked or set to None if value not present in src
        // }
        if constexpr (optional_attribute_meta<META> &&
                      optional_attribute_meta<OTHER_META>) {
          // "optional = optional"
          if (meta::has_value(OTHER_META::get(other_message))) {
            meta::construct_optional(META::get(message));
            decltype(auto) to = META::get(message);
            convert_existing_values<OTHER_META, META>(
                meta::unpack_optional(OTHER_META::get(other_message)),
                meta::unpack_optional(to));
          } else {
            clear_optional(META::get(message));
          }
        }
        // -------------------------------------
        // else if(attribute is optional in dest but not in src) {
        //   to = convert unpacked
        // }
        else if constexpr (optional_attribute_meta<META> &&
                           !optional_attribute_meta<OTHER_META>) {
          // "optional = non optional"
          meta::construct_optional(META::get(message));
          decltype(auto) to = META::get(message);
          convert_existing_values<OTHER_META, META>(
              OTHER_META::get(other_message), meta::unpack_optional(to),
              converter);
        }
        // -------------------------------------
        // else if(attribute is non optional in dest and src) {
        //   to = from
        // }
        else {
          // "non optional = non optional"
          static_assert(!optional_attribute_meta<OTHER_META>,
                        "unexpected, message attr non optional and other "
                        "message attribute optional");
          decltype(auto) to = META::get(message);
          convert_existing_values<OTHER_META, META>(
              OTHER_META::get(other_message), to, converter);
        }
      }
    }
  }
};

template <class OTHER_MESSAGE, class MESSAGE, class CONVERTER>
void convert_from_other_message(const OTHER_MESSAGE& o, MESSAGE& m,
                                CONVERTER converter) {
  meta::accept(FromOtherMessageVisitor<OTHER_MESSAGE, CONVERTER>{o, converter},
               m);
}

}  // namespace meta
