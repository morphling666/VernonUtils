#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <fstream>
#include <iostream>
#include <iterator>
#include <json/json.h>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string.h>
#include <unordered_map>
#include <utility>
#include <vector>
//#include "reflection.h"

namespace Vernon {

class BinarySerializer;
class BinaryDeserializer;
class JsonSerializer;
class JsonDeserializer;

#define BASIC_TYPE_SERIALIZE(Type)                                                                                     \
    template <> void serialize(BinarySerializer &s, Type &obj) {                                                       \
        std::string ret;                                                                                               \
        ret.append((const char *)&obj, sizeof(Type));                                                                  \
        s.outStream().write(ret.data(), ret.size());                                                                   \
    }                                                                                                                  \
    template <> void serialize(Json::Value &val, const std::string &name, Type &obj) {                                 \
        std::string ret;                                                                                               \
        ret.append((const char *)&obj, sizeof(int));                                                                   \
        val[name] = ret;                                                                                               \
    }

#define BASIC_TYPE_DESERIALIZE(Type)                                                                                   \
    template <> void deserialize(BinaryDeserializer &s, Type &obj) {                                                   \
        memcpy(&obj, s.str().data(), sizeof(Type));                                                                    \
        s.str() = s.str().substr(sizeof(Type));                                                                        \
    }                                                                                                                  \
    template <> void deserialize(Json::Value &val, const std::string &name, Type &obj) {                               \
        memcpy(&obj, val[name].asString().data(), sizeof(Type));                                                       \
    }

#define BASIC_TYPE_SERIALIZE_AND_DESERIALIZE(Type)                                                                     \
    BASIC_TYPE_SERIALIZE(Type)                                                                                         \
    BASIC_TYPE_DESERIALIZE(Type)

template <typename SerializableType> void serialize(BinarySerializer &s, SerializableType &obj) { obj.serialize(s); }

template <typename SerializableType> void serialize(BinarySerializer &s, SerializableType *ptr) { ptr->serialize(s); }

template <typename SerializableType> void serialize(BinarySerializer &s, std::vector<SerializableType> &obj) {
    int len = obj.size();
    serialize(s, len);
    for (int i = 0; i < len; ++i) {
        serialize(s, obj[i]);
    }
}

template <typename SerializableType> void serialize(BinarySerializer &s, std::list<SerializableType> &obj) {
    int len = obj.size();
    serialize(s, len);
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        serialize(s, *it);
    }
}

template <typename SerializableType> void serialize(BinarySerializer &s, std::set<SerializableType> &obj) {
    std::vector<SerializableType> tmp;
    std::copy(obj.begin(), obj.end(), std::back_inserter(tmp));
    serialize(s, tmp);
}

template <typename SerializableTypeA, typename SerializableTypeB>
void serialize(BinarySerializer &s, std::map<SerializableTypeA, SerializableTypeB> &obj) {
    std::vector<SerializableTypeA> tmp_key;
    std::vector<SerializableTypeB> tmp_value;

    for (auto it = obj.begin(); it != obj.end(); ++it) {
        tmp_key.push_back(it->first);
        tmp_value.push_back(it->second);
    }

    serialize(s, tmp_key);
    serialize(s, tmp_value);
}

template <typename SerializableTypeA, typename SerializableTypeB>
void serialize(BinarySerializer &s, std::unordered_map<SerializableTypeA, SerializableTypeB> &obj) {
    std::vector<SerializableTypeA> tmp_key;
    std::vector<SerializableTypeB> tmp_value;

    for (auto it = obj.begin(); it != obj.end(); ++it) {
        tmp_key.push_back(it->first);
        tmp_value.push_back(it->second);
    }

    serialize(s, tmp_key);
    serialize(s, tmp_value);
}

