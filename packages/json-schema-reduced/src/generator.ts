/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import * as path from 'path';
import * as fs from 'fs';
import chalk from 'chalk';
import { GlobalModel } from './schema';
import { dedent } from 'ts-dedent';
import indentString from 'indent-string';
export interface Generator {
    filename: string,
    generator: () => string;
}

export function loadModel(filenames: string[]|object[]): GlobalModel {
    try {
        const data = new GlobalModel();
        const errors = new Array<string>();
        if (data.loadModels(filenames, errors)) {
            return data;
        }
        else {
            console.error(chalk.redBright(`❌ errors loading ${filenames.join(',')}`));
            errors.forEach(e=>{
                console.error(chalk.red(`❌ ${e}`));
            });
            throw new Error('❌ error. stopping generation.');
        }
    }
    catch(e) {
        console.error(chalk.redBright(`❌ error loading models: ${e}`));
        throw new Error('❌ error. stopping generation.');
    }
}

export class GeneratorContext {
    generators = new Array<Generator>();
    destination = './src-gen';
    push(g: Generator) {
        this.generators.push(g);
    }
    generate() {
        let allOk = true;
        this.generators.forEach(g=>{
            const filename = path.join(this.destination, g.filename);
            try {
                const text = g.generator();
                fs.mkdirSync(path.dirname(filename), { recursive:true });
                fs.writeFileSync(filename, text);
                console.log(chalk.gray(`🔧 generated ${filename}`));
            }
            catch(e) {
                allOk = false;
                console.error(chalk.redBright(`❌ error generating ${filename}: ${e}`));
            }
        });
        if (allOk) console.log(chalk.greenBright('✅ code generation completed.'));
    }
}

export const NL = '\n';
export class IndentString {
    text = '';
    doDedent = true;
    appendRaw(indent: number, ...text: string[]) {
        text.forEach(t=>{
            if (this.doDedent && t!==NL) {
                t = indentString(dedent(t), indent);
                this.doDedent = false;
            }
            this.text += t;
            if (t.endsWith('\n')) {
                this.doDedent=true;
            }
        });
    }
}

export class OutputString {
    private text = new IndentString();
    private indentValue = 0;
    constructor(parent?: IndentString, indentValue?: number) {
        if (parent!==undefined) this.text = parent;
        if (indentValue!==undefined) this.indentValue = indentValue;
    }
    getText(): string {
        return this.text.text;
    }
    append(...text: string[]): void {
        this.text.appendRaw(this.indentValue, ...text);
    }

    indent( f: (x: OutputString) => void ): void {
        f( new OutputString(this.text, this.indentValue+4) );
    }
}
