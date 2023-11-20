/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { GlobalModel } from '../src/schema';

test('validate find recursion: 1-2-3-4-2', () => {
    const interfaceSpecOk = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    o: {$ref: '#/$defs/MyObj2'}
                },
                additionalProperties: false,
                required: ['o']
            },
            MyObj2: {
                type: 'object',
                properties: {
                    o: {$ref: '#/$defs/MyObj3'}
                },
                additionalProperties: false,
                required: ['o']
            },
            MyObj3: {
                type: 'object',
                properties: {
                    o: {$ref: '#/$defs/MyObj4'}
                },
                additionalProperties: false,
                required: ['o']
            },
            MyObj4: {
                type: 'object',
                properties: {
                    o: {$ref: '#/$defs/MyObj2'}
                },
                additionalProperties: false,
                required: ['o']
            }
        }
    };
    const model = new GlobalModel();
    const errors = new Array<string>();
    expect(model.loadModels([interfaceSpecOk], errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([
        expect.stringMatching(/recursion detected/)
    ]));
});

test('validate find recursion: 1-2-3-4-2 and arrays', () => {
    const interfaceSpecOk = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    o: {$ref: '#/$defs/MyObj2FromHere'}
                },
                additionalProperties: false,
                required: ['o']
            },
            MyObj2FromHere: {
                type: 'object',
                properties: {
                    o: {$ref: '#/$defs/MyObj3'}
                },
                additionalProperties: false,
                required: ['o']
            },
            MyObj3: {
                type: 'object',
                properties: {
                    o: {$ref: '#/$defs/MyObj4'}
                },
                additionalProperties: false,
                required: ['o']
            },
            MyObj4: {
                type: 'object',
                properties: {
                    o: { type: 'array', items: {$ref: '#/$defs/MyObj2FromHere'}}
                },
                additionalProperties: false,
                required: ['o']
            }
        }
    };
    const model = new GlobalModel();
    const errors = new Array<string>();
    expect(model.loadModels([interfaceSpecOk], errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([
        expect.stringMatching(/recursion detected/)
    ]));
});

test('validate find recursion: 1-1', () => {
    const interfaceSpecOk = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    o: {$ref: '#/$defs/MyObj1'}
                },
                additionalProperties: false,
                required: ['o']
            }
        }
    };
    const model = new GlobalModel();
    const errors = new Array<string>();
    expect(model.loadModels([interfaceSpecOk], errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([
        expect.stringMatching(/recursion detected/)
    ]));
});
