/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { GlobalModel } from '../src/schema';

test('validate min>max, no error', () => {
    const interfaceSpecOk = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: {type: 'integer', minimum:10, maximum:10},
                    f: {type: 'number', minimum:10, maximum:10}
                },
                additionalProperties: false,
                required: ['f','i']
            }
        }
    };
    expect(new GlobalModel().isValidMySchema(interfaceSpecOk, undefined)).toBeTruthy();
});

test('validate min>max, error in scalar1', () => {
    const interfaceSpecBad = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: {type: 'integer', minimum:11, maximum:10},
                    f: {type: 'number', minimum:10, maximum:10}
                },
                additionalProperties: false,
                required: ['f','i']
            }
        }
    };
    const errors = new Array<string>();
    expect(new GlobalModel().isValidMySchema(interfaceSpecBad, undefined, errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([expect.stringMatching(/minimum.*>.*maximum/)]));
});

test('validate min>max, error in scalar2', () => {
    const interfaceSpecBad = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: {type: 'integer', minimum:10, maximum:10},
                    f: {type: 'number', minimum:11, maximum:10}
                },
                additionalProperties: false,
                required: ['f','i']
            }
        }
    };
    const errors = new Array<string>();
    expect(new GlobalModel().isValidMySchema(interfaceSpecBad, undefined, errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([expect.stringMatching(/minimum.*>.*maximum/)]));
});

test('validate min>max, error in array', () => {
    const interfaceSpecBad = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: { type: 'array', items: {type: 'integer', minimum:10, maximum:10}},
                    f: {type: 'number', minimum:11, maximum:10}
                },
                additionalProperties: false,
                required: ['f','i']
            }
        }
    };
    const errors = new Array<string>();
    expect(new GlobalModel().isValidMySchema(interfaceSpecBad, undefined, errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([expect.stringMatching(/minimum.*>.*maximum/)]));
});

test('validate ref, no error', () => {
    const interfaceSpecOk = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: {type: 'integer', minimum:10, maximum:10},
                    f: {type: 'number', minimum:10, maximum:10}
                },
                additionalProperties: false,
                required: ['f','i']
            },
            MyObj2: {
                type: 'object',
                properties: {
                    r1: { $ref: '#/$defs/MyObj1'},
                    r2: { $ref: 'http://example.com/schemas/defs.json#/$defs/MyObj1'}
                },
                additionalProperties: false,
                required: ['r1','r2']
            }
        }
    };
    expect(new GlobalModel().isValidMySchema(interfaceSpecOk, undefined)).toBeTruthy();
});

test('validate ref, error1', () => {
    const interfaceSpecOk = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: {type: 'integer', minimum:10, maximum:10},
                    f: {type: 'number', minimum:10, maximum:10}
                },
                additionalProperties: false,
                required: ['f','i']
            },
            MyObj2: {
                type: 'object',
                properties: {
                    r1: { $ref: '#/$defs/MyObj3'},
                    r2: { $ref: 'http://example.com/schemas/defs.json#/$defs/MyObj1'}
                },
                additionalProperties: false,
                required: ['r1','r2']
            }
        }
    };
    const errors = new Array<string>();
    expect(new GlobalModel().isValidMySchema(interfaceSpecOk, undefined, errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([expect.stringMatching(/MyObj3 not present/)]));
});

test('validate ref, error2', () => {
    const interfaceSpecOk = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: {type: 'integer', minimum:10, maximum:10},
                    f: {type: 'number', minimum:10, maximum:10}
                },
                additionalProperties: false,
                required: ['f','i']
            },
            MyObj2: {
                type: 'object',
                properties: {
                    r1: { $ref: '#/definition/MyObj1'},
                    r2: { $ref: 'http://example.com/schemas/defs.json#/$defs/MyObj1'}
                },
                additionalProperties: false,
                required: ['r1','r2']
            }
        }
    };
    const errors = new Array<string>();
    expect(new GlobalModel().isValidMySchema(interfaceSpecOk, undefined, errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([expect.stringMatching(/must start with '\/\$defs\/'/)]));
});

