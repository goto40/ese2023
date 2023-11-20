/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#pragma once
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "meta.h"
#include "meta/json.h"
#include "meta/type_traits.h"

namespace meta {
struct UntypedAttribute;

struct UntypedStruct {
  virtual size_t size() = 0;
  virtual std::shared_ptr<UntypedAttribute> at(size_t index) = 0;
  virtual std::shared_ptr<const UntypedAttribute> at(size_t index) const = 0;
  virtual std::shared_ptr<UntypedAttribute> at(const std::string_view name) = 0;
  virtual std::shared_ptr<const UntypedAttribute> at(
      const std::string_view name) const = 0;
};

struct UntypedAttribute {
  virtual std::string name() const = 0;
  virtual bool is_array() const = 0;
  virtual bool is_struct() const = 0;
  virtual size_t size() const = 0;
  virtual void resize(size_t s) = 0;
  virtual std::string to_string(size_t index = 0) const = 0;
  virtual void from_string(std::string_view text, size_t index = 0) = 0;
  virtual std::shared_ptr<UntypedStruct> get_struct(size_t index = 0) = 0;
  virtual std::shared_ptr<const UntypedStruct> get_struct(
      size_t index = 0) const = 0;
};

template <class META_STRUCT, class META>
struct TypedAttributeBaseImpl : UntypedAttribute {
  std::weak_ptr<META_STRUCT> owner;
  TypedAttributeBaseImpl(std::weak_ptr<META_STRUCT> owner) : owner{owner} {}
  std::shared_ptr<const META_STRUCT> get_owner() const {
    auto ret = owner.lock();
    if (ret == nullptr) throw std::runtime_error("owner not existing any more");
    return ret;
  }
  std::shared_ptr<META_STRUCT> get_owner() {
    auto ret = owner.lock();
    if (ret == nullptr) throw std::runtime_error("owner not existing any more");
    return ret;
  }
  std::string name() const override { return META::name(); }
  /** default impl */
  bool is_struct() const override { return false; };
  /** default impl */
  std::shared_ptr<UntypedStruct> get_struct(size_t index) override {
    throw std::runtime_error{"no struct"};
  }
  /** default impl */
  std::shared_ptr<const UntypedStruct> get_struct(size_t index) const override {
    throw std::runtime_error{"no struct"};
  }

  bool is_array() const override { return meta::array_attribute_meta<META>; };
  size_t size() const override {
    if constexpr (meta::array_attribute_meta<META>) {
      return META::get(this->get_owner()->getStructRef()).size();

    } else {
      return 1;
    }
  };
  void resize(size_t s) override {
    if constexpr (meta::array_attribute_meta<META>) {
      META::get(this->get_owner()->getStructRef()).resize(s);

    } else {
      throw new std::runtime_error("no resize for scalars.");
    }
  }

  decltype(auto) get_value(size_t index = 0) const {
    if constexpr (meta::array_attribute_meta<META>) {
      return META::get(this->get_owner()->getStructRef()).at(index);
    } else {
      if (index != 0) throw new std::runtime_error("no index>0 for scalars.");
      return META::get(this->get_owner()->getStructRef());
    }
  }
  decltype(auto) get_value(size_t index = 0) {
    if constexpr (meta::array_attribute_meta<META>) {
      return META::get(this->get_owner()->getStructRef()).at(index);
    } else {
      if (index != 0) throw new std::runtime_error("no index>0 for scalars.");
      return META::get(this->get_owner()->getStructRef());
    }
  }
};

namespace details {

template <class META_STRUCT>
struct StructInitVisitor {
  std::vector<std::shared_ptr<UntypedAttribute>>& attributes;
  std::weak_ptr<META_STRUCT> parent;

