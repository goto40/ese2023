# Compile time reflection for different message frameworks in C++ Embedded Systems

## Project Layout

* Root: master workspace/project (see [`package.json`](./package.json))
  * [`.devcontainer`](.devcontainer): dependencies
  * packages
    * [`json-schema-reduced`](./packages/json-schema-reduced/): the (reduced) JSON Schema parser used by the generators
    * [`json-schema-reduced-cpp-codegen`](./packages/json-schema-reduced-cpp-codegen/): the JSON Schema C++ code generator
    * [`json-schema-reduced-py-codegen`](./packages/json-schema-reduced-py-codegen/): the JSON Schema Python code generator
  * [`cpp-library`](./cpp-library): the C++ library with examples
    * [.](./cpp-library/) library, examples and tests using [`json-schema-reduced-cpp-codegen`](./packages/json-schema-reduced-cpp-codegen/) and the proposed compile time reflection.
      * library using the generated compile time reflection (I/O, converting, printing).
      * unittests for the library
      * examples
    * [examples/fastdds](./cpp-library/examples/fastdds/) Fast-DDS examples and tests using the proposed compile time reflection library.
      * DDS examples
      * demonstration how other message systems (here: [OMG IDL](https://www.omg.org/spec/IDL/4.2/PDF)) can be "plugged" into our library with a small code generation extracting meta information for the compile time reflection API.
    * [examples/mqtt](./cpp-library/examples/mqtt/) MQTT example code
      * MQTT examples
  * [`python-library`](./python-library): the Python library with examples
  * [`idl-converter`](./idl-converter/): a simple script (limited functionality; not well tested) to convert OMG DDS IDL files to C++ compiletime reflection code ("meta code").
    * This is a "hack" based on the ridl-test code
    * the main code in in [`writer.rb`](./idl-converter/ridlbe/idl2meta/writer.rb) and is adapted from the corresponding [ridl test code](https://github.com/RemedyIT/ridl/blob/v2.9.0/ridlbe/test/writer.rb). The [ridl-license (MIT)](./idl-converter/ridlbe/ridl-LICENSE) is included.

## To work with the project

Open everything in vscode + dev-container

* Dependencies are installed (see Dockerfile)
* The JSON Schema codegenerators are built and tested in the loop (see shells; see [`tasks.json`](.vscode/tasks.json)).

## Prerequisites

See [Dockerfile](./.devcontainer/Dockerfile):

* [Node](https://nodejs.org/en) >= 18 for the toolset around the "reduced JSON Schema".
* [Fast-DDS](https://github.com/eProsima/Fast-DDS) for the C++ examples using DDS.
* [Mosquitto Broker](https://mosquitto.org/) and [Paho (C++)](https://github.com/eclipse/paho.mqtt.cpp) for the MQTT examples.
* C++ Compiler, Cmake etc. (details see [Dockerfile](./.devcontainer/Dockerfile)).

## Quick test (build + run tests)

In the root folder (a workspace project with two sub-projects).

```bash
npm i
npm run build
npm run test
```

## Contributing

### Code Formatting

The project uses [clang-format](https://releases.llvm.org/14.0.0/tools/clang/docs/ClangFormat.html) for (C/C++) code formatting.

The formatting shall be checked before commiting, for this we use [pre-commit](https://pre-commit.com/). The pre-commit-hook must be installed after the initial code checkout:
* If you run vscode + dev-container this is done automatically.
* If you don't use vscode you have to do it manually by running the following command in the project's root directory

```bash
pre-commit install
```

It is also best practice to configure your editor to run clang-format on saving files. Again for vscode this is already setup.
If you want to format the code from command line run

```bash
pre-commit run --all-file
```

## Features

### Common compile time C++ reflection API

* We define how a common [compile time reflection API](./cpp-library/src/meta/common_reflection.h) looks like:
* With the API
  * we can visit all attributes (Visitor Pattern): [see example `print.h`](./cpp-library/src/meta/print.h). Here, we iterate over all attributes at compile time.
  * we can access data by name: [see example](./cpp-library/test/accessAttributeByName.cpp).

#### Relation to the C++ reflection TS

The [Reflection TS](https://github.com/cplusplus/reflection-ts) defines [reflexpr](https://en.cppreference.com/w/cpp/keyword/reflexpr) to get compile time reflection information of C++ APIs. Some aspects might be of relevance:

1. **Syntax of the reflection TS**: We could harmonize our API to mimic aspects of the reflection TS API. However, we aim to enhance our reflection API with more model information from the data model (schema file), like mininum/maximum values. This is clearly an extension, which will differ from the proposed reflection TS API. Our contribution is a proof of concept and a platform to exchange ideas we developed in-house (with other data models) - and thus we do not aim to mimic the reflection TS API at the current point of this project.

2. **Usage of the reflection TS instead of our generated code**: This is clearly an aspect we might consider. E.g., we could allow to convert data from our data models to a non-modelled struct (using the built-in C++ reflection API defined in the reflection TS). This might be an extension of our work.

### Basic meta tools

[`meta.h`](./cpp-library/src/meta/meta.h) provides basic tool functions to handle meta information of structures/messages.

- basic visitor support (end-user visitor wrapper class and accept method)
  - `BasicVisitor` (see [example](./cpp-library/src/meta/print.h))
  - `accept` (same example as for `BasicVisitor`)
- access meta attribute information by name and index
  - `MetaByIndex` (see `experiment_with_meta_info1` in [unittest](./cpp-library/test/convert.cpp))
  - `MetaByName` (see `experiment_with_meta_info1` in [unittest](./cpp-library/test/convert.cpp))
  - `hasMetaByName` (see `experiment_with_meta_info1` in [unittest](./cpp-library/test/convert.cpp))
  - `accessByName` (see [unittest](./cpp-library/test/accessAttributeByName.cpp))
- get the number of attributes
  - `numEl`: yields the number attributes in a message (used internally, e.g. in [`meta.h`](./cpp-library/src/meta/meta.h))


### Convert function

Using the common reflection API we provide a convert function, which allows converting one structure into another. For fields where no match is present in the source, a custom function must be provided. For any field such a function can be provided in order to customize the copy behavior.

#### Example for the compile time reflection convert function

The basic functionality is shown in [`test/convert.cpp`](./cpp-library/test/convert.cpp):
```
  int64_t runtime_value = 42;
  meta::CustomConverter converter{                       // decide at runtime how V-------------------------------V--------------------V
      meta::createCustomSingleConverter<example_com_schemas_examples::MyObj1, "i">([&](const auto&, auto& dest){ dest=runtime_value; }),
      meta::createCustomSingleConverter<example_com_schemas_examples::MyObj1, "s">([](const auto&, auto& dest){ dest="text"; })
  };     // decide at compile time what ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  ^^^

  // ...

  meta::convert_from_other_message(o2, o1, converter); // copy o2 to o1 with the converter

```

#### Example and assembly analysis for the compile time reflection

In the following, we "convert" a struct `a` into `b` (same type) - basically we just copy the struct. We analyze the assembly output, in order to check, that no overhead is generated by our custom convert function (`g++ -std=c++20 -S -O2 test/convert_helper.cpp -I src-gen -I src/meta/include/`).

* We copy the struct by "hand" (no compile time reflection; just manual code):
  ```c++
  void simple_copy(const example_com_schemas_examples::Simple& a, example_com_schemas_examples::Simple& b) {
      b.j = a.j;
      b.i = a.i;
      b.f = a.f;
  }
  ```
  and assembly
  ```asm
  _Z11simple_copyRKN28example_com_schemas_examples6SimpleERS0_:
  .LFB3829:
    .cfi_startproc
    endbr64
    movq  (%rdi), %rax
    movsd 16(%rdi), %xmm0
    movq  %rax, (%rsi)
    movq  8(%rdi), %rax
    movsd %xmm0, 16(%rsi)
    movq  %rax, 8(%rsi)
    ret
    .cfi_endproc
  ```

* We copy the struct with our compile time reflection API:
  ```c++
  void convert_copy(const example_com_schemas_examples::Simple& a, example_com_schemas_examples::Simple& b) {
      meta::convert_from_other_message(a, b);
  }
  ```
  and assembly
  ```asm
  _Z12convert_copyRKN28example_com_schemas_examples6SimpleERS0_:
  .LFB3830:
    .cfi_startproc
    endbr64
    movq  (%rdi), %rax
    movsd 16(%rdi), %xmm0
    movq  %rax, (%rsi)
    movq  8(%rdi), %rax
    movsd %xmm0, 16(%rsi)
    movq  %rax, 8(%rsi)
    ret
    .cfi_endproc
  ```

* We copy the struct with our compile time reflection API in a customized way:
  ```c++
  void convert_copy_but_set_i_to_0(const example_com_schemas_examples::Simple& a, example_com_schemas_examples::Simple& b) {
      meta::CustomConverter converter{
          meta::createCustomSingleConverter<example_com_schemas_examples::Simple, "i">([&](const auto&, auto& dest){ dest = 0; }),
      };
      meta::convert_from_other_message(a, b, converter);
  }
  ```
  and assembly
  ```asm
  _Z27convert_copy_but_set_i_to_0RKN28example_com_schemas_examples6SimpleERS0_:
  .LFB3831:
    .cfi_startproc
    endbr64
    movq  (%rdi), %rax
    movsd 16(%rdi), %xmm0
    movq  $0, 8(%rsi)
    movq  %rax, (%rsi)
    movsd %xmm0, 16(%rsi)
    ret
    .cfi_endproc
  ```

### Support of OMG DDS IDL models

Our central model is a "(reduced) JSON Schema" (see [schema file](./packages/json-schema-reduced/src/MySchema.json) of our reduced json schema), with concrete data schema (data models) like


```json
{
    "$id": "http://example.com/schemas/examples.json",
    "$defs": {
...
        "Simple": {
          "type": "object",
          "properties": {
            "j": {"type": "integer"},
            "i": {"type": "integer"},
            "f": {"type": "number"}
          },
          "additionalProperties": false,
          "required": ["j","i","f"]
        },
...
```

From here, we derive a struct and meta code:

```c++
struct Simple {
    int64_t j; // attribute j
    int64_t i; // attribute i
    double f; // attribute f
};
```
```c++
namespace meta {
    struct Simple {
        static constexpr const char* name() { return "Simple"; }
        struct attributes {
            struct j {
                using STRUCT = example_com_schemas_examples::Simple;
                using ATTR_TYPE = int64_t;
                static constexpr const char* name() { return "j"; }
                static constexpr std::array<const char,2> ct_name() { return {"j"}; }
                static constexpr auto& get(STRUCT &s) { return s.j; }
                static constexpr const auto& get(const STRUCT &s) { return s.j; }
            };
            struct i {
//...
            };
            struct f {
//...
            };
        };

        template<class ...T> struct List;
        using ListOfAttributeMetaClasses = List<
            attributes::j,
            attributes::i,
            attributes::f
        >;
    };
}
//...
```

For [OMG DDS IDL](https://www.omg.org/spec/CPP11/1.1/PDF/) models we get [C++11 code as defined bythe OMG](https://www.omg.org/spec/CPP11/1.1/PDF/) (note: anonymous IDL types have been deprecated for the C++11 output, see section 6.2 in the OMG document).

The IDL model for a struct corresonding to the one shown above is:

```c++
module dds_examples {
  struct SimpleT
  {
      long j;
      long i;
      double f;
  };
};
```

The C++ 11 code is derived from the model using off-the-shelf software (here with the [eProsima Fast-DDS](https://github.com/eProsima/Fast-DDS) tooling):

```c++
namespace dds_examples {
    class SimpleT
    {
    public:
        // ...

        eProsima_user_DllExport void j(int32_t _j);
        eProsima_user_DllExport int32_t j() const;
        eProsima_user_DllExport int32_t& j();

        eProsima_user_DllExport void i(int32_t _i);
        eProsima_user_DllExport int32_t i() const;
        eProsima_user_DllExport int32_t& i();

        eProsima_user_DllExport void f(double _f);
        eProsima_user_DllExport double f() const;
        eProsima_user_DllExport double& f();

        // ...
    }
}
```

We then use an [open source IDL parser (ridl)](https://github.com/RemedyIT/ridl) to derive the required minimal information to process the IDL DDS code with our compile-time reflection API:

```c++
namespace dds_examples {
  namespace meta {
    struct SimpleT {
      static constexpr const char* name() { return "SimpleT"; }
      struct attributes {
        struct j {
          using STRUCT = dds_examples::SimpleT;
          using ATTR_TYPE = std::remove_reference_t<decltype(std::declval<dds_examples::SimpleT>().j())>;
          static constexpr const char* name() { return "j"; }
          static constexpr std::array<const char, 2> ct_name() { return {"j"}; }
          static auto& get(STRUCT& s) { return s.j(); }
          static auto get(const STRUCT& s) { return s.j(); }
        }; // struct j
        struct i {
          // ...
        }; // struct i
        struct f {
          // ...
        }; // struct f
      }; // struct attributes
      template <class... T>
      struct List;
      using ListOfAttributeMetaClasses =
        List<attributes::j,attributes::i,attributes::f>;
    }; // struct SimpleT
  } // namespace meta
```

Using this code we can then call our library with DDS code (see [unittests](./cpp-library/examples/fastdds/test/fastdds.cpp)). In the following example we print an IDL C++ object with our library using the generated compile time reflection:

```c++
dds_examples::HelloWorldT data;
init(data);

std::ostringstream output;
meta::print(data, output);

CHECK(output.str() == R"(HelloWorldT:
  id=99
  header=
    PayloadT:
      id=123
      color=RED
  index=10
  message="Hello ESE"
  color=BLUE
  colors=[ RED GREEN BLUE RED ]
  values=[ 11 22 33 ]
  arrayOfStructs=[
    PayloadT:
      id=1
      color=RED
    PayloadT:
      id=2
      color=GREEN
  ]
)");
```

We can also convert between IDL objects and our own representation using our convert function depicted above (see [unittests](./cpp-library/examples/fastdds/test/fastdds.cpp)):

```c++
dds_examples::SimpleT simple_dds;
example_com_schemas_examples::Simple simple_internal;

simple_internal.i = 10;
simple_internal.j = 11;
simple_internal.f = 1.23;

meta::convert_from_other_message(simple_internal, simple_dds);

CHECK(simple_dds.i() == 10);
CHECK(simple_dds.j() == 11);
CHECK(simple_dds.f() == 1.23);
```

## Extensions

* Google Protocol Buffers / gRPC
* OMG IDL / DDS
