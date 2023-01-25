//
// Created by siebe on 25/01/2023.
//

#include "../lib/doctest/doctest.h"
#include <fstream>

#include "../src/objects/JsonSchema.h"

using namespace std;

TEST_SUITE("JsonSchemaTests") {
    TEST_CASE("[JsonSchemaTests] test0") {

        string path1 = "../tests/input/input-json_schema1.json";
        string path2 = "../tests/input/input-json_example1.json";

        JsonSchema schema(path1);
        CHECK(schema.validate(path2));
    }
}