  StructInitVisitor(std::weak_ptr<META_STRUCT> parent,
                    std::vector<std::shared_ptr<UntypedAttribute>>& attributes)
      : parent{parent}, attributes{attributes} {}
  template <class META>
  void visitScalarString();
  template <class META>
  void visitScalarEnum();
  template <class META>
  void visitScalarStruct();
  template <class META>
  void visitScalarRawType();
  template <class META>
  void visitArrayString();
  template <class META>
  void visitArrayEnum();
  template <class META>
  void visitArrayStruct();
  template <class META>
  void visitArrayRawType();
};

}  // namespace details

template <class STRUCT>
class AbstractStruct
    : public UntypedStruct,
      public std::enable_shared_from_this<AbstractStruct<STRUCT>> {
  std::vector<std::shared_ptr<UntypedAttribute>> attributes;
  std::unordered_map<std::string, std::shared_ptr<UntypedAttribute>>
      attribute_map;

 protected:
  /** must be called by final implementations providing getStruct() */
  void init(std::weak_ptr<AbstractStruct<STRUCT>> parent) {
    meta::accept<std::remove_cvref_t<STRUCT>>(
        meta::BasicVisitor<details::StructInitVisitor<AbstractStruct>,
                           meta::Unpack::Yes>{
            details::StructInitVisitor<AbstractStruct>{parent, attributes}});
    for (auto a : attributes) {
      attribute_map[a->name()] = a;
    }
  }

 public:
  using value_type = STRUCT;
  virtual STRUCT& getStructRef() = 0;
  virtual const STRUCT& getStructRef() const = 0;
  size_t size() { return attributes.size(); }
  std::shared_ptr<UntypedAttribute> operator[](size_t index) {
    return attributes[index];
  }
  std::shared_ptr<const UntypedAttribute> operator[](size_t index) const {
    return attributes[index];
  }
  std::shared_ptr<UntypedAttribute> at(size_t index) override {
    return attributes.at(index);
  }
  std::shared_ptr<const UntypedAttribute> at(size_t index) const override {
    return attributes.at(index);
  }
  std::shared_ptr<const UntypedAttribute> at(
      const std::string_view key) const override {
    return attribute_map.at(std::string{key});
  }
  std::shared_ptr<UntypedAttribute> at(const std::string_view key) override {
    return attribute_map.at(std::string{key});
  }
  /** immutable iterator with an immutable pointer pointing to a mutable
   * content. */
  auto begin() { return attributes.cbegin(); }
  auto end() { return attributes.cend(); }
};

template <class STRUCT>
class Struct : public AbstractStruct<STRUCT> {
  STRUCT data;
  Struct() = default;
  static_assert(!std::is_const_v<STRUCT>, "non const payload expected");

 public:
  static auto create() {
    auto ret = std::shared_ptr<Struct>(new Struct{});
    ret->init(ret->shared_from_this());
    return ret;
  }
  STRUCT& getStructRef() override { return data; };
  const STRUCT& getStructRef() const { return data; };
};

template <class STRUCT>
class StructPtr : public AbstractStruct<STRUCT> {
  std::shared_ptr<STRUCT> data;
  StructPtr(std::shared_ptr<STRUCT> data) : data{data} {}
  static_assert(!std::is_const_v<STRUCT>, "non const payload expected");

 public:
  static auto create(std::shared_ptr<STRUCT> owning_data) {
    auto ret = std::shared_ptr<StructPtr>(new StructPtr{owning_data});
    ret->init(ret->shared_from_this());
    return ret;
  }
  STRUCT& getStructRef() override { return *data; };
  const STRUCT& getStructRef() const { return *data; };
};

template <class STRUCT, class OWNER>
class StructReference : public AbstractStruct<STRUCT> {
  std::shared_ptr<OWNER> owner;
  STRUCT& data;
  StructReference(STRUCT& data, std::shared_ptr<OWNER> owner)
      : data{data}, owner{owner} {}
  static_assert(!std::is_const_v<STRUCT>, "non const payload expected");

 public:
  STRUCT& getStructRef() override { return data; };
  const STRUCT& getStructRef() const { return data; };

  static auto create(STRUCT& data, std::shared_ptr<OWNER> owner) {
    if constexpr (std::is_same_v<OWNER, std::nullptr_t>) {
      auto ret = std::shared_ptr<StructReference<STRUCT, OWNER>>(
          new StructReference<STRUCT, OWNER>{data});
      ret->init(ret->shared_from_this());
      return ret;
    } else {
      auto ret = std::shared_ptr<StructReference<STRUCT, OWNER>>(
          new StructReference<STRUCT, OWNER>{data, owner});
      ret->init(ret->shared_from_this());
      return ret;
    }
  }
};

template <class STRUCT, class OWNER>
class StructConstReference : public AbstractStruct<STRUCT> {
  std::shared_ptr<OWNER> owner;
  const STRUCT& data;
  StructConstReference(const STRUCT& data, std::shared_ptr<OWNER> owner)
      : data{data}, owner{owner} {}
  static_assert(!std::is_const_v<STRUCT>,
                "non const payload type expected (is made const inside!)");

 public:
  STRUCT& getStructRef() override {
    throw std::runtime_error{
        "unexpceted. non const access in const obj (impossible)"};
  };
  const STRUCT& getStructRef() const { return data; };

  static auto create(const STRUCT& data, std::shared_ptr<OWNER> owner) {
    if constexpr (std::is_same_v<OWNER, std::nullptr_t>) {
      auto ret = std::shared_ptr<StructConstReference<STRUCT, OWNER>>(
          new StructConstReference<STRUCT, OWNER>{data});
      ret->init(ret->shared_from_this());
      return ret;
    } else {
      auto ret = std::shared_ptr<StructConstReference<STRUCT, OWNER>>(
          new StructConstReference<STRUCT, OWNER>{data, owner});
      ret->init(ret->shared_from_this());
      return ret;
    }
  }
};

namespace details {

// ---

template <class META_STRUCT, class META>
struct StringAttribute : TypedAttributeBaseImpl<META_STRUCT, META> {
  static_assert(meta::string_attribute_meta<META>, "T must contain an string");
  using TypedAttributeBaseImpl<META_STRUCT, META>::TypedAttributeBaseImpl;

