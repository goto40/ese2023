/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { ArrayType, Attribute, AttributeType, Enum, isArrayType, isAttribute, isEnum, isFixedSizedArray, isRawType, isStruct, Namespace, NL, OutputString, RawType, Struct } from '@ese/json-schema-reduced';

export function getCppNamespace(namespace: Namespace): string {
    let name = namespace.name;
    const separated = /\w+:\/\/([\w/.]*)/.exec(name);
    if (separated!==null) {
        name = separated[1];
    }
    if (name.length>5 && name.endsWith('.json')) name = name.substring(0,name.length-5);
    name = name.replace(/[/.]+/g, '_');
    return name;
}

export function getCppType(t: AttributeType|Attribute): string {
    if (isAttribute(t)) {
        const result = getCppType(t.type);
        if (!t.required) return `std::optional<${result}>`;
        else return result;
    }
    else {
        return getCppTypeWithoutOptional(t);
    }
}

export function getCppTypeWithoutOptional(t: AttributeType): string {
    if (isRawType(t)) {
        return getCppRawType(t);
    }
    else if (isEnum(t) || isStruct(t)) {
        return `${getCppNamespace(t.parent)}::${t.name}`;
    }
    else if (isArrayType(t)) {
        return getCppArrayType(t);
    }
    else {
        throw new Error('unexpected1');
    }
}

export function getCppRawType(t: RawType): string {
    if (t.type === 'string') {
        return 'std::string';
    }
    else if (t.type === 'boolean') {
        return 'bool';
    }
    else if (t.type === 'number') {
        return 'double';
    }
    else if (t.type === 'integer') {
        if (t.signed) {
            return `int${t.bits}_t`;
        }
        else {
            return `uint${t.bits}_t`;
        }
    }
    else {
        throw new Error(`unexpected type===${t.type} ${JSON.stringify(t.json)}`);
    }
}

export function getCppArrayType(t: ArrayType): string {
    if (isFixedSizedArray(t.jsonArray)) {
        return `std::array<${getCppTypeWithoutOptional(t.type)},${t.maxItems}>`;
    }
    else {
        return `std::vector<${getCppTypeWithoutOptional(t.type)}>`;
    }
}

export function createMetaTypeTrait(out: OutputString, structOrEnum: Struct|Enum): void {
    out.append(`template<> struct meta::meta_of<${getCppNamespace(structOrEnum.parent)}::${structOrEnum.name}>: std::type_identity<${getCppNamespace(structOrEnum.parent)}::meta::${structOrEnum.name}> {};`, NL);
}
