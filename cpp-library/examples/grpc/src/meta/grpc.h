/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#pragma once

#include <google/protobuf/repeated_field.h>
#include <meta/convert.h>
#include <meta/type_traits.h>

namespace meta {
namespace grpc {

template <typename T, typename Message>
class FieldWrapper {
 public:
  using NonConstMessage = std::remove_const_t<Message>;
  using Getter = T (NonConstMessage::*)() const;
  using Setter = void (NonConstMessage::*)(const T);

  FieldWrapper(Message& message, Getter getter, Setter setter)
    requires(!std::is_const_v<Message>)
      : message_{message}, getter_{getter}, setter_{setter} {}

  FieldWrapper(Message& message, Getter getter)
    requires(std::is_const_v<Message>)
      : message_{message}, getter_{getter}, setter_{} {}

  operator T() const { return (message_.*getter_)(); }

  FieldWrapper& operator=(const FieldWrapper& other)
    requires(!std::is_const_v<Message>)
  {
    if (this != &other) {
      (message_.*setter_)(static_cast<T>(other));
    }
    return *this;
  }

  FieldWrapper& operator=(const T& other)
    requires(!std::is_const_v<Message>)
  {
    (message_.*setter_)(static_cast<T>(other));
    return *this;
  }

  FieldWrapper& operator+=(const T& other)
    requires(!std::is_const_v<Message>)
  {
    (message_.*setter_)(static_cast<T>(*this) + static_cast<T>(other));
    return *this;
  };

 private:
  Message& message_;
  Getter getter_;
  Setter setter_;
};

template <typename T, template <typename> typename Container>
class ConstDynamicArrayWrapper {
 public:
  using iterator = Container<T>::iterator;
  using const_iterator = Container<T>::const_iterator;
  using value_type = Container<T>::value_type;
  using reference = Container<T>::reference;
  using const_reference = Container<T>::const_reference;
  using pointer = Container<T>::pointer;
  using const_pointer = Container<T>::const_pointer;
  using size_type = Container<T>::size_type;
  using difference_type = Container<T>::difference_type;
  using const_reverse_iterator = Container<T>::const_reverse_iterator;
  using reverse_iterator = Container<T>::reverse_iterator;

  ConstDynamicArrayWrapper(const Container<T>& data) : data_{data} {}

  ConstDynamicArrayWrapper(const ConstDynamicArrayWrapper&) = default;
  ConstDynamicArrayWrapper(ConstDynamicArrayWrapper&&) = default;
  ConstDynamicArrayWrapper& operator=(ConstDynamicArrayWrapper&&) = default;

  operator const Container<T>&() const { return data_; }

  auto size() const { return data_.size(); }

  const auto& operator[](int index) const { return data_[index]; }

  const_iterator begin() const { return data_.begin(); }
  const_iterator cbegin() const { return data_.cbegin(); }

  const_iterator end() const { return data_.end(); }
  const_iterator cend() const { return data_.cend(); }

  const_reverse_iterator rbegin() const { return data_.rbegin(); }
  reverse_iterator rend() const { return data_.rend(); }

 private:
  const Container<T>& data_;
};

template <typename T, template <typename> typename Container>
class DynamicArrayWrapper {
 public:
  using iterator = Container<T>::iterator;
  using const_iterator = Container<T>::const_iterator;
  using value_type = Container<T>::value_type;
  using reference = Container<T>::reference;
  using const_reference = Container<T>::const_reference;
  using pointer = Container<T>::pointer;
  using const_pointer = Container<T>::const_pointer;
  using size_type = Container<T>::size_type;
  using difference_type = Container<T>::difference_type;
  using const_reverse_iterator = Container<T>::const_reverse_iterator;
  using reverse_iterator = Container<T>::reverse_iterator;

  DynamicArrayWrapper(Container<T>& data) : data_{data} {}

  DynamicArrayWrapper(const DynamicArrayWrapper&) = default;
  DynamicArrayWrapper(DynamicArrayWrapper&&) = default;
  DynamicArrayWrapper& operator=(const DynamicArrayWrapper& other) {
    if (this != &other) {
      data_ = other.data_;
    }
    return *this;
  }
  DynamicArrayWrapper& operator=(
      const ConstDynamicArrayWrapper<T, Container>& other) {
    data_ = other.data_;
    return *this;
  }

  DynamicArrayWrapper& operator=(DynamicArrayWrapper&&) = default;

  operator const Container<T>&() const { return data_; }
  operator Container<T>&() { return data_; }

  auto size() const { return data_.size(); }

  void resize(size_t new_size) {
    data_.Clear();
    data_.Reserve(new_size);
    for (size_t i = 0; i != new_size; ++i) {
      data_.Add();
    }
  }

  const auto& operator[](int index) const { return data_[index]; }
  auto& operator[](int index) { return data_[index]; }

  iterator begin() { return data_.begin(); }
  const_iterator begin() const { return data_.begin(); }
  const_iterator cbegin() const { return data_.cbegin(); }

  iterator end() { return data_.end(); }
  const_iterator end() const { return data_.end(); }
  const_iterator cend() const { return data_.cend(); }

  reverse_iterator rbegin() { return data_.rbegin(); }
  const_reverse_iterator rbegin() const { return data_.rbegin(); }
  reverse_iterator rend() { return data_.rend(); }

 private:
  Container<T>& data_;
};

}  // namespace grpc

template <class T, class Message>
struct value_type<meta::grpc::FieldWrapper<T, Message>, false> {
  using type = T;
};

template <typename T, template <typename> typename Container>
struct is_dynamic_array_type<meta::grpc::DynamicArrayWrapper<T, Container>>
    : std::true_type {};

template <class U, class Message>
struct remove_optional<meta::grpc::FieldWrapper<U, Message>> {
  using type = U;
};

}  // namespace meta