test('validate ref, error3', () => {
    const interfaceSpecOk = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: {type: 'integer', minimum:10, maximum:10},
                    f: {type: 'number', minimum:10, maximum:10}
                },
                additionalProperties: false,
                required: ['f','i']
            },
            MyObj2: {
                type: 'object',
                properties: {
                    r1: { $ref: '#/$defs/MyObj1'},
                    r2: { $ref: 'http://wrong.example.com/schemas/defs.json#/$defs/MyObj1'}
                },
                additionalProperties: false,
                required: ['r1','r2']
            }
        }
    };
    const errors = new Array<string>();
    expect(new GlobalModel().isValidMySchema(interfaceSpecOk, undefined, errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([expect.stringMatching(/wrong\.example.*not found/)]));
});

test('validate ref, error4', () => {
    const interfaceSpecOk = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: {type: 'integer', minimum:10, maximum:10},
                    f: {type: 'number', minimum:10, maximum:10}
                },
                additionalProperties: false,
                required: ['s','i']
            },
            MyObj2: {
                type: 'object',
                properties: {
                    r1: { $ref: '#/$defs/MyObj1'},
                    r2: { $ref: 'http://example.com/schemas/defs.json#/$defs/MyObj4'}
                },
                additionalProperties: false,
                required: ['r1','r2']
            }
        }
    };
    const errors = new Array<string>();
    expect(new GlobalModel().isValidMySchema(interfaceSpecOk, undefined, errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([expect.stringMatching(/MyObj4 not present/)]));
});

test('validate minMaxItems, no errors in array', () => {
    const interfaceSpecOk = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: { type: 'array', items: {type: 'integer'}, minItems:0, maxItems:1},
                    f: {type: 'number'}
                },
                additionalProperties: false,
                required: ['f','i']
            }
        }
    };
    expect(new GlobalModel().isValidMySchema(interfaceSpecOk, undefined)).toBeTruthy();
});

test('validate minMaxItems, errors in array 1', () => {
    const interfaceSpecBad = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: { type: 'array', items: {type: 'integer'}, minItems:-1, maxItems:1},
                    f: {type: 'number'}
                },
                additionalProperties: false,
                required: ['f','i']
            }
        }
    };
    const errors = new Array<string>();
    expect(new GlobalModel().isValidMySchema(interfaceSpecBad, undefined, errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([expect.stringMatching(/minItems.*must be >=0/)]));
});

test('validate minMaxItems, errors in array 2', () => {
    const interfaceSpecBad = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: { type: 'array', items: {type: 'integer'}, minItems:0, maxItems: 0},
                    f: {type: 'number'}
                },
                additionalProperties: false,
                required: ['f','i']
            }
        }
    };
    const errors = new Array<string>();
    expect(new GlobalModel().isValidMySchema(interfaceSpecBad, undefined, errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([expect.stringMatching(/maxItems.*must be >=1/)]));
});

test('validate minMaxItems, errors in array 3', () => {
    const interfaceSpecBad = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: { type: 'array', items: {type: 'integer'}, minItems:10, maxItems: 9},
                    f: {type: 'number'}
                },
                additionalProperties: false,
                required: ['f','i']
            }
        }
    };
    const errors = new Array<string>();
    expect(new GlobalModel().isValidMySchema(interfaceSpecBad, undefined, errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([expect.stringMatching(/minItems.*>.*maxItems/)]));
});

test('validate required: no valid entry', () => {
    const interfaceSpecBad = {
        $id: 'http://example.com/schemas/defs.json',
        $defs: {
            MyObj1: {
                type: 'object',
                properties: {
                    i: { type: 'array', items: {type: 'integer'}, minItems:10, maxItems: 9},
                    f: {type: 'number'}
                },
                additionalProperties: false,
                required: ['unknown','i']
            }
        }
    };
    const errors = new Array<string>();
    expect(new GlobalModel().isValidMySchema(interfaceSpecBad, undefined, errors)).toBeFalsy();
    expect(errors).toStrictEqual(expect.arrayContaining([expect.stringMatching(/required.*unknown.*not a valid field/)]));
});
