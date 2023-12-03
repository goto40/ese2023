#!/usr/bin/env python3
#******************************************************************************
#  Copyright 2023 Pierre Bayerl and Michael Thoma
#  This program and the accompanying materials are made available under the
#  terms of the MIT License, which is available in the project root.
#******************************************************************************

import json
import logging
import sys

from google.protobuf.compiler import plugin_pb2 as plugin
from google.protobuf.descriptor_pb2 import FileDescriptorProto
from google.protobuf.descriptor_pb2 import FieldDescriptorProto

cpptypes = {
    FieldDescriptorProto.TYPE_DOUBLE: 'double',
    FieldDescriptorProto.TYPE_FLOAT: 'float',
    FieldDescriptorProto.TYPE_INT32: 'int32_t',
    FieldDescriptorProto.TYPE_INT64: 'int64_t',
    FieldDescriptorProto.TYPE_UINT32: 'uint32_t',
    FieldDescriptorProto.TYPE_UINT64: 'uint64_t',
    FieldDescriptorProto.TYPE_SINT32: 'int32_t',
    FieldDescriptorProto.TYPE_SINT64: 'int64_t',
    FieldDescriptorProto.TYPE_FIXED32: 'uint32_t',
    FieldDescriptorProto.TYPE_FIXED64: 'uint64_t',
    FieldDescriptorProto.TYPE_SFIXED32: 'int32_t',
    FieldDescriptorProto.TYPE_SFIXED64: 'int64_t',
    FieldDescriptorProto.TYPE_BOOL: 'bool',
    FieldDescriptorProto.TYPE_STRING: 'std::string',
    FieldDescriptorProto.TYPE_BYTES: 'std::string',
    FieldDescriptorProto.TYPE_ENUM: 'int',
    FieldDescriptorProto.TYPE_MESSAGE: 'message',
}

logger = logging.getLogger(__name__)
logging.basicConfig(
    level=logging.INFO, format="%(asctime)s - %(name)s - %(levelname)s - %(message)s"
)


