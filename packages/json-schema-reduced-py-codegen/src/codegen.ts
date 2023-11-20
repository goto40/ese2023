/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { loadModel, getAllStructs, GeneratorContext, OutputString, GlobalModel, getAllEnums } from '@ese/json-schema-reduced';
import { GeneratorOptions } from './cli';
import { generateCppEnumCode, getPyEnumFilename } from './enum';
import { generateCppStructCode, getPyStructFilename } from './struct';

export function generatePyCode(filenames: string[]|object[], options: GeneratorOptions): [GeneratorContext, GlobalModel] {
    const model = loadModel(filenames);
    const context = new GeneratorContext();
    context.destination = options.destination;
    for (const struct of getAllStructs(model)) {
        context.push({
            filename: getPyStructFilename(struct),
            generator: () => {
                const out = new OutputString();
                generateCppStructCode(out, struct);
                return out.getText();
            }
        });
    }
    for (const enumObj of getAllEnums(model)) {
        context.push({
            filename: getPyEnumFilename(enumObj),
            generator: () => {
                const out = new OutputString();
                generateCppEnumCode(out, enumObj);
                return out.getText();
            }
        });
    }
    return [context, model];
}