class BinarySerializer {
public:
    BinarySerializer() : os(std::ios::binary) {}
    template <typename SerializableType> BinarySerializer &operator<<(SerializableType &obj) {
        serialize(*this, obj);
        return *this;
    }
    template <typename SerializableType> BinarySerializer &operator<<(SerializableType *obj) {
        serialize(*this, obj);
        return *this;
    }
    template <typename SerializableType> BinarySerializer &operator<<(std::vector<SerializableType> &obj) {
        int len = obj.size();
        serialize(*this, len);

        for (int i = 0; i < len; ++i) {
            serialize(*this, obj[i]);
        }
        return *this;
    }
    template <typename SerializableType> BinarySerializer &operator<<(std::list<SerializableType> &obj) {
        int len = obj.size();
        serialize(*this, len);

        for (auto it = obj.begin(); it != obj.end(); ++it) {
            serialize(*this, *it);
        }
        return *this;
    }
    template <typename SerializableType> BinarySerializer &operator<<(std::set<SerializableType> &obj) {
        std::vector<SerializableType> tmp;
        std::copy(obj.begin(), obj.end(), std::back_inserter(tmp));
        return *this << tmp;
    }

    template <typename SerializableTypeA, typename SerializableTypeB>
    BinarySerializer &operator<<(std::map<SerializableTypeA, SerializableTypeB> &obj) {
        std::vector<SerializableTypeA> tmp_key;
        std::vector<SerializableTypeB> tmp_value;

        for (auto it = obj.begin(); it != obj.end(); ++it) {
            tmp_key.push_back(it->first);
            tmp_value.push_back(it->second);
        }

        *this << tmp_key;
        return *this << tmp_value;
    }
    template <typename SerializableTypeA, typename SerializableTypeB>
    BinarySerializer &operator<<(std::unordered_map<SerializableTypeA, SerializableTypeB> &obj) {
        std::vector<SerializableTypeA> tmp_key;
        std::vector<SerializableTypeB> tmp_value;

        for (auto it = obj.begin(); it != obj.end(); ++it) {
            tmp_key.push_back(it->first);
            tmp_value.push_back(it->second);
        }

        *this << tmp_key;
        return *this << tmp_value;
    }
    void reset() { os.str(""); }
    std::ostringstream &outStream() { return os; }
    std::string str() { return os.str(); }

private:
    std::ostringstream os;
};

template <typename SerializableType> void deserialize(BinaryDeserializer &s, SerializableType &obj) {
    obj.deserialize(s);
}

template <typename SerializableType> void deserialize(BinaryDeserializer &s, SerializableType *ptr) {
    ptr->deserialize(s);
}

template <typename SerializableType> void deserialize(BinaryDeserializer &s, std::vector<SerializableType> &obj) {
    int len = 0;
    deserialize(s, len);
    for (int i = 0; i < len; ++i) {
        SerializableType tmp;
        deserialize(s, tmp);
        obj.emplace_back(tmp);
    }
}

template <typename SerializableType> void deserialize(BinaryDeserializer &s, std::list<SerializableType> &obj) {
    int len = 0;
    deserialize(s, len);
    for (int i = 0; i < len; ++i) {
        SerializableType tmp;
        deserialize(s, tmp);
        obj.emplace_back(tmp);
    }
}

template <typename SerializableType> void deserialize(BinaryDeserializer &s, std::set<SerializableType> &obj) {
    std::vector<SerializableType> tmp;
    deserialize(s, tmp);
    for (auto &elem : tmp)
        obj.insert(elem);
}

template <typename SerializableTypeA, typename SerializableTypeB>
void deserialize(BinaryDeserializer &s, std::map<SerializableTypeA, SerializableTypeB> &obj) {
    std::vector<SerializableTypeA> tmp_key;
    std::vector<SerializableTypeB> tmp_value;
    deserialize(s, tmp_key);
    deserialize(s, tmp_value);
    int size = tmp_key.size();
    for (int i = 0; i < size; ++i) {
        obj.insert(std::make_pair(tmp_key[i], tmp_value[i]));
    }
}

template <typename SerializableTypeA, typename SerializableTypeB>
void deserialize(BinaryDeserializer &s, std::unordered_map<SerializableTypeA, SerializableTypeB> &obj) {
    std::vector<SerializableTypeA> tmp_key;
    std::vector<SerializableTypeB> tmp_value;
    deserialize(s, tmp_key);
    deserialize(s, tmp_value);
    int size = tmp_key.size();
    for (int i = 0; i < size; ++i) {
        obj.insert(std::make_pair(tmp_key[i], tmp_value[i]));
    }
}

