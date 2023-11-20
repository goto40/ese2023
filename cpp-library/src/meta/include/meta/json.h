/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#pragma once

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "meta/meta.h"
#include "meta/type_traits.h"

namespace meta {
template <class MESSAGE>
void from_json(const nlohmann::json& j, MESSAGE& m);

struct FromJsonVisitor {
  const nlohmann::json& j;
  template <class META, class T>
  void visitScalarString(T& value) {
    if (!required_attribute_meta<META> && !j.contains(META::name())) return;
    remove_optional_t<typename META::ATTR_TYPE> v;
    j.at(META::name()).get_to(v);
    value = v;
  }
  template <class META, class T>
  void visitScalarEnum(T& value) {
    if (!required_attribute_meta<META> && !j.contains(META::name())) return;
    std::string enumAsText;
    j.at(META::name()).get_to(enumAsText);
    value = meta::meta_of_t<remove_optional_t<typename META::ATTR_TYPE>>::
        string2enum(enumAsText.c_str());
  }
  template <class META, class T>
  void visitScalarStruct(T& value) {
    if (!required_attribute_meta<META> && !j.contains(META::name())) return;
    auto& inner = j.at(META::name());
    remove_optional_t<typename META::ATTR_TYPE> v;
    from_json(inner, v);
    value = v;
  }
  template <class META, class T>
  void visitScalarRawType(T& value) {
    if (!required_attribute_meta<META> && !j.contains(META::name())) return;
    remove_optional_t<typename META::ATTR_TYPE> v;
    j.at(META::name()).get_to(v);
    value = v;
  }
  template <class META, class T>
  void visitArrayString(T& value) {
    if (!required_attribute_meta<META> && !j.contains(META::name())) return;
    visitArray<META>(value, [&](auto& array, size_t idx) {
      auto& s = array[idx];
      remove_optional_t<typename META::ATTR_TYPE> v;
      s.get_to(v);
      value[idx] = v;
    });
  }
  template <class META, class T>
  void visitArrayEnum(T& value) {
    if (!required_attribute_meta<META> && !j.contains(META::name())) return;
    visitArray<META>(value, [&](auto& array, size_t idx) {
      auto& e = array[idx];
      std::string enumAsText;
      e.get_to(enumAsText.c_str());
      if (idx >= value.size())
        throw std::runtime_error(
            std::string("unexpected: index out of range for field ") +
            META::name());
      value[idx] = meta::meta_of_t<
          remove_optional_t<typename META::ATTR_TYPE>>::string2enum(enumAsText);
    });
  }
  template <class META, class T>
  void visitArrayStruct(T& value) {
    if (!required_attribute_meta<META> && !j.contains(META::name())) return;
    visitArray<META>(value, [&](auto& array, size_t idx) {
      auto& e = array[idx];
      if (idx >= value.size())
        throw std::runtime_error(
            std::string("unexpected: index out of range for field ") +
            META::name());
      if constexpr (required_attribute_meta<META>) {
        from_json(e, value[idx]);
      } else {
        value[idx] = {};
        from_json(e, *value[idx]);
      }
    });
  }
  template <class META, class T>
  void visitArrayRawType(T& value) {
    if (!required_attribute_meta<META> && !j.contains(META::name())) return;
    visitArray<META>(value, [&](auto& array, size_t idx) {
      auto& e = array[idx];
      if (idx >= value.size())
        throw std::runtime_error(
            std::string("unexpected: index out of range for field ") +
            META::name());
      remove_optional_t<typename META::ATTR_TYPE> v;
      e.get_to(v);
      value[idx] = v;
    });
  }

  template <class META, class T, class F>
  void visitArray(T& value, F f) {
    auto& array = j.at(META::name());
    if (!array.is_array())
      throw std::runtime_error(
          std::string("unexpected: array required for field ") + META::name());
    if constexpr (fixed_sized_array_attribute_meta<META>) {
      if (array.size() != value.size())
        throw std::runtime_error(
            std::string("unexpected: array size not ok for field ") +
            META::name());
    } else {
      value.resize(array.size());
    }
    for (size_t idx = 0; idx < array.size(); idx++) {
      f(array, idx);
    }
  }
};
template <class MESSAGE>
void from_json(const nlohmann::json& j, MESSAGE& m) {
  meta::accept(meta::BasicVisitor<FromJsonVisitor>{FromJsonVisitor{j}}, m);
}

template <class MESSAGE>
auto from_json(std::string json_text) {
  const nlohmann::json& j = nlohmann::json::parse(json_text);
  MESSAGE m;
  from_json(j, m);
  return m;
}

template <class MESSAGE>
auto from_json(std::string json_text, MESSAGE& m) {
  const nlohmann::json& j = nlohmann::json::parse(json_text);
  from_json(j, m);
  return m;
}

// --------------------------------------------------

template <class MESSAGE>
void to_json(const MESSAGE& m, nlohmann::json& j);

struct ToJsonVisitor {
  nlohmann::json& j;
  template <class META, class T>
  void visitScalarString(const T& value) {
    j[META::name()] = value;
  }
  template <class META, class T>
  void visitScalarEnum(const T& value) {
    j[META::name()] = meta::meta_of_t<
        remove_optional_t<typename META::ATTR_TYPE>>::enum2string(value);
  }
  template <class META, class T>
  void visitScalarStruct(const T& value) {
    j[META::name()] = nlohmann::json{};
    to_json(value, j[META::name()]);
  }
  template <class META, class T>
  void visitScalarRawType(const T& value) {
    j[META::name()] =
        static_cast<meta::value_type_t<typename META::ATTR_TYPE>>(value);
  }
  template <class META, class T>
  void visitArrayString(const T& value) {
    auto a = nlohmann::json::array();
    for (auto& x : value) a.push_back(x);
    j[META::name()] = a;
  }
  template <class META, class T>
  void visitArrayEnum(const T& value) {
    auto a = nlohmann::json::array();
    for (auto& x : value)
      a.push_back(
          meta::meta_of_t<meta::value_type_t<
              remove_optional_t<typename META::ATTR_TYPE>>>::enum2string(x));
    j[META::name()] = a;
  }
  template <class META, class T>
  void visitArrayStruct(const T& value) {
    auto a = nlohmann::json::array();
    for (auto& x : value) a.push_back(nlohmann::json{});
    for (size_t idx = 0; idx < value.size(); idx++)
      to_json(value[idx], a.at(idx));
    j[META::name()] = a;
  }
  template <class META, class T>
  void visitArrayRawType(const T& value) {
    auto a = nlohmann::json::array();
    for (auto& x : value) a.push_back(x);
    j[META::name()] = a;
  }
};
template <class MESSAGE>
void to_json(const MESSAGE& m, nlohmann::json& j) {
  meta::accept(meta::BasicVisitor<ToJsonVisitor, Unpack::Yes>{ToJsonVisitor{j}},
               m);
}

template <class MESSAGE>
auto to_json_obj(const MESSAGE& m) {
  nlohmann::json j;
  to_json(m, j);
  return j;
}

template <class MESSAGE>
auto to_json(const MESSAGE& m) -> std::string {
  return to_json_obj(m).dump();
}

}  // namespace meta
