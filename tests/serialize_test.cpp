#include "reflection/serialization.h"
#include <iostream>


struct A {
    A() {
        a = 1;
    }
    virtual void serialize(Vernon::BinarySerializer& s)
    {
        s << a;
    }
    virtual void serialize(Json::Value& val)
    {
        Vernon::serialize(val, "a", a);
    }
    virtual void deserialize(Vernon::BinaryDeserializer& s)
    {
        s >> a;
    }
    virtual void deserialize(Json::Value& val)
    {
        Vernon::deserialize(val, "a", a);
    }
    virtual void print() {
        std::cout << "a = "<< a << std::endl;
    }
    int a;
};

struct B: public A {
    B(): A() {
        b = 66;
    }
    void serialize(Vernon::BinarySerializer& s) override
    {
        A::serialize(s);
        s << b;
    }
    void serialize(Json::Value& val) override
    {
        A::serialize(val);
        Vernon::serialize(val, "b", b);
    }
    void deserialize(Vernon::BinaryDeserializer& s) override
    {
        A::deserialize(s);
        s >> b;
    }
    void deserialize(Json::Value& val) override
    {
        A::deserialize(val);
        Vernon::deserialize(val, "b", b);
    }
    void print() override {
        A::print();
        std::cout << "b = "<< b << std::endl;
    }
    int a;
    int b;
};

int main() {
    // 1. binary serializer and deserializer test
    // serialize 2-level stl vector
    std::vector<std::vector<int>> vector_level_2(2);
    vector_level_2[0] = std::vector<int>{12, 3, 2};
    vector_level_2[1] = std::vector<int>{1, 3, 33, 22};
    Vernon::BinarySerializer serializer;
    serializer << vector_level_2;
    std::cout<<"binary size = "<<serializer.str().size()<<std::endl;
    // deserialize 2-level stl vector
    Vernon::BinaryDeserializer deserialier(serializer.str());
    std::vector<std::vector<int>> vector_level_2_new;
    deserialier >> vector_level_2_new;
    for (int i = 0; i < (int)vector_level_2_new.size(); ++i) {
        for (int j = 0; j < (int)vector_level_2_new[i].size(); ++j)
            std::cout << vector_level_2_new[i][j] << " ";
        std::cout<<std::endl;
    }
    // serialize pointer
    serializer.reset();
    A *a=new A();
    serializer << a;
    std::cout<<"binary size = "<<serializer.str().size()<<std::endl;
    // deserialize base class
    deserialier.reset(serializer.str());
    A new_a;
    deserialier >> new_a;
    new_a.print();
    // serialize derive class
    serializer.reset();
    B b;
    A &base_b=b;
    serializer << base_b;
    std::cout<<"binary size = "<<serializer.str().size()<<std::endl;
    // deserialize derive class
    deserialier.reset(serializer.str());
    B new_b;
    deserialier >> new_b;
    new_b.print();
    // 2. json serializer and deserializer test
    Vernon::JsonSerializer json_serializer("test_serial");
    // serialize 3-level stl vector
    std::vector<std::vector<std::vector<int>>> vector_level_3(2);
    vector_level_3[0] = vector_level_2;
    vector_level_3[1] = vector_level_2;
    json_serializer.transferToJson("vector_level_3", vector_level_3);
    // deserialize 3-level stl vector
    Vernon::JsonDeserializer json_deserializer("test_serial");
    std::vector<std::vector<std::vector<int>>> vector_level_3_new;
    json_deserializer.transferToObject("vector_level_3", vector_level_3_new);
    for (int i = 0; i < (int)vector_level_3_new.size(); ++i) {
        for (int j = 0; j < (int)vector_level_3_new[i].size(); ++j) {
            for (int k = 0; k < (int)vector_level_3_new[i][j].size(); ++k) {
                std::cout << vector_level_3_new[i][j][k] << " ";
            }
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
    }
    // serialize unordered_map
    json_serializer.reset();
    std::unordered_map<int, std::vector<int>> hash_level_2;
    hash_level_2[1] = std::vector<int>{1, 2, 4};
    hash_level_2[8] = std::vector<int>{3, 44, 8};
    json_serializer.transferToJson("hash_level_2", hash_level_2);
    // deserialize unordered_map
    std::unordered_map<int, std::vector<int>> hash_level_2_new;
    json_deserializer.transferToObject("hash_level_2", hash_level_2_new);
    // here
    for (auto it = hash_level_2_new.begin(); it != hash_level_2_new.end(); ++it) {
        int key = it->first;
        std::cout<<"key="<<key<<", value=";
        for (int j = 0; j < (int)it->second.size(); ++j) {
            std::cout << it->second[j] << " ";
        }
        std::cout<<std::endl;
    }
    // serialize base class
    json_serializer.reset();
    json_serializer.transferToJson("A", *a);
    A new_json_a;
    json_deserializer.transferToObject("A", new_json_a);
    new_json_a.print();
    // deserialize derive class
    json_serializer.reset();
    json_serializer.transferToJson("B", base_b);
    B new_json_b;
    json_deserializer.transferToObject("B", new_json_b);
    new_json_b.print();

	return 0;
}
