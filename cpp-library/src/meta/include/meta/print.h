/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#pragma once
#include <iomanip>
#include <iostream>
#include <ostream>

#include "meta/meta.h"
#include "meta/type_traits.h"

namespace meta {

template <class MESSAGE>
void print(const MESSAGE& m, std::ostream& out = std::cout, size_t indent = 0);

struct PrintVisitor {
  std::ostream& out;
  size_t indent;
  template <class META, class T>
  void visitScalarString(const T& value) {
    out << std::string(indent, ' ') << META::name() << "=" << std::quoted(value)
        << "\n";
  }
  template <class META, class T>
  void visitScalarEnum(const T& value) {
    visitScalarRawType<META>(value);
  }
  template <class META, class T>
  void visitScalarStruct(const T& value) {
    out << std::string(indent, ' ') << META::name() << "="
        << "\n";
    print(value, out, indent + 2);
  }
  template <class META, class T>
  void visitScalarRawType(const T& value) {
    out << std::string(indent, ' ') << META::name() << "="
        << static_cast<meta::value_type_t<typename META::ATTR_TYPE>>(value)
        << "\n";
  }
  template <class META, class T>
  void visitArrayString(const T& value) {
    out << std::string(indent, ' ') << META::name() << "=";
    out << "[";
    for (auto& x : value) {
      out << " " << std::quoted(x);
    }
    out << " ]"
        << "\n";
  }
  template <class META, class T>
  void visitArrayEnum(const T& value) {
    visitArrayRawType<META>(value);
  }
  template <class META, class T>
  void visitArrayStruct(const T& value) {
    out << std::string(indent, ' ') << META::name() << "=";
    out << "["
        << "\n";
    for (auto& x : value) {
      print(x, out, indent + 2);
    }
    out << std::string(indent, ' ') << "]"
        << "\n";
  }
  template <class META, class T>
  void visitArrayRawType(const T& value) {
    out << std::string(indent, ' ') << META::name() << "=";
    out << "[";
    for (size_t i = 0; i < value.size(); i++) {
      decltype(auto) x = value[i];
      if constexpr (meta::optional_attribute_meta<META>) {
        out << " "
            << static_cast<meta::value_type_t<
                   meta::value_type_t<typename META::ATTR_TYPE>>>(x);
      } else {
        out << " "
            << static_cast<meta::value_type_t<typename META::ATTR_TYPE>>(x);
      }
    }
    out << " ]"
        << "\n";
  }
};
template <class MESSAGE>
void print(const MESSAGE& m, std::ostream& out, size_t indent) {
  out << std::string(indent, ' ') << meta::meta_of_t<MESSAGE>::name() << ":\n";
  meta::accept(meta::BasicVisitor<PrintVisitor, meta::Unpack::Yes>{PrintVisitor{
                   out, indent + 2}},
               m);
}

}  // namespace meta
