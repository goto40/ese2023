/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/

import { IndentString, NL, OutputString } from '../src/generator';

test('IndentString, simple1', () => {
    const text = new IndentString();
    text.appendRaw(0,'Hello');
    expect(text.text).toBe('Hello');
});

test('IndentString, simple2', () => {
    const text = new IndentString();
    text.appendRaw(2,'Hello\nWorld');
    expect(text.text).toBe('  Hello\n  World');
});

test('OutputString, simple1', () => {
    const text = new OutputString();
    text.append('Hello\nWorld',NL,NL);
    text.indent(text=>{
        text.append('Hello\nWorld',NL,NL);
    });
    text.append('Hello\nWorld');
    expect(text.getText()).toBe('Hello\nWorld\n\n    Hello\n    World\n\nHello\nWorld');
});
