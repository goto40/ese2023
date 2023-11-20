# json-schema-reduced

## Responsibility

* [Load and parse JSON Schema files](./test/basic.test.ts) with [basic validation](./test/validation-test.test.ts) (e.g. [no recursive references](./test/recursion-check.test.ts)).
* [Define a "high-level" model to query a JSON Schema definition](./src/model.ts)
* [Provide helper classes to control code generation](./test/generator.test.ts)
