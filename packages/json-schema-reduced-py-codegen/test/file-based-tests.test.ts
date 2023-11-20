/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { generatePyCode } from '../src/codegen';
import * as path from 'path';
import * as fs from 'fs';
import * as glob from 'glob';

const schemaFiles = await glob.sync(path.join(__dirname, 'files', 'model', '**', '*.json'));
const refFiles = await glob.sync(path.join(__dirname, 'files', 'reference', '**', '*.*'));

const srcGenPath = path.join(__dirname, 'files', 'reference');
const [context,] = generatePyCode(schemaFiles , {destination: srcGenPath});

context.generators.forEach(g=>{
    test(`file based test: ${g.filename}`, () => {
        const expectedCode = fs.readFileSync(path.join(srcGenPath, g.filename)).toString();
        expect(expectedCode.length).not.toBe(0);
        expect(g.generator()).toBe(expectedCode);
    });
});

test(`file based test: check generated files, ${context.generators.length}`, () => {
    expect(refFiles.length).toBe(context.generators.length);
});
