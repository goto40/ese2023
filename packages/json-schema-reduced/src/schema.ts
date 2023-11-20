/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import * as fs from 'fs';
import Ajv from 'ajv';
import * as schemaSchema from './MySchema.json';
import { Enum, finalizeReferences, Namespace, prepareNamespace, Struct } from './model';
import { highLevelPlausibility } from './highlevel-plausibility';
const ajv = new Ajv();

export interface MySchema {
    $schema: string;
    $id: string;
    title: string;
    description: string;
    $defs: Record<string,JsonStruct|JsonEnum>;
}
export interface JsonStruct {
    description?: string;
    properties: Record<string,JsonRef|JsonRawType|JsonArrayType>;
    required?: string[];
}
export interface JsonRawType {
    type: 'string'|'integer'|'number'|'boolean';
    description?: string;
    minimum?: number;
    maximum?: number;
}
export interface JsonEnum {
    enum: string[];
}
export interface JsonArrayType {
    type: 'array';
    description?: string;
    items: JsonRawType|JsonRef;
    minItems?: number;
    maxItems?: number;
}
export interface JsonArrayTypeWithMaxItems {
    type: 'array';
    items: JsonRawType|JsonRef;
    minItems?: number;
    maxItems: number;
}
export interface JsonRef {
    description?: string;
    $ref: string;
}
export type JsonStructOrEnum = JsonStruct|JsonEnum;

export function isJsonEnum(obj: unknown): obj is JsonEnum {
    return typeof obj === 'object' && obj !== null && 'enum' in obj;
}
export function isJsonStructOrEnum(obj: unknown): obj is JsonStructOrEnum {
    return isJsonStruct(obj)||isJsonEnum(obj);
}
export function isJsonStruct(obj: unknown): obj is JsonStruct {
    return typeof obj === 'object' && obj !== null && 'properties' in obj;
}
export function isJsonRef(obj: unknown): obj is JsonRef {
    return typeof obj === 'object' && obj !== null && '$ref' in obj;
}
export function isJsonArrayType(obj: unknown): obj is JsonArrayType {
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    return typeof obj === 'object' && obj !== null && 'type' in obj && (<any>obj).type==='array';
}
export function isJsonRawType(obj: unknown): obj is JsonRawType {
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    return typeof obj === 'object' && obj !== null && 'type' in obj && (<any>obj).type!=='array';
}

export class GlobalModel {
    schemas = new Map<string, MySchema>(); // raw model
    namespaces = new Map<string, Namespace>(); // high level model
    filenames = new Map<string, string>();

    isMySchema(schema: unknown, errors?: string[]): schema is MySchema {
        const validator = ajv.compile(schemaSchema);
        if (validator(schema)) {
            return true;
        }
        else {
            if (errors!==undefined) {
                validator.errors?.forEach(e=>{
                    errors.push(`error ${e.instancePath}, ${e.schemaPath}: ${e.message}`);
                });
            }
        }
        return false;
    }

    isValidMySchema(schema: unknown, id: string|undefined, errors?: string[]): schema is MySchema {
        const throwOnError = errors===undefined;
        if (errors===undefined) errors = new Array<string>(); // dummy error list, if no error info is requested
        let removeId: string|undefined;
        if (this.isMySchema(schema, errors)) {
            if (id===undefined) id = schema.$id;
            if (schema.$id!==undefined && schema.$id!==id) throw new Error(`unexpected isValidMySchema, $id (${schema.$id}) != id (${id})`);

            if (schema.$id!==undefined && !this.schemas.has(schema.$id)) {
                this.schemas.set(schema.$id, schema);
                removeId = schema.$id;
            }
        }
        const result = this.isMySchema(schema, errors) && this.validateMySchema('root://', schema, errors, id);
        if (removeId!==undefined) {
            this.schemas.delete(removeId);
        }
        if (throwOnError && !result) {
            throw new Error('schema invalid, '+errors.map(e=>`${e}`).join(','));
        }
        return result;
    }

    validateMySchema(location: string, element: JsonRawType|JsonArrayType|JsonRef|JsonStruct|MySchema|JsonEnum, errors: string[], id: string|undefined): boolean {
        let result = true; // valid
        if (isJsonStruct(element)) {
            Object.entries(element.properties).forEach(([name, property])=>{
                result = this.validateMySchema(location+'::'+name, property, errors, id) && result;
            });
            if (element.required!==undefined) {
                const required = element.required;
                element.required.forEach(r=>{
                    if (!Object.keys(element.properties).includes(r)) {
                        result = false;
                        errors.push(`${location}: required '${r}' is not a valid field`);
                    }
                });
                Object.keys(element.properties).forEach(name=>{
                    if (!required.includes(name)) {
                        result = false;
                        errors.push(`${location}: field '${name}' is not required`);
                    }
                });
            }
            else {
                result = false;
                errors.push(`${location}: all fields must be required (missing field "required")`);
            }
        }
        else if (isJsonRawType(element)) {
            if (element.minimum !== undefined && element.maximum !== undefined && element.minimum>element.maximum) {
                result = false;
                errors.push(`${location}: minimum (${element.minimum}) > maximum (${element.maximum}).`);
            }
        }
        else if (isJsonArrayType(element)) {
            if (element.minItems !== undefined && element.maxItems !== undefined && element.minItems>element.maxItems) {
                result = false;
                errors.push(`${location}: minItems (${element.minItems}) > maxItems (${element.maxItems}).`);
            }
            if (element.minItems !== undefined && element.minItems < 0) {
                result = false;
                errors.push(`${location}: minItems (${element.minItems}) must be >=0.`);
            }
            if (element.maxItems !== undefined && element.maxItems < 1) {
                result = false;
                errors.push(`${location}: maxItems (${element.maxItems}) must be >=1.`);
            }
            if (isJsonRawType(element.items)) {
                result = this.validateMySchema(location+'::items', element.items, errors, id) && result;
            }
        }
        else if (isJsonRef(element)) {
            try {
                this.getJsonStructOrEnum(element.$ref, id);
            }
            catch(e) {
                result = false;
                errors.push(`${e}`);
            }
        }
        else if (isJsonEnum(element)) {
            // nothing
        }
        else {
            Object.entries(element.$defs).forEach(([name, struct])=>{
                result = this.validateMySchema(name, struct, errors, id) && result;
            });
        }
        return result;
    }

