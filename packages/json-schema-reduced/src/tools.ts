/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { isJsonArrayType, isJsonRef, JsonArrayType, JsonArrayTypeWithMaxItems, JsonRef, JsonRawType, JsonStruct } from './schema';

export function getInformalType(t: JsonRef|JsonRawType|JsonArrayType): string {
    if (isJsonRef(t)) {
        return `struct ${t.$ref}`;
    }
    else if (isJsonArrayType(t)) {
        const dim = isFixedSizedArray(t)?
            `${t.maxItems}`
            :(isBoundedSizeArray(t)?
                `..${t.maxItems}`
                :''
            );
        return `array [${dim}] of ${getInformalType(t.items)}`;
    }
    else {
        const bits = (t.type==='integer'||t.type==='number')?` with ${getTypeBitWidth(t)} bits`:'';
        return `type ${t.type}${bits}`;
    }
}

export function isFixedSizedArray(t: JsonArrayType): t is JsonArrayTypeWithMaxItems {
    return (t.maxItems!==undefined) && (t.minItems===t.maxItems);
}

export function isBoundedSizeArray(t: JsonArrayType): t is JsonArrayTypeWithMaxItems {
    return (t.maxItems!==undefined);
}

export function isTypeUnsigned(t: JsonRawType) {
    return (t.minimum!==undefined && t.minimum>=0);
}
export function isTypeSigned(t: JsonRawType) {
    return !isTypeUnsigned(t);
}

export function getTypeBitWidth(t: JsonRawType): number|undefined {
    if (t.type==='boolean') return 1;
    else if (t.type==='number') return 64;
    else if (t.type==='integer') {
        if (isTypeSigned(t)) {
            if (t.maximum!==undefined && t.minimum!==undefined) {
                if (t.maximum <= 2**8/2-1 && t.minimum >= -(2**8)/2) return 8;
                if (t.maximum <= 2**16/2-1 && t.minimum >= -(2**16)/2) return 16;
                if (t.maximum <= 2**32/2-1 && t.minimum >= -(2**32)/2) return 32;
                else return 64;
            }
            else {
                return 64;
            }
        }
        else {
            if (t.maximum!==undefined) {
                if (t.maximum < 2**8) return 8;
                else if (t.maximum < 2**16) return 16;
                else if (t.maximum < 2**32) return 32;
                else return 64;
            }
            else {
                return 64; // max!
            }
        }
    }
    else return undefined;
}

export function getIsRequired(struct: JsonStruct, attrName: string) {
    return struct.required && struct.required.includes(attrName);
}
