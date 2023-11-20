/******************************************************************************
 * Copyright 2023 Pierre Bayerl and Michael Thoma
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 ******************************************************************************/
#include <iostream>

#include "commandline/CommandlineApp.h"
#include "meta/print.h"
#include "meta/program_options.h"

// ./examples/commandline --color red --n=8 --data 1.2 --data 9.7 --switches 1
// --switches 0 --switches 1

// Motivated from https://www.youtube.com/watch?v=JrOJ012XxNg, Minute 38:44
// (C++Now 2017: Jackie Kay “Practical (?) Applications of Reflection")

int main(int argc, const char** argv) {
  auto data =
      meta::from_program_options<commandline::CommandlineApp>(argc, argv);

  meta::print(data);
}
