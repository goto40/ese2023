/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { ArrayType, Attribute, AttributeType, Enum, isArrayType, isEnum, isRawType, isStruct, Namespace, RawType, Struct } from '@ese/json-schema-reduced';

export function getPyNamespace(namespace: Namespace): string {
    let name = namespace.name;
    const separated = /\w+:\/\/([\w/.]*)/.exec(name);
    if (separated!==null) {
        name = separated[1];
    }
    if (name.length>5 && name.endsWith('.json')) name = name.substring(0,name.length-5);
    name = name.replace(/[/.]+/g, '_');
    return name;
}

export function getPyModuleName(t: Enum|Struct): string {
    return `${getPyNamespace(t.parent)}.${t.name}`;
}

export function getPyAttributeType(a: Attribute): string {
    const result = getPyType(a.type);
    if (a.required) return result;
    else return `Optional[${result}]`;
}

export function getPyType(t: AttributeType): string {
    if (isRawType(t)) {
        return getPyRawType(t);
    }
    else if (isEnum(t) || isStruct(t)) {
        return `${getPyNamespace(t.parent)}.${t.name}.${t.name}`;
    }
    else if (isArrayType(t)) {
        return getPyArrayType(t);
    }
    else {
        throw new Error('unexpected1');
    }
}

export function getPyRawType(t: RawType): string {
    if (t.type === 'string') {
        return 'str';
    }
    else if (t.type === 'boolean') {
        return 'bool';
    }
    else if (t.type === 'number') {
        return 'float';
    }
    else if (t.type === 'integer') {
        return 'int';
    }
    else {
        throw new Error(`unexpected type===${t.type} ${JSON.stringify(t.json)}`);
    }
}

export function getPyArrayType(t: ArrayType): string {
    return `List[${getPyType(t.type)}]`;
}
