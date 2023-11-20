/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { Enum, NL, OutputString } from '@ese/json-schema-reduced';
import { createMetaTypeTrait, getCppNamespace, getCppType } from './language-support-cpp';

export function getCppEnumFilename(enumObj: Enum): string {
    return getCppNamespace(enumObj.parent).replace(/::/g,'/')+'/'+enumObj.name+'.h';
}

export function generateCppEnumCode(out: OutputString, enumObj: Enum): void {
    out.append('#pragma once', NL);
    out.append('#include <cstring>', NL);
    out.append('#include <stdexcept>', NL);
    out.append('#include <ostream>', NL);
    out.append(NL);
    out.append(`namespace ${getCppNamespace(enumObj.parent)} {`, NL);
    out.indent(out=>{
        out.append(`enum class ${enumObj.name} {`, NL);
        out.indent(out=>{
            enumObj.values.forEach((v,index)=>{
                const comma = (index<enumObj.values.length-1)?',':'';
                out.append(`${v}${comma}`, NL);
            });
        });
        out.append('};',NL,NL);
        out.append('namespace meta {',NL);
        out.indent(out=>{
            out.append(`struct ${enumObj.name} {`, NL);
            out.indent(out=>{
                out.append(`static const char* enum2string(${getCppType(enumObj)} e) {`, NL);
                out.indent(out=>{
                    enumObj.values.forEach((v, index)=>{
                        out.append(`${(index>0)?'else ':''}if (e == ${getCppType(enumObj)}::${v}) return "${v}";`, NL);
                    });
                    out.append(`else throw new std::runtime_error("${getCppType(enumObj)}: unmapped enum value");`, NL);
                });
                out.append('}',NL);
                out.append(`static ${getCppType(enumObj)} string2enum(const char* enumAsText) {`, NL);
                out.indent(out=>{
                    enumObj.values.forEach((v, index)=>{
                        out.append(`${(index>0)?'else ':''}if (strcmp("${v}",enumAsText)==0) return ${getCppType(enumObj)}::${v};`, NL);
                    });
                    out.append(`else throw new std::runtime_error("${getCppType(enumObj)}: unmapped string for string2enum");`, NL);
                });
                out.append('}',NL);
            });
            out.append('};',NL);
        });
        out.append('}',NL);
        out.append(NL);
        out.append(`inline std::ostream& operator<< (std::ostream& os, const ${getCppType(enumObj)}& e) {`, NL);
        out.indent(out=>{
            out.append(`os << meta::${enumObj.name}::enum2string(e);`, NL);
            out.append('return os;', NL);
        });
        out.append('}', NL);
        out.append(NL);
    });
    out.append('}',NL);
    createMetaTypeTrait(out, enumObj);
}
