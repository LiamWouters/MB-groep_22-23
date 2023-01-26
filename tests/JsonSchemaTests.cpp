#include "../lib/doctest/doctest.h"
#include <fstream>

#include "../src/objects/JsonSchema.h"

using namespace std;

TEST_SUITE("JsonSchemaTests") {
    TEST_CASE("[JsonSchemaTests] object_test01") {

        string path1 = "../tests/input/input-json_schema1.json";
        string path2 = "../tests/input/input-json_object1.json";
        // string path3 = "../tests/input/input-json_object2.json";
        // string path4 = "../tests/input/input-json_object3.json";

        JsonSchema schema(path1);
        CHECK(schema.validate(path2));
        // CHECK(schema.validate(path3));
        // CHECK_FALSE(schema.validate(path4));

    }
    TEST_CASE("[JsonSchemaTests] object_test02") {

        string path1 = "../tests/input/input-json_schema1.json";
        // string path2 = "../tests/input/input-json_object1.json";
        string path3 = "../tests/input/input-json_object2.json";
        // string path4 = "../tests/input/input-json_object3.json";

        JsonSchema schema(path1);
        // CHECK(schema.validate(path2));
        CHECK(schema.validate(path3));
        // CHECK_FALSE(schema.validate(path4));

    }
    TEST_CASE("[JsonSchemaTests] object_test03") {

        string path1 = "../tests/input/input-json_schema1.json";
        // string path2 = "../tests/input/input-json_object1.json";
        // string path3 = "../tests/input/input-json_object2.json";
        string path4 = "../tests/input/input-json_object3.json";

        JsonSchema schema(path1);
        // CHECK(schema.validate(path2));
        // CHECK(schema.validate(path3));
        CHECK_FALSE(schema.validate(path4));

    }
    TEST_CASE("[JsonSchemaTests] object_test04") {

        string path1 = "../tests/input/input-json_schema2.json";
        string path2 = "../tests/input/input-json_object1.json";
        // string path3 = "../tests/input/input-json_object2.json";
        // string path4 = "../tests/input/input-json_object3.json";

        JsonSchema schema(path1);
        CHECK(schema.validate(path2));
        // CHECK_FALSE(schema.validate(path3));
        // CHECK(schema.validate(path4));

    }
    TEST_CASE("[JsonSchemaTests] object_test05") {

        string path1 = "../tests/input/input-json_schema2.json";
        // string path2 = "../tests/input/input-json_object1.json";
        string path3 = "../tests/input/input-json_object2.json";
        // string path4 = "../tests/input/input-json_object3.json";

        JsonSchema schema(path1);
        // CHECK(schema.validate(path2));
        CHECK_FALSE(schema.validate(path3));
        // CHECK(schema.validate(path4));

    }
    TEST_CASE("[JsonSchemaTests] object_test06") {

        string path1 = "../tests/input/input-json_schema2.json";
        // string path2 = "../tests/input/input-json_object1.json";
        // string path3 = "../tests/input/input-json_object2.json";
        string path4 = "../tests/input/input-json_object3.json";

        JsonSchema schema(path1);
        // CHECK(schema.validate(path2));
        // CHECK_FALSE(schema.validate(path3));
        CHECK(schema.validate(path4));

    }

    TEST_CASE("[JsonSchemaTests] array_test01") {

        string path1 = "../tests/input/input-json_schema3.json";
        string path2 = "../tests/input/input-json_array1.json";
        // string path3 = "../tests/input/input-json_array2.json";

        JsonSchema schema(path1);
        CHECK(schema.validate(path2));
        // CHECK_FALSE(schema.validate(path3));

    }
    TEST_CASE("[JsonSchemaTests] array_test02") {

        string path1 = "../tests/input/input-json_schema3.json";
        // string path2 = "../tests/input/input-json_array1.json";
        string path3 = "../tests/input/input-json_array2.json";

        JsonSchema schema(path1);
        // CHECK(schema.validate(path2));
        CHECK_FALSE(schema.validate(path3));

    }
    TEST_CASE("[JsonSchemaTests] array_test03") {

        string path1 = "../tests/input/input-json_schema4.json";
        string path2 = "../tests/input/input-json_array1.json";
        // string path3 = "../tests/input/input-json_array2.json";

        JsonSchema schema(path1);
        CHECK_FALSE(schema.validate(path2));
        // CHECK(schema.validate(path3));

    }
    TEST_CASE("[JsonSchemaTests] array_test04") {

        string path1 = "../tests/input/input-json_schema4.json";
        // string path2 = "../tests/input/input-json_array1.json";
        string path3 = "../tests/input/input-json_array2.json";

        JsonSchema schema(path1);
        // CHECK_FALSE(schema.validate(path2));
        CHECK(schema.validate(path3));

    }
    TEST_CASE("[JsonSchemaTests] recursion_test01") {

        string path1 = "../tests/input/input-json_schema5.json";
        string path2 = "../tests/input/input-json_rec1.json";
        // string path3 = "../tests/input/input-json_rec2.json";

        JsonSchema schema(path1);
        CHECK(schema.validate(path2));
        // CHECK_FALSE(schema.validate(path3));

    }
    TEST_CASE("[JsonSchemaTests] recursion_test02") {

        string path1 = "../tests/input/input-json_schema5.json";
        // string path2 = "../tests/input/input-json_rec1.json";
        string path3 = "../tests/input/input-json_rec2.json";

        JsonSchema schema(path1);
        // CHECK(schema.validate(path2));
        CHECK_FALSE(schema.validate(path3));

    }
}