class BinaryDeserializer {
public:
    BinaryDeserializer(const std::string &str) : binary(str) {}
    template <typename SerializableType> void operator>>(SerializableType &obj) { deserialize(*this, obj); }
    template <typename SerializableType> void operator>>(SerializableType *obj) { deserialize(*this, obj); }
    template <typename SerializableType> void operator>>(std::vector<SerializableType> &obj) {
        int len = obj.size();
        deserialize(*this, len);

        for (int i = 0; i < len; ++i) {
            SerializableType tmp;
            deserialize(*this, tmp);
            obj.emplace_back(tmp);
        }
    }
    template <typename SerializableType> void operator>>(std::list<SerializableType> &obj) {
        int len = obj.size();
        deserialize(*this, len);

        for (int i = 0; i < len; ++i) {
            SerializableType tmp;
            deserialize(*this, tmp);
            obj.emplace_back(tmp);
        }
    }
    template <typename SerializableType> void operator>>(std::set<SerializableType> &obj) {
        std::vector<SerializableType> tmp;
        *this >> tmp;
        for (auto &elem : tmp)
            obj.insert(elem);
    }

    template <typename SerializableTypeA, typename SerializableTypeB>
    void operator>>(std::map<SerializableTypeA, SerializableTypeB> &obj) {
        std::vector<SerializableTypeA> tmp_key;
        std::vector<SerializableTypeB> tmp_value;
        *this >> tmp_key;
        *this >> tmp_value;
        int size = tmp_key.size();
        for (int i = 0; i < size; ++i) {
            obj.insert(std::make_pair(tmp_key[i], tmp_value[i]));
        }
    }
    template <typename SerializableTypeA, typename SerializableTypeB>
    void operator>>(std::unordered_map<SerializableTypeA, SerializableTypeB> &obj) {
        std::vector<SerializableTypeA> tmp_key;
        std::vector<SerializableTypeB> tmp_value;
        *this >> tmp_key;
        *this >> tmp_value;
        int size = tmp_key.size();
        for (int i = 0; i < size; ++i) {
            obj.insert(std::make_pair(tmp_key[i], tmp_value[i]));
        }
    }
    void reset(const std::string &str) { binary = str; }
    std::string &str() { return binary; }

private:
    std::string binary;
};

template <typename SerializableType> void serialize(Json::Value &val, const std::string &name, SerializableType &obj) {
    obj.serialize(val[name]);
}

template <typename SerializableType>
void serialize(Json::Value &val, const std::string &name, std::vector<SerializableType> &obj) {
    int len = obj.size();
    serialize(val[name], "size", len);
    for (int i = 0; i < len; ++i) {
        std::string data = "data_" + std::to_string(i);
        serialize(val[name], data, obj[i]);
    }
}

template <typename SerializableType>
void serialize(Json::Value &val, const std::string &name, std::list<SerializableType> &obj) {
    int len = obj.size();
    serialize(val[name], "size", len);
    int i = 0;
    for (auto it = obj.begin(); it != obj.end(); ++it, ++i) {
        std::string data = "data_" + std::to_string(i);
        serialize(val[name], data, *it);
    }
}

template <typename SerializableType>
void serialize(Json::Value &val, const std::string &name, std::set<SerializableType> &obj) {
    std::vector<SerializableType> tmp;
    std::copy(obj.begin(), obj.end(), std::back_inserter(tmp));
    serialize(val, name, tmp);
}

template <typename SerializableTypeA, typename SerializableTypeB>
void serialize(Json::Value &val, const std::string &name, std::map<SerializableTypeA, SerializableTypeB> &obj) {
    std::vector<SerializableTypeA> tmp_key;
    std::vector<SerializableTypeB> tmp_value;

    for (auto it = obj.begin(); it != obj.end(); ++it) {
        tmp_key.push_back(it->first);
        tmp_value.push_back(it->second);
    }

    serialize(val, name + "_key", tmp_key);
    serialize(val, name + "_value", tmp_value);
}

