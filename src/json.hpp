#ifndef mineworld_json_hpp
#define mineworld_json_hpp
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

namespace mineworld {
    inline rapidjson::Document readJson(const std::string & path) {
        std::ifstream file(path);
        if (!file) throw std::runtime_error("cannot open JSON file: " + path);
        std::ostringstream contents;
        contents << file.rdbuf();
        rapidjson::Document document;
        document.Parse(contents.str().c_str());
        if (document.HasParseError())
            throw std::runtime_error(path + ": JSON error at " + std::to_string(document.GetErrorOffset()) +
                ": " + rapidjson::GetParseError_En(document.GetParseError()));
        return document;
    }
    inline const rapidjson::Value & jsonMember(const rapidjson::Value & object, const char * name) {
        if (!object.IsObject() || !object.HasMember(name))
            throw std::runtime_error(std::string("missing JSON field: ") + name);
        return object[name];
    }
    inline std::string jsonString(const rapidjson::Value & object, const char * name) {
        const auto & value = jsonMember(object, name);
        if (!value.IsString()) throw std::runtime_error(std::string("expected string: ") + name);
        return value.GetString();
    }
    inline int jsonInt(const rapidjson::Value & object, const char * name) {
        const auto & value = jsonMember(object, name);
        if (!value.IsInt()) throw std::runtime_error(std::string("expected integer: ") + name);
        return value.GetInt();
    }
    inline float jsonFloat(const rapidjson::Value & object, const char * name) {
        const auto & value = jsonMember(object, name);
        if (!value.IsNumber()) throw std::runtime_error(std::string("expected number: ") + name);
        return value.GetFloat();
    }
    inline bool jsonBool(const rapidjson::Value & object, const char * name) {
        const auto & value = jsonMember(object, name);
        if (!value.IsBool()) throw std::runtime_error(std::string("expected boolean: ") + name);
        return value.GetBool();
    }
}
#endif
