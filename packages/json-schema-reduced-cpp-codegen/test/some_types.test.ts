/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { generateCppCode } from '../src/codegen';
import { dedent } from 'ts-dedent';

test('simple codegen', () => {
    const interfaceSpec = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    s: {type: 'string'},
                },
                additionalProperties: false,
                required: ['s']
            },
            MyObj2: {
                type: 'object',
                properties: {
                    b: {type: 'boolean'},
                    s: {type: 'string'},
                    i8: {type: 'integer', minimum: -128, maximum: 127},
                    i16a: {type: 'integer', minimum: -129, maximum: 127},
                    i16b: {type: 'integer', minimum: -128, maximum: 128},
                    i16c: {type: 'integer', minimum: -12, maximum: 5000},
                    u16: {type: 'integer', minimum: 12, maximum: 5000},
                    i32: {type: 'integer', minimum: -2147483648, maximum: 2147483647},
                    u32: {type: 'integer', minimum: 0, maximum: 4294967295},
                    i64a: {type: 'integer', minimum: -1, maximum: 4294967295},
                    u64b: {type: 'integer', minimum: 0, maximum: 4294967296},
                    i64c: {type: 'integer', minimum: -2147483649, maximum: 2147483647},
                    i64d: {type: 'integer', minimum: -2147483648, maximum: 2147483648},
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
                required: ['b','s','i8','i16a','i16b','i16c','u16','i32','u32','i64a','u64b','i64c','i64d','f','o','a1','a2','a3','a4']
            },
            MyEnum1: {
                enum: ['a', 'b', 'c']
            }
        }
    };

    const [context, model] = generateCppCode([interfaceSpec], {destination: 'src-gen'});
    const struct = model.get('http://example.com/schemas/defs.json#/$defs/MyObj2');
    expect(struct.name).toBe('MyObj2');

    const generator = context.generators.find(g=>g.filename.endsWith('MyObj2.h'));
    if (generator===undefined) throw new Error('unexpected');

    const code = generator.generator();
    const filteredAttributes = dedent('\n'+code.split('\n').filter(x=>x.match(/; \/\/ attribute/)).join('\n'));
    expect(filteredAttributes).toBe(dedent(`
        bool b; // attribute b
        std::string s; // attribute s
        int8_t i8; // attribute i8
        int16_t i16a; // attribute i16a
        int16_t i16b; // attribute i16b
        int16_t i16c; // attribute i16c
        uint16_t u16; // attribute u16
        int32_t i32; // attribute i32
        uint32_t u32; // attribute u32
        int64_t i64a; // attribute i64a
        uint64_t u64b; // attribute u64b
        int64_t i64c; // attribute i64c
        int64_t i64d; // attribute i64d
        double f; // attribute f
        example_com_schemas_defs::MyObj1 o; // attribute o
        std::vector<uint16_t> a1; // attribute a1
        std::vector<bool> a2; // attribute a2
        std::array<double,10> a3; // attribute a3
        std::vector<example_com_schemas_defs::MyObj1> a4; // attribute a4
    `));
});
