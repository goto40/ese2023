/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include "convert_helper.h"

#include "meta/convert.h"

/*
Assembly analysis:
    - from the project root (cpp-library):
    - `g++ -std=c++20 -S -O2 test/convert_helper.cpp -I src-gen/ -I src`
    - inspect `less ./convert_helper.s`
    - remove file `rm ./convert_helper.s`
*/

void simple_copy(const example_com_schemas_examples::Simple& a,
                 example_com_schemas_examples::Simple& b) {
  b.j = a.j;
  b.i = a.i;
  b.f = a.f;
}
/*
_Z11simple_copyRKN28example_com_schemas_examples6SimpleERS0_:
.LFB3829:
        .cfi_startproc
        endbr64
        movq	(%rdi), %rax
        movsd	16(%rdi), %xmm0
        movq	%rax, (%rsi)
        movq	8(%rdi), %rax
        movsd	%xmm0, 16(%rsi)
        movq	%rax, 8(%rsi)
        ret
        .cfi_endproc
*/

void convert_copy(const example_com_schemas_examples::Simple& a,
                  example_com_schemas_examples::Simple& b) {
  meta::convert_from_other_message(a, b);
}

/*
_Z12convert_copyRKN28example_com_schemas_examples6SimpleERS0_:
.LFB3830:
        .cfi_startproc
        endbr64
        movq	(%rdi), %rax
        movsd	16(%rdi), %xmm0
        movq	%rax, (%rsi)
        movq	8(%rdi), %rax
        movsd	%xmm0, 16(%rsi)
        movq	%rax, 8(%rsi)
        ret
        .cfi_endproc
*/

void convert_copy_but_set_i_to_0(const example_com_schemas_examples::Simple& a,
                                 example_com_schemas_examples::Simple& b) {
  meta::CustomStructConverter converter{
      meta::createCustomAttributeConverter<example_com_schemas_examples::Simple,
                                           "i">(
          [&](const auto&, auto& dest) { dest = 0; }),
  };
  meta::convert_from_other_message(a, b, converter);
}
/*
_Z27convert_copy_but_set_i_to_0RKN28example_com_schemas_examples6SimpleERS0_:
.LFB3831:
        .cfi_startproc
        endbr64
        movq	(%rdi), %rax
        movsd	16(%rdi), %xmm0
        movq	$0, 8(%rsi)
        movq	%rax, (%rsi)
        movsd	%xmm0, 16(%rsi)
        ret
        .cfi_endproc
*/
