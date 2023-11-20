# json-schema-reduced-cpp-codegen

## Responsibility

* Generate C++ code from JSON Schema files (as defined by [json-schema-reduced](./../json-schema-reduced/)).

## Architecture

* [cli.ts](./src/cli.ts): Entry point (command line code generator)
* [codegen.ts](./src/codegen.ts): Decide for what elements (e.g. Enums) we need to generate a source code file
  * Define the file name of the generated file.
  * Define which function is called to generate the program code.
* [enum.ts](./src/enum.ts) and [struct.ts](./src/struct.ts): functions to generate code for enums/structs.
* [language-support-cpp.ts](./src/language-support-cpp.ts): helper functions to, e.g., determine the C++ name of a type.

## Tests

The main test [file-based-tests.test.ts](./test/file-based-tests.test.ts) compares the generated code with reference code.

Once the code output changes, the reference can be updated (`npm run update-reference`). The "end user" can then review the changes in the reference.