def process_file(
        proto_file: FileDescriptorProto, response: plugin.CodeGeneratorResponse
) -> None:
    logger.info(f"Processing proto_file: {proto_file.name}")

    # Create list of dependencies
    dependencies_list = list(proto_file.dependency)

    file = response.file.add()
    file.name = proto_file.name.removesuffix(".proto") + ".meta.h"
    logger.info(f"Creating new file: {file.name}")

    file.content = "// clang-format off\n"
    file.content += "#pragma once\n\n"
    file.content += '#include "meta/grpc.h"\n'
    file.content += '#include "meta/meta.h"\n'
    file.content += f'#include "{proto_file.name.removesuffix(".proto")}.pb.h"\n'
    for dep in dependencies_list:
        file.content += f'#include "{dep.removesuffix(".proto")}.meta.h"\n'
    file.content += f'\n'

    for enum in proto_file.enum_type:
        file.content += f'namespace {proto_file.package} {{\n'
        file.content += f'namespace meta {{\n'
        file.content += f'struct {enum.name} {{\n'
        file.content += f'static const char* enum2string(int i) {{ return {proto_file.package}::{enum.name}_Name(static_cast<{proto_file.package}::{enum.name}>(i)).c_str(); }}\n'
        file.content += f'static const char* enum2string({proto_file.package}::{enum.name} c) {{ return {proto_file.package}::{enum.name}_Name(c).c_str(); }}\n'
        file.content += f'static {proto_file.package}::{enum.name} string2enum(const char* enumAsText) {{'
        first = True
        for value in enum.value:
            if first == True:
                first = False
            else:
                file.content += f'else '
            file.content += f'if (strcmp("{value.name}", enumAsText) == 0) return {proto_file.package}::{enum.name}::{value.name};\n'
        file.content += f'else throw new std::runtime_error("{proto_file.package}::{enum.name}: string for string2enum");'
        file.content += f'}};\n'
        file.content += f'}};\n'
        file.content += f'}} // namespace meta\n'
        file.content += f'std::ostream& operator<<(std::ostream& os, {enum.name} color) {{ return os << {enum.name}_Name(color); }}'
        file.content += f'}} // namespace {proto_file.package}\n'
        file.content += f'template <> struct meta::meta_of<{proto_file.package}::{enum.name}> : std::type_identity<{proto_file.package}::meta::{enum.name}> {{}};\n'
        file.content += f'template <typename Message> struct meta::meta_of<meta::grpc::FieldWrapper<{proto_file.package}::{enum.name}, Message>> : std::type_identity<{proto_file.package}::meta::{enum.name}> {{}};\n'
    file.content += f'\n'

    for msg in proto_file.message_type:
        file.content += f'namespace {proto_file.package} {{\n'
        file.content += f'namespace meta {{\n'
        file.content += f'struct {msg.name} {{\n'
        file.content += f'static constexpr const char* name() {{ return "{msg.name}"; }}\n'
        file.content += f'static constexpr const char* description() {{ return "{msg.name}"; }}\n'
        file.content += f'struct attributes {{\n'
        for field in msg.field:
            if field.type in cpptypes:
                file.content += f'struct {field.name} {{\n'
                file.content += f'static constexpr std::array<const char, {len(field.name)+1}> ct_name() {{ return {{"{field.name}"}}; }}\n'
                file.content += f'static constexpr const char* name() {{ return "{field.name}"; }}\n'
                file.content += f'static constexpr const char* description() {{ return "{field.name}"; }}\n'
                file.content += f'using STRUCT = {proto_file.package}::{msg.name};\n'
                if field.label == FieldDescriptorProto.LABEL_REPEATED:
                    if field.type == FieldDescriptorProto.TYPE_MESSAGE:
                        file.content += f'using ATTR_TYPE = ::meta::grpc::DynamicArrayWrapper<{field.type_name.replace(".","::")}, ::google::protobuf::RepeatedPtrField>;\n'
                    else:
                        file.content += f'using ATTR_TYPE = ::meta::grpc::DynamicArrayWrapper<{cpptypes[field.type]}, ::google::protobuf::RepeatedField>;\n'
                    file.content += f'static decltype(auto) get(STRUCT& s) {{ return ATTR_TYPE(*s.mutable_{field.name.lower()}());}}\n'
                    file.content += f'static decltype(auto) get(const STRUCT& s) {{ return ::meta::grpc::ConstDynamicArrayWrapper(s.{field.name.lower()}());}}\n'
                else:
                    if field.type == FieldDescriptorProto.TYPE_MESSAGE:
                        file.content += f"using ATTR_TYPE = {field.type_name.replace('.','::')};\n"
                        file.content += f'static decltype(auto) get(STRUCT& s) {{ return *s.mutable_{field.name.lower()}();}}\n'
                        file.content += f'static decltype(auto) get(const STRUCT& s) {{ return s.{field.name.lower()}();}}\n'
                    elif field.type == FieldDescriptorProto.TYPE_ENUM:
                        file.content += f"using ATTR_TYPE = {field.type_name.replace('.','::')};\n"
                        file.content += f'static decltype(auto) get(STRUCT& s) {{ return ::meta::grpc::FieldWrapper(s, &STRUCT::{field.name}, &STRUCT::set_{field.name});}}\n'
                        file.content += f'static decltype(auto) get(const STRUCT& s) {{ return ::meta::grpc::FieldWrapper(s, &STRUCT::{field.name});}}\n'
                    elif field.type == FieldDescriptorProto.TYPE_STRING or field.type == FieldDescriptorProto.TYPE_BYTES:
                        file.content += f'using ATTR_TYPE = {cpptypes[field.type]};\n'
                        file.content += f'static decltype(auto) get(STRUCT& s) {{ return *s.mutable_{field.name.lower()}();}}\n'
                        file.content += f'static decltype(auto) get(const STRUCT& s) {{ return s.{field.name.lower()}();}}\n'
                    else:
                        file.content += f'using ATTR_TYPE = {cpptypes[field.type]};\n'
                        file.content += f'static decltype(auto) get(STRUCT& s) {{ return ::meta::grpc::FieldWrapper(s, &STRUCT::{field.name}, &STRUCT::set_{field.name});}}\n'
                        file.content += f'static decltype(auto) get(const STRUCT& s) {{ return ::meta::grpc::FieldWrapper(s, &STRUCT::{field.name});}}\n'
                file.content += f'}};\n'
            else:
                logger.error(
                    f"unsupported type for field {field.name}: {field.type}")
        file.content += f'}};\n'
        file.content += f'template <class...T> struct List;\n'
        file.content += f'using ListOfAttributeMetaClasses = List<'
        first = True
        for field in msg.field:
            if first:
                first = False
            else:
                file.content += f', '
            file.content += f'attributes::{field.name}'
        file.content += f'>;\n'
        file.content += f'}};\n'
        file.content += f'}} // namespace meta\n'
        file.content += f'}} // namespace {proto_file.package}\n'
        file.content += f'template <> struct meta::meta_of<{proto_file.package}::{msg.name}> : std::type_identity<{proto_file.package}::meta::{msg.name}> {{}};\n'
        for field in msg.field:
            if field.type in cpptypes:
                if field.label == FieldDescriptorProto.LABEL_REPEATED:
                    if field.type == FieldDescriptorProto.TYPE_ENUM:
                        file.content += f'template <> struct meta::value_type<{proto_file.package}::meta::{msg.name}::attributes::{field.name}> {{ using type = {field.type_name.replace(".","::")}; }};\n'
                        file.content += f'template <> struct meta::value_type<{proto_file.package}::meta::{msg.name}::attributes::{field.name}::ATTR_TYPE> {{ using type = {field.type_name.replace(".","::")}; }};\n'
                else:
                    if not field.type == FieldDescriptorProto.TYPE_MESSAGE and not field.type == FieldDescriptorProto.TYPE_STRING and not field.type == FieldDescriptorProto.TYPE_BYTES:
                        file.content += f'template <> struct meta::value_type<{proto_file.package}::meta::{msg.name}::attributes::{field.name}> {{'
                        if field.type == FieldDescriptorProto.TYPE_MESSAGE or field.type == FieldDescriptorProto.TYPE_ENUM:
                            file.content += f'using type = {field.type_name.replace(".","::")};'
                        else:
                            file.content += f'using type = {cpptypes[field.type]};'
                        file.content += f'}};\n'
        file.content += f'\n'
    file.content += "// clang-format on\n"


def process(
        request: plugin.CodeGeneratorRequest, response: plugin.CodeGeneratorResponse
) -> None:
    for proto_file in request.proto_file:
        process_file(proto_file, response)


def main() -> None:
    # Load the request from stdin
    request = plugin.CodeGeneratorRequest.FromString(sys.stdin.buffer.read())

    # Create a response
    response = plugin.CodeGeneratorResponse()

    process(request, response)

    # Serialize response and write to stdout
    sys.stdout.buffer.write(response.SerializeToString())


if __name__ == "__main__":
    main()
