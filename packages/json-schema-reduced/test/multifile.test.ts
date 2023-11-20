/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { GlobalModel } from '../src/schema';

test('simple multfile w/o errors', () => {
    const data = [
        {
            $id: 'http://example.com/schema1.json',
            $defs: {
                MyObj1: {
                    type: 'object',
                    properties: {
                        i: {type: 'integer', minimum:10, maximum:10},
                        o: {$ref: 'http://example.com/schema2.json#/$defs/MyObj2' }
                    },
                    additionalProperties: false,
                    required: ['i','o']
                }
            }
        },
        {
            $id: 'http://example.com/schema2.json',
            $defs: {
                MyObj2: {
                    type: 'object',
                    properties: {
                        i: {type: 'integer', minimum:10, maximum:10},
                    },
                    additionalProperties: false,
                    required: ['i']
                }
            }
        }
    ];

    const model = new GlobalModel();
    expect(model.loadModels(data)).toBeTruthy();
});

test('simple multfile with double files', () => {
    const data = [
        {
            $id: 'http://example.com/schema1.json',
            $defs: {
                MyObj1: {
                    type: 'object',
                    properties: {
                        i: {type: 'integer', minimum:10, maximum:10},
                    },
                    additionalProperties: false,
                    required: ['i']
                }
            }
        },
        {
            $id: 'http://example.com/schema1.json',
            $defs: {
                MyObj2: {
                    type: 'object',
                    properties: {
                        i: {type: 'integer', minimum:10, maximum:10},
                    },
                    additionalProperties: false,
                    required: ['i']
                }
            }
        }
    ];

    const model = new GlobalModel();
    const errors = new Array<string>();
    expect(model.loadModels(data, errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([
        expect.stringMatching(/schema1\.json .* already defined/)
    ]));
});
