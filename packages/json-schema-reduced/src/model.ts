/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { GlobalModel, isJsonArrayType, isJsonEnum, isJsonRawType, isJsonRef, JsonArrayType, JsonEnum, JsonRawType, JsonRef, JsonStruct, MySchema } from './schema';
import { getIsRequired, getTypeBitWidth, isTypeSigned } from './tools';

export interface Namespace {
    json: MySchema;
    $type: 'Namespace';
    name: string;
    elements: Array<Struct|Enum>;
}

export interface Struct {
    json: JsonStruct;
    $type: 'Struct';
    name: string;
    description: string;
    parent: Namespace;
    attributes: Attribute[];
}

export interface Attribute {
    json: JsonRawType|JsonArrayType|JsonRef;
    $type: 'Attribute';
    name: string;
    description: string;
    parent: Struct;
    type: AttributeType;
    required: boolean;
}

export type AttributeType = Struct|Enum|RawType|ArrayType;

export interface Enum {
    json: JsonEnum;
    $type: 'Enum'
    name: string;
    parent: Namespace;
    values: string[];
}

export function isAttribute(element: unknown): element is Attribute {
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    return typeof element === 'object' && element !== null && (<any>element).$type === 'Attribute';
}

export function isEnum(element: unknown): element is Enum {
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    return typeof element === 'object' && element !== null && (<any>element).$type === 'Enum';
}

export function isNamespace(element: unknown): element is Enum {
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    return typeof element === 'object' && element !== null && (<any>element).$type === 'Namespace';
}

export function isRawType(element: unknown): element is RawType {
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    return typeof element === 'object' && element !== null && (<any>element).$type === 'RawType';
}

export function isString(element: unknown): element is RawType {
    return isRawType(element) && element.type==='string';
}
export function isNumber(element: unknown): element is RawType {
    return isRawType(element) && element.type==='number';
}
export function isInteger(element: unknown): element is RawType {
    return isRawType(element) && element.type==='integer';
}
export function isBoolean(element: unknown): element is RawType {
    return isRawType(element) && element.type==='boolean';
}

export function isStruct(element: unknown): element is Struct {
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    return typeof element === 'object' && element !== null && (<any>element).$type === 'Struct';
}

export interface RawType {
    json: JsonRawType;
    $type: 'RawType';
    type: 'number'|'integer'|'string'|'boolean';
    signed?: boolean;
    bits?: number;
    minimum?: number;
    maximum?: number;
}

export interface ArrayType {
    json: JsonRef|JsonRawType;
    jsonArray: JsonArrayType;
    $type: 'ArrayType';
    type: Struct|Enum|RawType;
    maxItems?: number;
    minItems?: number;
}

export function isArrayType(element: unknown): element is ArrayType {
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    return typeof element === 'object' && element !== null && (<any>element).$type === 'ArrayType';
}

export function prepareNamespace(mySchema: MySchema, id: string): Namespace {
    return {
        json: mySchema,
        $type: 'Namespace',
        name: id,
        elements: prepareElements(mySchema)
    };
}

export function prepareElements(mySchema: MySchema): Array<Struct|Enum> {
    const result = new Array<Struct|Enum>();
    Object.keys(mySchema.$defs).forEach(name =>{
        const structOrEnum = mySchema.$defs[name];
        if (isJsonEnum(structOrEnum)) result.push(prepareEnum(name, structOrEnum));
        else result.push(prepareStruct(name, structOrEnum));
    });
    return result;
}

export function prepareEnum(name: string, e: JsonEnum): Enum {
    return {
        json: e,
        $type: 'Enum',
        name: name,
        // eslint-disable-next-line @typescript-eslint/no-explicit-any
        parent: <any>undefined,
        values: e.enum
    };
}

export function prepareStruct(name: string, struct: JsonStruct): Struct {

    function prepareType(t: JsonRef|JsonRawType|JsonArrayType): AttributeType|undefined {
        if (isJsonRawType(t)) {
            return {
                json: t,
                $type: 'RawType',
                type: t.type,
                signed: isTypeSigned(t),
                bits: getTypeBitWidth(t),
                minimum: t.minimum,
                maximum: t.maximum
            };
        }
        else if (isJsonArrayType(t)) {
            const it = prepareType(t.items);
            if (isArrayType(it)) throw new Error('impossible');
            if (!isJsonRawType(t.items)&&!isJsonRef(t.items)) throw new Error('impossible');
            const result: ArrayType = {
                json: t.items,
                jsonArray: t,
                $type: 'ArrayType',
                // eslint-disable-next-line @typescript-eslint/no-explicit-any
                type: <any>it,
                minItems: t.minItems,
                maxItems: t.maxItems
            };
            return result;
        }
        else {
            return undefined;
        }
    }

    function prepareAttributes(struct: JsonStruct): Attribute[] {
        const result = new Array<Attribute>();
        Object.keys(struct.properties).forEach(attrName =>{
            const t = struct.properties[attrName];
            result.push({
                json: t,
                $type: 'Attribute',
                name: attrName,
                description: t.description??'',
                // eslint-disable-next-line @typescript-eslint/no-explicit-any
                parent: <any>undefined,
                // eslint-disable-next-line @typescript-eslint/no-explicit-any
                type: <any>prepareType(t),
                required: getIsRequired(struct, attrName)===true
            });
            //console.log(`${attrName} --> ${prepareType(t)}`);
        });
        return result;
    }
    return {
        json: struct,
        $type: 'Struct',
        name: name,
        description: struct.description??'',
        // eslint-disable-next-line @typescript-eslint/no-explicit-any
        parent: <any>undefined,
        attributes: prepareAttributes(struct)
    };
}

export function* getAllStructs(data: GlobalModel) {
    for(const [,namespace] of data.namespaces) {
        for (const e of namespace.elements) {
            if (isStruct(e)) yield e;
        }
    }
}

export function* getAllEnums(data: GlobalModel) {
    for(const [,namespace] of data.namespaces) {
        for (const e of namespace.elements) {
            if (isEnum(e)) yield e;
        }
    }
}

export function* getAllNamespaces(data: GlobalModel) {
    for(const [,namespace] of data.namespaces) {
        yield namespace;
    }
}

export function* getAllTypesWithRef(struct: Struct)  {
    for(const attribute of struct.attributes) {
        if (isJsonRef(attribute.json)) {
            yield attribute;
        }
        else if (isArrayType(attribute.type)) {
            if (isJsonRef(attribute.type.json)) {
                yield attribute.type;
            }
        }
    }
}

export function finalizeReferences(model: GlobalModel): void {
    model.namespaces.forEach((namespace, id)=>{
        namespace.elements.filter(isEnum).forEach(e=>{
            e.parent = namespace;
        });
        namespace.elements.filter(isStruct).forEach(struct=>{
            struct.parent = namespace;
            struct.attributes.forEach(a=>{
                a.parent = struct;
            });
            for(const t of getAllTypesWithRef(struct)) {
                if (t.type === undefined) {
                    if (!isJsonRef(t.json)) throw new Error('unexpected');
                    const referenceLink = t.json.$ref;
                    const resolvedReference = model.get(referenceLink, id);
                    t.type = resolvedReference;
                    //console.log(`${t.type}->${referenceLink}`);
                }
            }
        });
    });
}
