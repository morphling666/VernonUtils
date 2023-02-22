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
        val["a"] = a;
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
        val["b"] = b;
    }
    int a;
    int b;
};

int main() {
    // serialize 2-level stl vector
    std::vector<std::vector<int>> vector_level_2(2);
    vector_level_2[0] = std::vector<int>{12, 3, 2};
    vector_level_2[1] = std::vector<int>{1, 3, 33, 22};
    Vernon::BinarySerializer ss;
    ss << vector_level_2;
    std::cout<<ss.str().size()<<std::endl;
    // serialize pointer
    A *a=new A();
    ss << a;
    std::cout<<ss.str().size()<<std::endl;
    // serialize derive class
    B b;
    A &base_b=b;
    ss << base_b;
    std::cout<<ss.str().size()<<std::endl;
    Vernon::JsonSerializer test_json("test_serial");
    // serialize 3-level stl vector
    std::vector<std::vector<std::vector<int>>> vector_level_3(2);
    vector_level_3[0] = vector_level_2;
    vector_level_3[1] = vector_level_2;
    // serialize unordered_map
    std::unordered_map<int, std::vector<int>> hash_level_2;
    hash_level_2[1] = std::vector<int>{1, 2, 4};
    hash_level_2[8] = std::vector<int>{3, 44, 8};
    int name = 1;
    int k = 10;
    test_json.transferToJson("name", name).transferToJson("k", k).transferToJson("A", *a);
    test_json.transferToJson("vector_level_2", vector_level_2).transferToJson("B", base_b);
    test_json.transferToJson("vector_level_3", vector_level_3);
    test_json.transferToJson("hash_level_2", hash_level_2);
	return 0;
}
