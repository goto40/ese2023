/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { isArrayType, Attribute, isEnum, isStruct, NL, OutputString, Struct } from '@ese/json-schema-reduced';
import { getPyAttributeType, getPyModuleName, getPyNamespace, getPyType } from './language-support-py';

export function getPyStructFilename(struct: Struct): string {
    return getPyNamespace(struct.parent).replace(/::/g,'/')+'/'+struct.name+'.py';
}

function initCommand(a: Attribute): string {
    if (a.required) return '';
    else return ` = field(default_factory=lambda: ${initValue(a)})`;
}

export function generateCppStructCode(out: OutputString, struct: Struct): void {
    out.append('from dataclasses import dataclass, field', NL);
    out.append('from dataclasses_json import dataclass_json, Undefined', NL);
    out.append('from typing import Optional, List');
    out.append(NL);
    createImports(out, struct);
    out.append('@dataclass_json(undefined=Undefined.RAISE)', NL);
    out.append('@dataclass', NL);
    out.append(`class ${struct.name}:`, NL);
    out.indent(out=>{
        struct.attributes.forEach(a=>{
            out.append(`${a.name}: ${getPyAttributeType(a)}${initCommand(a)}`, NL);
        });
    });
    out.append(NL);

    out.append(`def create${struct.name}():`, NL);
    function args(): string {
        return struct.attributes.map(a=>initValue(a)).join(',');
    }
    out.indent(out=>{
        out.append(`return ${struct.name}(${args()})`, NL);
    });
}

export function initValue(a: Attribute) {
    let s = '';
    if (isArrayType(a.type)) s = '[]';
    else if (isEnum(a.type)) s = `list(${getPyType(a.type)})[0]`;
    else s = `${getPyType(a.type)}()`;
    if (a.required) return s;
    else return 'None';
}

export function createImports(out: OutputString, struct: Struct): void {
    const includes = new Set<string>();
    struct.attributes.forEach(a=>{
        if (isEnum(a.type) || isStruct(a.type)) includes.add(`import ${getPyModuleName(a.type)}`);
        if (isArrayType(a.type) && (isEnum(a.type.type) || isStruct(a.type.type))) includes.add(`import ${getPyModuleName(a.type.type)}`);
    });
    Array.from(includes).sort().forEach(i=>{
        out.append(i, NL);
    });
    if (includes.size>0) out.append(NL);
}