    addValidSchema(data: object): void {
        this.loadModels([data]);
    }

    loadModels(data: Array<string|object>, errors?: string[]): boolean {
        let result = true;
        let throwOnError = false;
        if (errors===undefined) {
            errors = new Array<string>();
            throwOnError = true;
        }
        data.forEach(filenameOrSchema=>{
            if (errors===undefined) throw new Error('unexpected');
            let filename = '<nofile>';
            let mySchema: object;
            if (typeof filenameOrSchema === 'string') {
                filename = filenameOrSchema;
                mySchema = JSON.parse(fs.readFileSync(filename, 'utf-8'));
            }
            else {
                mySchema = filenameOrSchema;
            }
            //console.log(JSON.stringify(mySchema));
            if (this.isMySchema(mySchema,errors)) { // 1. rough plausibility
                const id = mySchema.$id;
                if (id!==undefined) {
                    if (this.schemas.has(id)) {
                        errors.push(`${id} in ${filename} already defined in ${this.filenames.get(id)} (double id!)`);
                        result = false;
                    }
                    else {
                        this.schemas.set(id, mySchema);
                        this.filenames.set(id, filename);
                    }
                }
            }
            else {
                result = false;
            }
        });
        if (result) {
            this.schemas.forEach((mySchema, id)=>{
                if (this.isValidMySchema(mySchema, mySchema.$id, errors)) { // 2. detailed validation
                    this.namespaces.set(id, prepareNamespace(mySchema, id));
                }
                else {
                    result = false;
                }
            });
            finalizeReferences(this);

            if (result && !highLevelPlausibility(this, errors)) {
                result = false;
            }
        }
        if (throwOnError && !result) throw new Error('errors:'+errors.join(';'));
        return result;
    }

    getJsonStructOrEnum(ref0: string, id?: string): JsonStructOrEnum {
        return this.getJson(ref0, isJsonStructOrEnum, 'JsonStructOrEnum', id);
    }
    getJsonStruct(ref0: string, id?: string): JsonStruct {
        return this.getJson(ref0, isJsonStruct, 'JsonStruct', id);
    }
    getJsonEnum(ref0: string, id?: string): JsonEnum {
        return this.getJson(ref0, isJsonEnum, 'JsonEnum', id);
    }

    getIdAndNameAndFullRefOfReference(ref0: string, id?: string): [string, string] {
        let ref = ref0.slice();
        const hashIdx = ref.indexOf('#');
        if (hashIdx===-1) throw new Error(`${ref} has no hash`);
        if (hashIdx>0) {
            id = ref.substring(0,hashIdx);
            ref = ref.substring(hashIdx+1);
        }
        else {
            if (id===undefined) throw new Error('no id specified');
            ref = ref.substring(1);
        }
        if (!ref.startsWith('/$defs/')) throw new Error(`${ref} must start with '/$defs/'`);
        const name = ref.substring('/$defs/'.length);
        //ref = id+'#/$defs/'+name;
        return [id, name];
    }

    getJson<T>(ref0: string, isType: (x: unknown) => x is T, typeName: string, id0?: string): T {
        const [id, name] = this.getIdAndNameAndFullRefOfReference(ref0,id0);
        const schema = this.schemas.get(id);
        if (schema===undefined) throw new Error(`${ref0} not found (id ${id} not found in json schemas)`);
        if (!(name in schema.$defs)) throw new Error(`${ref0} not json-found/enum (struct ${name} not present)`);
        const result = schema.$defs[name];
        if (isType(result)) return result;
        else throw new Error(`${ref0} is not of type ${typeName}`);
    }

    get(ref0: string, id0?: string): Struct|Enum {
        const [id, name] = this.getIdAndNameAndFullRefOfReference(ref0,id0);
        const namespace = this.namespaces.get(id);
        if (namespace===undefined) throw new Error(`${ref0} not found (id ${id} not found)`);
        const result = namespace.elements.find(e=>e.name===name);
        if (result===undefined) throw new Error(`${ref0} not found (struct/enum ${name} not present in [${namespace.elements.map(e=>e.name).join(',')}])`);
        return result;
    }
}
