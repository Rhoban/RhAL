#include <iostream>
#include "json.hpp"

void test(const nlohmann::json& j)
{
    std::cout << j.dump(4) << std::endl;
}

int main()
{
    nlohmann::json j;
    j["test"] = 3.14;
    j["test3"] = "this is a test";
    j["sub1"]["sub2"]["field"] = true;
    std::cout << j.dump(4) << std::endl;
    test(j["sub1"]);
    j["test2"] = nlohmann::json::array();
    j["test2"].push_back(nlohmann::json::object());
    j["test2"].push_back(nlohmann::json::object());
    std::cout << j.dump(4) << std::endl;

    if (j.is_object()) {
        std::cout << "OK!" << std::endl;
    }
    //for (const auto& it : j) {
    for (nlohmann::json::iterator it=j.begin();it!=j.end();it++) {
        std::cout << it.key() << std::endl;
        std::cout << "??1 " << it.value().is_object() << std::endl;
        std::cout << "??2 " << it.value().is_array() << std::endl;
        std::cout << "??3 " << it.value().is_string() << std::endl;
        std::cout << "??4 " << it.value().is_number() << std::endl;
        std::cout << ">>> " << it.value() << std::endl;
        /*
        if (it.is_number()) {
            std::cout << "# " << it.get<double>() << std::endl;
        }
        if (it.is_string()) {
            std::cout << "# " << it.get<std::string>() << std::endl;
        }
        */
    }

    return 0;
}

