/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { Command } from 'commander';
import { version, name as appName } from '../package.json';
import { generateCppCode } from './codegen';

function processSchema(filenames: string[], options: GeneratorOptions): void {
    const [context,] = generateCppCode(filenames, options);
    context.generate();
}

export interface GeneratorOptions {
    destination: string;
}

const program = new Command();
program
    .version(version)
    .description(`${appName}`)
    .argument('<files...>', 'models to process')
    .option('-d, --destination <folder>', 'the destination folder', 'src-gen')
    .action(processSchema)
    .parse(process.argv);