template <typename SerializableTypeA, typename SerializableTypeB>
void serialize(Json::Value &val, const std::string &name,
               std::unordered_map<SerializableTypeA, SerializableTypeB> &obj) {
    std::vector<SerializableTypeA> tmp_key;
    std::vector<SerializableTypeB> tmp_value;

    for (auto it = obj.begin(); it != obj.end(); ++it) {
        tmp_key.push_back(it->first);
        tmp_value.push_back(it->second);
    }

    serialize(val, name + "_key", tmp_key);
    serialize(val, name + "_value", tmp_value);
}

class JsonSerializer {
public:
    JsonSerializer(const std::string &name) { filename = name + ".json"; }
    template <typename SerializableType>
    JsonSerializer &transferToJson(const std::string &name, SerializableType &obj) {
        serialize(root, name, obj);
        std::ofstream file_stream(filename, std::ios::out | std::ios::app);
        file_stream << json_writer.write(root);
        file_stream.close();
        root.clear();
        return *this;
    }
    template <typename SerializableType>
    JsonSerializer &transferToJson(const std::string &name, std::vector<SerializableType> &obj) {
        int len = obj.size();
        serialize(root[name], "size", len);

        for (int i = 0; i < len; ++i) {
            std::string data = "data_" + std::to_string(i);
            serialize(root[name], data, obj[i]);
        }
        std::ofstream file_stream(filename, std::ios::out | std::ios::app);
        file_stream << json_writer.write(root);
        file_stream.close();
        root.clear();
        return *this;
    }
    template <typename SerializableType>
    JsonSerializer &transferToJson(const std::string &name, std::list<SerializableType> &obj) {
        int len = obj.size();
        serialize(root[name], "size", len);
        int i = 0;
        for (auto it = obj.begin(); it != obj.end(); ++it, ++i) {
            std::string data = "data_" + std::to_string(i);
            serialize(root[name], data, *it);
        }
        std::ofstream file_stream(filename, std::ios::out | std::ios::app);
        file_stream << json_writer.write(root);
        file_stream.close();
        root.clear();
        return *this;
    }
    template <typename SerializableType>
    JsonSerializer &transferToJson(const std::string &name, std::set<SerializableType> &obj) {
        std::vector<SerializableType> tmp;
        std::copy(obj.begin(), obj.end(), std::back_inserter(tmp));
        serialize(root, name, tmp);
        std::ofstream file_stream(filename, std::ios::out | std::ios::app);
        file_stream << json_writer.write(root);
        file_stream.close();
        root.clear();
        return *this;
    }
    template <typename SerializableTypeA, typename SerializableTypeB>
    JsonSerializer &transferToJson(const std::string &name, std::map<SerializableTypeA, SerializableTypeB> &obj) {
        std::vector<SerializableTypeA> tmp_key;
        std::vector<SerializableTypeB> tmp_value;

        for (auto it = obj.begin(); it != obj.end(); ++it) {
            tmp_key.push_back(it->first);
            tmp_value.push_back(it->second);
        }
        serialize(root, name + "_key", tmp_key);
        serialize(root, name + "_value", tmp_value);
        std::ofstream file_stream(filename, std::ios::out | std::ios::app);
        file_stream << json_writer.write(root);
        file_stream.close();
        root.clear();
        return *this;
    }
    template <typename SerializableTypeA, typename SerializableTypeB>
    JsonSerializer &transferToJson(const std::string &name,
                                   std::unordered_map<SerializableTypeA, SerializableTypeB> &obj) {
        std::vector<SerializableTypeA> tmp_key;
        std::vector<SerializableTypeB> tmp_value;

        for (auto it = obj.begin(); it != obj.end(); ++it) {
            tmp_key.push_back(it->first);
            tmp_value.push_back(it->second);
        }
        serialize(root, name + "_key", tmp_key);
        serialize(root, name + "_value", tmp_value);
        std::ofstream file_stream(filename, std::ios::out | std::ios::app);
        file_stream << json_writer.write(root);
        file_stream.close();
        root.clear();
        return *this;
    }
    void reset() {
        std::ofstream file_stream(filename, std::ios::out);
        file_stream.close();
    }

private:
    std::string filename;
    Json::Value root;
    Json::StyledWriter json_writer;
};