  std::string to_string(size_t index = 0) const override {
    return this->get_value(index);
  };
  void from_string(std::string_view text, size_t index = 0) override {
    this->get_value(index) = text;
  };
};

template <class META_STRUCT, class META>
struct EnumAttribute : TypedAttributeBaseImpl<META_STRUCT, META> {
  static_assert(meta::enum_attribute_meta<META>, "type must contain an enum");
  using TypedAttributeBaseImpl<META_STRUCT, META>::TypedAttributeBaseImpl;

  std::string to_string(size_t index = 0) const override {
    return meta_of_t<value_type_t<typename META::ATTR_TYPE>>::enum2string(
        this->get_value(index));
  };
  void from_string(std::string_view text, size_t index = 0) override {
    std::string t{text};
    this->get_value(index) =
        meta_of_t<value_type_t<typename META::ATTR_TYPE>>::string2enum(
            t.c_str());
  };
};

template <class META_STRUCT, class META>
struct RawAttribute : TypedAttributeBaseImpl<META_STRUCT, META> {
  static_assert(meta::raw_attribute_meta<META>,
                "type must contain a raw value");
  using TypedAttributeBaseImpl<META_STRUCT, META>::TypedAttributeBaseImpl;

  std::string to_string(size_t index = 0) const override {
    std::stringstream s;
    s << this->get_value(index);
    return s.str();
  };
  void from_string(std::string_view text, size_t index = 0) override {
    std::stringstream s{std::string{text}};
    meta::value_type_t<typename META::ATTR_TYPE> x;  // TODO META::FINAL_VALUE?
    s >> x;
    this->get_value(index) = x;
  };
};

template <class META_STRUCT, class META>
struct StructAttribute : TypedAttributeBaseImpl<META_STRUCT, META> {
  static_assert(meta::struct_attribute_meta<META>,
                "type must contain a raw value");
  using TypedAttributeBaseImpl<META_STRUCT, META>::TypedAttributeBaseImpl;

  std::string to_string(size_t index = 0) const override {
    return to_json(this->get_value(index));
  };
  void from_string(std::string_view text, size_t index = 0) override {
    return from_json(text, this->get_value(index));
  };

  bool is_struct() const override { return true; }

  std::shared_ptr<UntypedStruct> get_struct(size_t index) override {
    return StructReference<meta::value_type_t<typename META::ATTR_TYPE>,
                           UntypedStruct>::create(this->get_value(index),
                                                  this->get_owner());
  }

  std::shared_ptr<const UntypedStruct> get_struct(size_t index) const override {
    return StructConstReference<meta::value_type_t<typename META::ATTR_TYPE>,
                                const UntypedStruct>::create(this
                                                                 ->get_value(
                                                                     index),
                                                             this->get_owner());
  }
};

// ---

template <class META_STRUCT>
template <class META>
void StructInitVisitor<META_STRUCT>::visitScalarString() {
  attributes.push_back(
      std::make_shared<StringAttribute<META_STRUCT, META>>(parent));
}
template <class META_STRUCT>
template <class META>
void StructInitVisitor<META_STRUCT>::visitScalarEnum() {
  attributes.push_back(
      std::make_shared<EnumAttribute<META_STRUCT, META>>(parent));
}
template <class META_STRUCT>
template <class META>
void StructInitVisitor<META_STRUCT>::visitScalarStruct() {
  attributes.push_back(
      std::make_shared<StructAttribute<META_STRUCT, META>>(parent));
}
template <class META_STRUCT>
template <class META>
void StructInitVisitor<META_STRUCT>::visitScalarRawType() {
  attributes.push_back(
      std::make_shared<RawAttribute<META_STRUCT, META>>(parent));
}
template <class META_STRUCT>
template <class META>
void StructInitVisitor<META_STRUCT>::visitArrayString() {
  attributes.push_back(
      std::make_shared<StringAttribute<META_STRUCT, META>>(parent));
}
template <class META_STRUCT>
template <class META>
void StructInitVisitor<META_STRUCT>::visitArrayEnum() {
  attributes.push_back(
      std::make_shared<EnumAttribute<META_STRUCT, META>>(parent));
}
template <class META_STRUCT>
template <class META>
void StructInitVisitor<META_STRUCT>::visitArrayStruct() {
  attributes.push_back(
      std::make_shared<StructAttribute<META_STRUCT, META>>(parent));
}
template <class META_STRUCT>
template <class META>
void StructInitVisitor<META_STRUCT>::visitArrayRawType() {
  attributes.push_back(
      std::make_shared<RawAttribute<META_STRUCT, META>>(parent));
};

}  // namespace details

}  // namespace meta
