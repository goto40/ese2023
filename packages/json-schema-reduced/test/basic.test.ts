/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { GlobalModel } from '../src/schema';

test('simple model w/o errors', () => {
    const interfaceSpec = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    s: {type: 'string'},
                    i: {type: 'integer'},
                    f: {type: 'number'}
                },
                additionalProperties: false,
                required: ['s','i','f']
            },
            MyObj2: {
                type: 'object',
                properties: {
                    b: {type: 'boolean'},
                    s: {type: 'string'},
                    i8: {type: 'integer', minimum: -128, maximum: 127},
                    i16a: {type: 'integer', minimum: -129, maximum: 127},
                    i16b: {type: 'integer', minimum: -128, maximum: 128},

                    i16: {type: 'integer', minimum: -12, maximum: 5000},
                    u16: {type: 'integer', minimum: 12, maximum: 5000},
                    u32: {type: 'integer', minimum: 12, maximum: 500000},
                    f: {type: 'number', maximum: 12},
                    o: {$ref: '#/$defs/MyObj1'},

                    a1: {
                        type: 'array',
                        items: { type: 'integer', minimum: 0, maximum: 65535 }
                    },
                    a2: {
                        type: 'array',
                        items: { type: 'boolean' },
                        maxItems: 10
                    },
                    a3: {
                        type: 'array',
                        items: { type: 'number' },
                        minItems: 10,
                        maxItems: 10
                    },
                    a4: {
                        type: 'array',
                        items: { $ref: '#/$defs/MyObj1' },
                        minItems: 2,
                        maxItems: 10
                    }
                },
                additionalProperties: false,
                required: ['b','s','i8','i16a','i16b','i16','u16','u32','f','o','a1','a2','a3','a4']
            },
            MyEnum1: {
                enum: ['a', 'b', 'c']
            }
        }
    };
    expect(new GlobalModel().isValidMySchema(interfaceSpec, undefined)).toBeTruthy();

    const model = new GlobalModel();
    model.addValidSchema(interfaceSpec);

    const MyObj1 = model.getJsonStruct('http://example.com/schemas/defs.json#/$defs/MyObj1');
    expect(Object.entries(MyObj1.properties).length).toBe(3);
    const MyObj2 = model.getJsonStruct('http://example.com/schemas/defs.json#/$defs/MyObj2');
    expect(Object.entries(MyObj2.properties).length).toBe(14);
    const MyEnum1 = model.getJsonEnum('http://example.com/schemas/defs.json#/$defs/MyEnum1');
    expect(MyEnum1.enum.length).toBe(3);
    expect(MyEnum1.enum).toStrictEqual(
        expect.arrayContaining(['a', 'b', 'c'])
    );
});