template <typename SerializableType>
void deserialize(Json::Value &val, const std::string &name, SerializableType &obj) {
    obj.deserialize(val[name]);
}

template <typename SerializableType>
void deserialize(Json::Value &val, const std::string &name, std::vector<SerializableType> &obj) {
    int len = 0;
    deserialize(val[name], "size", len);
    for (int i = 0; i < len; ++i) {
        SerializableType tmp;
        std::string data = "data_" + std::to_string(i);
        deserialize(val[name], data, tmp);
        obj.emplace_back(tmp);
    }
}

class JsonDeserializer {
public:
    JsonDeserializer(const std::string &name) { filename = name + ".json"; }
    template <typename SerializableType> void transferToObject(const std::string &name, SerializableType &obj) {
        std::ifstream file_stream(filename, std::ios::in);
        json_reader.parse(file_stream, root);
        deserialize(root, name, obj);
        file_stream.close();
    }
    template <typename SerializableType>
    void transferToObject(const std::string &name, std::vector<SerializableType> &obj) {
        std::ifstream file_stream(filename, std::ios::in);
        json_reader.parse(file_stream, root);
        int len = 0;
        deserialize(root[name], "size", len);
        for (int i = 0; i < len; ++i) {
            SerializableType tmp;
            std::string data = "data_" + std::to_string(i);
            deserialize(root[name], data, tmp);
            obj.emplace_back(tmp);
        }
        file_stream.close();
    }
    template <typename SerializableTypeA, typename SerializableTypeB>
    void transferToObject(const std::string &name, std::map<SerializableTypeA, SerializableTypeB> &obj) {
        std::ifstream file_stream(filename, std::ios::in);
        json_reader.parse(file_stream, root);

        std::vector<SerializableTypeA> tmp_key;
        std::vector<SerializableTypeB> tmp_value;
        deserialize(root, name + "_key", tmp_key);
        deserialize(root, name + "_value", tmp_value);
        int size = tmp_key.size();
        for (int i = 0; i < size; ++i) {
            obj.insert(std::make_pair(tmp_key[i], tmp_value[i]));
        }
        file_stream.close();
    }
    template <typename SerializableTypeA, typename SerializableTypeB>
    void transferToObject(const std::string &name, std::unordered_map<SerializableTypeA, SerializableTypeB> &obj) {
        std::ifstream file_stream(filename, std::ios::in);
        json_reader.parse(file_stream, root);

        std::vector<SerializableTypeA> tmp_key;
        std::vector<SerializableTypeB> tmp_value;
        deserialize(root, name + "_key", tmp_key);
        deserialize(root, name + "_value", tmp_value);
        int size = tmp_key.size();
        for (int i = 0; i < size; ++i) {
            obj.insert(std::make_pair(tmp_key[i], tmp_value[i]));
        }
        file_stream.close();
    }

private:
    std::string filename;
    Json::Value root;
    Json::Reader json_reader;
};

BASIC_TYPE_SERIALIZE_AND_DESERIALIZE(char)
BASIC_TYPE_SERIALIZE_AND_DESERIALIZE(unsigned char)
BASIC_TYPE_SERIALIZE_AND_DESERIALIZE(short int)
BASIC_TYPE_SERIALIZE_AND_DESERIALIZE(unsigned short int)
BASIC_TYPE_SERIALIZE_AND_DESERIALIZE(int)
BASIC_TYPE_SERIALIZE_AND_DESERIALIZE(unsigned int)
BASIC_TYPE_SERIALIZE_AND_DESERIALIZE(long int)
BASIC_TYPE_SERIALIZE_AND_DESERIALIZE(unsigned long int)
BASIC_TYPE_SERIALIZE_AND_DESERIALIZE(long long int)
BASIC_TYPE_SERIALIZE_AND_DESERIALIZE(unsigned long long int)
BASIC_TYPE_SERIALIZE_AND_DESERIALIZE(float)
BASIC_TYPE_SERIALIZE_AND_DESERIALIZE(double)

} // namespace Vernon

#endif
