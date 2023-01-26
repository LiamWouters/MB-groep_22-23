//
// Created by siebe on 22/01/2023.
//

#ifndef MB_GROEP_22_23_JSONSCHEMA_H
#define MB_GROEP_22_23_JSONSCHEMA_H

#include <iostream>
#include <fstream>
#include <vector>
#include "../../lib/nlohmann-json/json.hpp"

using namespace std;
using json = nlohmann::json;

class JsonString;
class JsonNumber;
class JsonArray;
class JsonObject;

class JsonSchema {
    // Functionaliteit:
    /*
     * Schema aanmaken
     * .json file valideren volgens schema
     * Meer?
     */
    // enum type {string, number, integer, object, array, boolean, null};
    string type;
    vector<string> required;
    vector<JsonObject> properties;
    JsonString* stringdata;
    JsonNumber* numberdata;
    JsonArray* arraydata;
    bool privateValidate(json j);

public:
    explicit JsonSchema(const string& path);
    bool validate(const string& path);
};

class JsonString {
public:
    int minLength;
    int maxLength;
    JsonString(int minLength, int maxLength);
};

class JsonNumber {
public:
    double minimum;
    double maximum;
    double multipleOf;
    JsonNumber(double minimum, double maximum, double multipleOf);
};

class JsonArray {
public:
    JsonSchema* items;
    JsonSchema* contains;
    int minItems;
    int maxItems;
    JsonArray(JsonSchema *items, JsonSchema *contains, int minItems, int maxItems);
};

class JsonObject {
public:
    string name;
    JsonSchema schema;
    JsonObject(string  name, JsonSchema  schema);
};



#endif //MB_GROEP_22_23_JSONSCHEMA_H
