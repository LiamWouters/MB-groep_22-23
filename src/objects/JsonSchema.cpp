//
// Created by siebe on 22/01/2023.
//

#include "JsonSchema.h"
#include "JsonTokenizer.h"
#include <limits>

using json = nlohmann::json;

JsonSchema::JsonSchema(const string& path){
    ifstream input(path);
    json j;
    input >> j;

    type = j["type"].get<string>();
    if (type == "boolean");
    else if (type == "null") {}
    else if (type == "string") {
        int minLength = 0;
        int maxLength = numeric_limits<int>::max();
        if (j.contains("minLength")) {minLength = j["minLength"].get<int>();}
        if (j.contains("maxLength")) {maxLength = j["maxLength"].get<int>();}
        stringdata = new JsonString(minLength, maxLength);
    }
    else if (type == "number") {
        double multipleOf = 0;
        double minimum = -numeric_limits<double>::infinity();
        double maximum = numeric_limits<double>::infinity();
        if (j.contains("multipleOf")) {multipleOf = j["multipleOf"].get<double>();}
        if (j.contains("minimum")) {minimum = j["minimum"].get<double>();}
        if (j.contains("maximum")) {maximum = j["maximum"].get<double>();}
        numberdata = new JsonNumber(minimum, maximum, multipleOf);
    }
    else if (type == "array") {
        JsonSchema* items = nullptr;
        JsonSchema* contains = nullptr;
        int minItems = 0;
        int maxItems = numeric_limits<int>::max();
        if (j.contains("items")) {
            json arrayItemType = j["items"];
            ofstream out("schemagarbagefile.json");
            out << arrayItemType << endl;
            items = new JsonSchema("schemagarbagefile.json");
        }
        if (j.contains("contains")) {
            json arrayContainType = j["contains"];
            ofstream out("schemagarbagefile.json");
            out << arrayContainType << endl;
            contains = new JsonSchema("schemagarbagefile.json");
        }
        if (j.contains("minItems")) {minItems = j["minItems"].get<int>();}
        if (j.contains("minItems")) {maxItems = j["maxItems"].get<int>();}
        arraydata = new JsonArray(items, contains, minItems, maxItems);
    } else if (type == "object") {
        if (j.contains("required")) {
            for (string name : j["required"]) {
                required.push_back(name);
            }
        }
        if (j.contains("properties")) {
            for (const auto& obj : j["properties"].items()) {
                const string& objName = obj.key();
                json jo = obj.value();
                ofstream out("schemagarbagefile.json");
                out << jo << endl;
                JsonSchema objSchem("schemagarbagefile.json");
                JsonObject object(objName, objSchem);
                properties.push_back(object);
            }
        }
    }
}

bool JsonSchema::privateValidate(json j){
    ofstream out("validationgarbagefile.json");
    out << j << endl;
    JsonTokenizer tokenizer;
    tokenizer.tokenizeSimplified("validationgarbagefile.json");
    string arrayPart;
    string objectPart;
    int inObject = 0;
    int inArray = 0;
    for (const auto& token : tokenizer.tokens) {
        if (inArray > 0 and token.type != "ARRAY_CLOSE" and token.type != "ARRAY_OPEN") {
            arrayPart += token.content;
            continue;
        }
        if (inObject > 0 and token.type != "CURLY_CLOSE" and token.type != "CURLY_OPEN") {
            objectPart += token.content;
            continue;
        }
        if (token.type == "COMMA") {
            arrayPart += ",";
            continue;
        }
        if (token.type == "COLON") {
            continue;
        }
        if (token.type == "BOOLEAN") {
            if (type == "boolean") {
                continue;
            } else {
                return false;
            }
        }
        if (token.type == "NULL") {
            if (type == "null") {
                continue;
            } else {
                return false;
            }
        }
        if (token.type == "STRING") {
            if (type == "string" and token.content.size() >= stringdata->minLength and token.content.size() <= stringdata->maxLength) {
                continue;
            } else {
                return false;
            }
        }
        if (token.type == "NUMBER") {
            if (type == "number" and stoi(token.content) >= numberdata->minimum and stoi(token.content) <= numberdata->maximum) {
                continue;
            } else {
                return false;
            }
        }
        if (token.type == "ARRAY_OPEN") {
            if (type == "array") {
                inArray += 1;
                arrayPart += "[";
                continue;
            } else {
                return false;
            }
        }
        if (token.type == "CURLY_OPEN") {
            if (type == "object") {
                inObject += 1;
                objectPart += "{";
                continue;
            } else {
                return false;
            }
        }
        if (token.type == "ARRAY_CLOSE") {
            inArray -= 1;
            arrayPart += "]";
            if (inArray > 0) {
                continue;
            }
            if (arraydata->items != nullptr) {
                for (const auto& arrayPiece : json::parse(arrayPart)) {
                    if (arraydata->items->privateValidate(arrayPiece)) {
                        continue;
                    } else {
                        return false;
                    }
                }
            }
            if (arraydata->contains != nullptr) {
                bool found = false;
                for (const auto& arrayPiece : json::parse(arrayPart)) {
                    if (arraydata->contains->privateValidate(arrayPiece)) {
                        found = true;
                    }
                }
                if (!found) {
                    return false;
                }
            }
        }
        if (token.type == "CURLY_CLOSE") {
            inObject -= 1;
            objectPart += "}";
            if (inObject > 0) {
                continue;
            }
            json jPart = json::parse(objectPart);
            for (const string& name : required) {
                bool found = false;
                for (const auto& obj : jPart.items()) {
                    if (name == obj.key()) {
                        found = true;
                    }
                }
                if (!found) {
                    return false;
                }
            }
            for (auto prop : properties) {
                for (const auto& obj : jPart.items()) {
                    if (prop.name == obj.key()) {
                        if (prop.schema.privateValidate(obj.value())) {
                            continue;
                        } else {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool JsonSchema::validate(const string& path){
    ifstream input(path);
    json j;
    input >> j;
    return privateValidate(j);
}

JsonString::JsonString(int minLength, int maxLength):minLength(minLength),maxLength(maxLength){}
JsonNumber::JsonNumber(double minimum, double maximum, double multipleOf = 0):minimum(minimum),maximum(maximum),multipleOf(multipleOf){}
JsonArray::JsonArray(JsonSchema *items, JsonSchema *contains = nullptr, int minItems = 0, int maxItems = 0):items(items),contains(contains),minItems(minItems),maxItems(maxItems){}
JsonObject::JsonObject(string name, JsonSchema schema):name(std::move(name)),schema(std::move(schema)){}
