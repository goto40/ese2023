/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { Attribute, isArrayType, isEnum, isFixedSizedArray, isInteger, isString, isStruct, NL, OutputString, Struct } from '@ese/json-schema-reduced';
import { getCppEnumFilename } from './enum';
import { createMetaTypeTrait, getCppNamespace, getCppType } from './language-support-cpp';

export function getCppStructFilename(struct: Struct): string {
    return getCppNamespace(struct.parent).replace(/::/g,'/')+'/'+struct.name+'.h';
}

export function escape_string_for_cpp(text: string): string {
    return text.replace(/\n/g,'\\n').replace(/"/g,'\\"');
}

export function generateCppStructCode(out: OutputString, struct: Struct): void {
    out.append('#pragma once', NL);
    createIncludes(out, struct);
    out.append(`namespace ${getCppNamespace(struct.parent)} {`, NL);
    out.indent(out=>{
        out.append(`struct ${struct.name} {`, NL);
        out.indent(out=>{
            struct.attributes.forEach(a=>{
                out.append(`${getCppType(a)} ${a.name}; // attribute ${a.name}`, NL);
            });
        });
        out.append('};',NL,NL);
        out.append('namespace meta {',NL);
        out.indent(out=>{
            out.append(`struct ${struct.name} {`, NL);
            out.indent(out=>{
                out.append(`static constexpr const char* name() { return "${struct.name}"; }`, NL);
                out.append(`static constexpr const char* description() { return "${escape_string_for_cpp(struct.description)}"; }`, NL);
                out.append('struct attributes {', NL);
                out.indent(out=>{
                    struct.attributes.forEach(a=>{
                        createAttributeMeta(out, struct, a);
                    });
                });
                out.append('};',NL);
                out.append(NL);
                createListOfMetaInfos(out, struct);
            });
            out.append('};',NL);
        });
        out.append('}',NL);
    });
    out.append('}',NL);
    out.append(NL);  
    createMetaTypeTrait(out, struct);
}

export function createAttributeMeta(out: OutputString, struct: Struct, a: Attribute): void {
    out.append(`struct ${a.name} {`, NL);
    out.indent(out=>{
        out.append(`using STRUCT = ${getCppType(struct)};`, NL);
        out.append(`using ATTR_TYPE = ${getCppType(a)};`, NL);
        out.append(`static constexpr const char* name() { return "${a.name}"; }`, NL);
        out.append(`static constexpr const char* description() { return "${escape_string_for_cpp(a.description)}"; }`, NL);
        out.append(`static constexpr std::array<const char,${a.name.length+1}> ct_name() { return {"${a.name}"}; }`, NL);
        out.append(`static constexpr auto& get(STRUCT &s) { return s.${a.name}; }`, NL);
        out.append(`static constexpr const auto& get(const STRUCT &s) { return s.${a.name}; }`, NL);
    });
    out.append('};',NL);
}

export function createListOfMetaInfos(out: OutputString, struct: Struct): void {
    out.append('template<class ...T> struct List;', NL);
    out.append('using ListOfAttributeMetaClasses = List<', NL);
    out.indent(out=>{
        struct.attributes.forEach((a, index)=>{
            let sep='';
            if (index<struct.attributes.length-1) sep=',';
            out.append(`attributes::${a.name}${sep}`, NL);
        });
    });
    out.append('>;', NL);
}

export function createIncludes(out: OutputString, struct: Struct): void {    
    out.append('#include <type_traits>', NL);
    if (struct.attributes.some(a=>!a.required)) {
        out.append('#include <optional>', NL);
    }
    if (struct.attributes.some(a=>isString(a.type) || (isArrayType(a.type) && isString(a.type.type)))) {
        out.append('#include <string>', NL);
    }
    if (struct.attributes.some(a=>isInteger(a.type) || (isArrayType(a.type) && isInteger(a.type.type)))) {
        out.append('#include <cstdint>', NL);
    }
    // if (struct.attributes.some(a=>isArrayType(a.type) && isFixedSizedArray(a.type.jsonArray))) {
    //     out.append('#include <array>', NL);
    // }
    out.append('#include <array>', NL); // because of ct_name()
    if (struct.attributes.some(a=>isArrayType(a.type) && !isFixedSizedArray(a.type.jsonArray))) {
        out.append('#include <vector>', NL);
    }
    out.append(NL);
    out.append('#include "meta/type_traits.h"', NL);
    out.append(NL);
    const includes = new Set<string>();
    struct.attributes.forEach(a=>{
        if (isEnum(a.type)) includes.add(`#include "${getCppEnumFilename(a.type)}"`);
        if (isStruct(a.type)) includes.add(`#include "${getCppStructFilename(a.type)}"`);
        if (isArrayType(a.type) && isEnum(a.type.type)) includes.add(`#include "${getCppEnumFilename(a.type.type)}"`);
        if (isArrayType(a.type) && isStruct(a.type.type)) includes.add(`#include "${getCppStructFilename(a.type.type)}"`);
    });
    Array.from(includes).sort().forEach(i=>{
        out.append(i, NL);
    });
    if (includes.size>0) out.append(NL);
}