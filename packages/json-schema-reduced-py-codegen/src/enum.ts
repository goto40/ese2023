/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { Enum, NL, OutputString } from '@ese/json-schema-reduced';
import { getPyNamespace } from './language-support-py';

export function getPyEnumFilename(enumObj: Enum): string {
    return getPyNamespace(enumObj.parent).replace(/::/g,'/')+'/'+enumObj.name+'.py';
}

export function generateCppEnumCode(out: OutputString, enumObj: Enum): void {
    out.append('from enum import Enum', NL);
    out.append(NL);
    out.append(`class ${enumObj.name}(Enum):`, NL);
    out.indent(out=>{
        enumObj.values.forEach((v,_index)=>{
            out.append(`${v} = '${v}'`, NL);
        });
    });
}
