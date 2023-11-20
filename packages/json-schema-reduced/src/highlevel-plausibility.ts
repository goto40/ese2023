/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { getAllTypesWithRef, isStruct, Struct } from './model';
import { GlobalModel } from './schema';

export function highLevelPlausibility(model: GlobalModel, errors: string[]): boolean {
    let result = true;
    result = checkForRecursion(model, errors) && result;
    return result;
}

function findRecursion(struct: Struct, recursion: Set<Struct>): boolean {
    if (recursion.has(struct)) return true;
    recursion.add(struct);

    for(const t of getAllTypesWithRef(struct)) {
        if (isStruct(t.type) && findRecursion(t.type, recursion)) { return true; }
    }

    recursion.delete(struct);
    return false;
}

function checkForRecursion(model: GlobalModel, errors: string[]): boolean {
    let result = true;
    model.namespaces.forEach((namespace, id)=>{
        namespace.elements.filter(isStruct).forEach(struct=>{
            const recursion = new Set<Struct>();
            if (findRecursion(struct,recursion)) {
                result = false;
                errors.push(`recursion detected: ${id}::${struct.name} ${Array.from(recursion.values()).map(s=>s.name).join(',')}`);
            }
        });
    });
    return result;
}
