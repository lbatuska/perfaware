#include <iostream>
#include <type_traits>

#include "include/json.h"

int main(int argc, char* argv[]) {
  std::string jsonstring{
      R"(
{
  "integer": 42,
  "negative_integer": -42,
  "double": 3.14159,
  "exponential": 1.23e4,
  "string": "Hello, world!",
  "escaped_string": "This is a \"quoted\" string with a newline\nand a tab\t.",
  "boolean_true": true,
  "boolean_false": false,
  "null_value": null,
  "array": [1, -2, 3.5, "four", true, null],
  "nested_object": {
    "nested_integer": 100,
    "nested_string": "Test nested object",
    "nested_array": [1, 2, 3]
  }
}
  )"};

  auto x = json::parse(std::move(jsonstring));

  std::cout << x.value() << " \n";

  return 0;
}
