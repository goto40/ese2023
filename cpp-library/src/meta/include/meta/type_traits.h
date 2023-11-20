/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#pragma once
#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace meta {

template <class T>
struct meta_of;

template <class T>
using meta_of_t = meta_of<T>::type;

// mapping of standard type_traits, in order to remap them for custom value
// types
template <class T>
constexpr bool is_enum_v = std::is_enum_v<T>;

template <class T>
constexpr bool is_floating_point_v = std::is_floating_point_v<T>;

template <class T>
constexpr bool is_signed_v = std::is_signed_v<T>;

template <class T>
constexpr size_t size_of = sizeof(T);

template <class>
struct is_dynamic_array_type : std::false_type {};
template <class A>
struct is_dynamic_array_type<std::vector<A>> : std::true_type {
};  // specialize your own arrays this way
template <class T>
constexpr bool is_dynamic_array_type_v = is_dynamic_array_type<T>::value;
template <class T>
concept dynamic_array_type = is_dynamic_array_type_v<T>;

template <class>
struct is_fixed_sized_array_type : std::false_type {};
template <class A, size_t N>
struct is_fixed_sized_array_type<std::array<A, N>> : std::true_type {};
template <class T>
constexpr bool is_fixed_sized_array_type_v =
    is_fixed_sized_array_type<T>::value;
template <class T>
concept fixed_sized_array_type = is_fixed_sized_array_type_v<T>;

template <class T>
concept array_type = dynamic_array_type<T> || fixed_sized_array_type<T>;

template <class, class>
struct is_std_array_of_type : std::false_type {};
template <class A, size_t N>
struct is_std_array_of_type<std::array<A, N>, A> : std::true_type {};
template <class T, class A>
constexpr bool is_std_array_of_type_v = is_std_array_of_type<T, A>::value;
template <class T, class A>
concept std_array_of_type =
    is_std_array_of_type_v<std::remove_cv_t<std::remove_reference_t<T>>, A>;

template <class T>
struct remove_optional {
  using type = T;
};
template <class T>
struct remove_optional<std::optional<T>> {
  using type = T;
};
template <class T>
using remove_optional_t = remove_optional<T>::type;

template <class>
struct is_optional_type : std::false_type {};
template <class A>
struct is_optional_type<std::optional<A>> : std::true_type {
};  // specialize your own arrays this way
template <class T>
constexpr bool is_optional_type_v = is_optional_type<T>::value;
template <class T>
concept optional_type = is_optional_type_v<T>;
template <class T>
concept non_optional_type = !optional_type<T>;

template <class T>
concept has_meta = requires() { typename meta_of_t<T>; };

/** Interface of a reflection struct (META) describing an attribute of a struct
 * (e.g. an attribute `Point::x`)
 *
 * The META of the attribute allows
 * - accessing the attribute value form a given struct instance
 * - accessing the attribute properties (type, name and meta information)
 */
template <class META>
concept attribute_meta =
    requires(typename META::STRUCT &mut_s, const typename META::STRUCT &const_s,
             typename META::ATTR_TYPE attrValue) {
      typename META::STRUCT;  /// the owner

      typename META::ATTR_TYPE;  /// the content type

      //(TODO: use case for vectors?)
      { META::get(mut_s) = attrValue };    /// mutable access
      { attrValue = META::get(const_s) };  /// const access
      { attrValue = META::get(mut_s) };    /// mutable access

      // the attribute itself...
      { META::name() } -> std::convertible_to<std::string>;  /// the name
      {
        META::ct_name()
      } -> std_array_of_type<const char>;  /// the name, usable as compile
                                           /// time template parameter
    };

template <class T>
concept scalar_attribute_meta =
    attribute_meta<T> && !array_type<remove_optional_t<typename T::ATTR_TYPE>>;
template <class T>
concept array_attribute_meta =
    attribute_meta<T> && array_type<remove_optional_t<typename T::ATTR_TYPE>>;
template <class T>
concept dynamic_array_attribute_meta =
    attribute_meta<T> &&
    dynamic_array_type<remove_optional_t<typename T::ATTR_TYPE>>;
template <class T>
concept fixed_sized_array_attribute_meta =
    attribute_meta<T> &&
    fixed_sized_array_type<remove_optional_t<typename T::ATTR_TYPE>>;
template <class T>
concept optional_attribute_meta =
    attribute_meta<T> && optional_type<typename T::ATTR_TYPE>;
template <class T>
concept required_attribute_meta =
    attribute_meta<T> && non_optional_type<typename T::ATTR_TYPE>;

template <class T, bool has_inner_type = array_type<T> || is_optional_type_v<T>>
struct value_type {
  using type = T;
};
template <class T>
struct value_type<T, true> {
  using type = typename T::value_type;
};
template <class T>
using value_type_t = value_type<T>::type;

template <class T>
concept enum_attribute_meta =
    attribute_meta<T> && meta::is_enum_v<value_type_t<typename T::ATTR_TYPE>>;
template <class T>
concept string_attribute_meta =
    attribute_meta<T> &&
    std::is_convertible_v<value_type_t<typename T::ATTR_TYPE>,
                          std::string_view>;
template <class T>
concept struct_attribute_meta =
    attribute_meta<T> && has_meta<value_type_t<typename T::ATTR_TYPE>> &&
    !meta::enum_attribute_meta<T>;

template <class T>
concept raw_attribute_meta = attribute_meta<T> && (!enum_attribute_meta<T> &&
                                                   !struct_attribute_meta<T> &&
                                                   !string_attribute_meta<T>);

template <class T>
struct is_attribute_meta : public std::bool_constant<attribute_meta<T>> {};

template <class T>
inline constexpr bool is_attribute_meta_v = is_attribute_meta<T>::value;

template <class List>
struct are_attributes_meta : std::false_type {};

template <template <class...> class List, class... T>
struct are_attributes_meta<List<T...>>
    : std::conjunction<is_attribute_meta<T>...> {};

template <class T>
inline constexpr bool are_attributes_meta_v = are_attributes_meta<T>::value;

/** Interface of a reflection struct (META) describing a struct
 *
 * The META of the struct allows
 * - accessing the struct name (e.g. `Point`)
 * - accessing the list of common reflection information for all attributes
 * (META of attributes for, e.g., `Point::x` and `Point::y`)
 */
template <class META>
concept struct_meta = requires() {
  { META::name() } -> std::convertible_to<std::string>;  /// the name
  requires are_attributes_meta_v<typename META::ListOfAttributeMetaClasses>;
};

}  // namespace meta
