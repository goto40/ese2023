/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#ifndef META_PROGRAM_OPTIONS_H
#define META_PROGRAM_OPTIONS_H

#include <boost/program_options.hpp>
#include <boost/program_options/option.hpp>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>

#include "meta/meta.h"
#include "meta/type_traits.h"

namespace meta {
namespace {

namespace po = boost::program_options;

template <class STRUCT>
struct ProgramOptionsVisitorPreparator {
  po::options_description& desc;
  template <class META>
  void visit() {
    if (meta::struct_attribute_meta<META>) {
      throw new std::runtime_error{
          "no substructs allowed for commandline objects"};
    }
    if (meta::array_attribute_meta<META>) {
      desc.add_options()(META::name(), po::value<std::vector<std::string>>(),
                         META::description());
    } else {
      desc.add_options()(META::name(), po::value<std::string>(),
                         META::description());
    }
  }
};

template <class STRUCT>
struct ProgramOptionsVisitorFiller {
  po::variables_map& vm;
  template <class META>
  void visit(STRUCT& s) {
    decltype(auto) value = META::get(s);
    using T = std::remove_reference_t<decltype(value)>;
    if (vm.count(META::name())) {
      if constexpr (meta::array_attribute_meta<META>) {
        std::vector<std::string> text =
            vm.at(META::name()).template as<std::vector<std::string>>();
        if constexpr (meta::string_attribute_meta<META>) {
          if constexpr (meta::optional_attribute_meta<META>) {
            meta::construct_optional(value);
            meta::unpack_optional(value).resize(text.size());
            for (size_t i = 0; i < text.size(); i++) {
              meta::unpack_optional(value)[i] = text[i];
            }
          } else {
            value.resize(text.size());
            for (size_t i = 0; i < text.size(); i++) {
              value[i] = text[i];
            }
          }
        } else if constexpr (meta::enum_attribute_meta<META>) {
          if constexpr (meta::optional_attribute_meta<META>) {
            using ENUM_META =
                meta::meta_of_t<meta::value_type_t<meta::value_type_t<T>>>;
            meta::construct_optional(value);
            meta::unpack_optional(value).resize(text.size());
            for (size_t i = 0; i < text.size(); i++) {
              meta::unpack_optional(value)[i] =
                  ENUM_META::string2enum(text[i].c_str());
            }
          } else {
            using ENUM_META = meta::value_type_t<meta::value_type_t<T>>;
            value.resize(text.size());
            for (size_t i = 0; i < text.size(); i++) {
              value[i] = ENUM_META::string2enum(text[i].c_str());
            }
          }
        } else if constexpr (meta::struct_attribute_meta<META>) {
          throw std::runtime_error("unexpected");
        } else {  // raw
          if constexpr (meta::optional_attribute_meta<META>) {
            meta::construct_optional(value);
            meta::unpack_optional(value).resize(text.size());
            for (size_t i = 0; i < text.size(); i++) {
              std::stringstream s{text[i].c_str()};
              if (sizeof(meta::value_type_t<meta::value_type_t<T>>) == 1) {
                int x;
                s >> x;
                meta::unpack_optional(value)[i] = x;
              } else {
                meta::value_type_t<meta::value_type_t<T>> x;
                s >> x;
                meta::unpack_optional(value)[i] = x;
              }
            }
          } else {
            value.resize(text.size());
            for (size_t i = 0; i < text.size(); i++) {
              std::stringstream s{text[i].c_str()};
              if (sizeof(meta::value_type_t<T>) == 1) {
                int x;
                s >> x;
                // TODO check range.
                value[i] = x;
              } else {
                meta::value_type_t<typename META::ATTR_TYPE> x;
                s >> x;
                value[i] = x;
              }
            }
          }
        }
      } else {  // scalar
        std::string text = vm.at(META::name()).template as<std::string>();
        if constexpr (meta::string_attribute_meta<META>) {
          value = text;
        } else if constexpr (meta::enum_attribute_meta<META>) {
          using ENUM_META = meta::meta_of_t<T>;
          value = ENUM_META::string2enum(text.c_str());
        } else if constexpr (meta::struct_attribute_meta<META>) {
          throw std::runtime_error("unexpected");
        } else {  // raw
          std::stringstream stream{text};
          if (meta::size_of<T> == 1) {
            int i;
            stream >> i;
            value = i;

          } else {
            if constexpr (meta::optional_attribute_meta<META>) {
              meta::construct_optional(value);
              meta::value_type_t<typename META::ATTR_TYPE> x;
              stream >> x;
              value = x;
            } else {
              meta::value_type_t<typename META::ATTR_TYPE> x;
              stream >> x;
              value = x;
            }
          }
          // TODO check range.
        }
      }
    } else {  // not present
      if constexpr (meta::optional_attribute_meta<META>) {
        value = std::nullopt;
      } else {
        throw std::runtime_error{
            std::string("unexpected: missing required option ") + META::name()};
      }
    }
  }
};

}  // namespace

template <class MESSAGE>
MESSAGE from_program_options(int argc, const char** argv) {
  namespace po = boost::program_options;
  MESSAGE data;
  po::options_description desc{meta::meta_of_t<MESSAGE>::description()};
  desc.add_options()("help", "display help message");
  ProgramOptionsVisitorPreparator<MESSAGE> preparator{desc};
  meta::accept<MESSAGE>(preparator);

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    std::exit(0);
  }

  ProgramOptionsVisitorFiller<MESSAGE> filler{vm};
  meta::accept(filler, data);

  return data;
}

}  // namespace meta

#endif
