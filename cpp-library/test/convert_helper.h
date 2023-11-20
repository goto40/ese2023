/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#pragma once
#include "example_com_schemas_examples/Simple.h"

void simple_copy(const example_com_schemas_examples::Simple& a,
                 example_com_schemas_examples::Simple& b);
void convert_copy(const example_com_schemas_examples::Simple& a,
                  example_com_schemas_examples::Simple& b);
void convert_copy_but_set_i_to_0(const example_com_schemas_examples::Simple& a,
                                 example_com_schemas_examples::Simple& b);
