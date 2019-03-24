#include <iostream>
#include <json/json.h>

void test(const Json::Value& j)
{
  std::string json_str = Json::writeString(Json::StreamWriterBuilder(), j);
  std::cout << json_str << std::endl;
}

int main()
{
  Json::Value j;
  j["test"] = 3.14;
  j["test3"] = "this is a test";
  j["sub1"]["sub2"]["field"] = true;
  test(j);
  test(j["sub1"]);
  j["test2"] = Json::Value();
  j["test2"].append(Json::Value(Json::ValueType::objectValue));
  j["test2"].append(Json::Value(Json::ValueType::objectValue));
  test(j);

  if (j.isObject())
  {
    std::cout << "OK!" << std::endl;
  }
  for (Json::Value::const_iterator it = j.begin(); it != j.end(); it++)
  {
    std::cout << it.name() << std::endl;
    std::cout << "??1 " << it.key().isObject() << std::endl;
    std::cout << "??2 " << it.key().isArray() << std::endl;
    std::cout << "??3 " << it.key().isString() << std::endl;
    std::cout << "??4 " << it.key().isDouble() << std::endl;
    std::cout << ">>> " << it.key() << std::endl;
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
