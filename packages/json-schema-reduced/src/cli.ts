/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { Command } from 'commander';
import { version, name as appName } from '../package.json';
import { GlobalModel, isJsonEnum, isJsonStruct } from './schema';
import chalk from 'chalk';
import { getInformalType, getIsRequired } from './tools';

function processSchema(filenames: string[]): void {
    const model = new GlobalModel();
    const errors = new Array<string>();
    if (model.loadModels(filenames, errors)) {
        model.schemas.forEach((mySchema, id)=>{
            Object.keys(mySchema.$defs).forEach(name =>{
                const structOrEnum = mySchema.$defs[name];
                //console.log(`${name}: ${JSON.stringify(definition)}`);
                if (isJsonStruct(structOrEnum)) {
                    const struct = structOrEnum;
                    console.log(`struct ${id} / ${name}:`);
                    Object.keys(struct.properties).forEach(attrName =>{
                        const t = struct.properties[attrName];
                        console.log(` -  ${attrName}: ${getIsRequired(struct, attrName)?'required':'optional'} ${getInformalType(t)}`);
                    });
                }
                else if (isJsonEnum(structOrEnum)) {
                    const enumObj = structOrEnum;
                    console.log(`enum ${name}: ${enumObj.enum.join(',')}`);
                }
            });
        });
    }
    else {
        console.error(chalk.redBright(`error loading ${filenames.join(',')}`));
        errors.forEach(error=>{
            console.error(chalk.red(`${error}`));
        });
    }
}

const program = new Command();
program
    .version(version)
    .description(`${appName}`)
    .argument('<files...>', 'models to process')
    .action(processSchema)
    .parse(process.argv);
