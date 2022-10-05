#include <iostream>
#include <vector>
#include <fstream>

#include "json_parser.hpp"

using namespace std;
using namespace jsonparser;

class Student :public JsonObject {
public:
	Student(const std::string& name,int age){
		this->set("name",new JsonString(name));
		this->set("age",new JsonNumber(age));
	}
};

int main(){
	JsonObject* school=new JsonObject; // 新建一个JsonObject对象
	JsonArray* students=nullptr;
	school->create_array("students")->to_array("students",&students) // 新建"students"的数组字段,并赋值给students变量
		->set("name",new JsonString("University")); // 新建一个name字段
	students->push_back(new Student("kit",18))      // 为students字段添加学生
		->push_back(new Student("kitty",20))
		->push_back(new Student("world",16));
	std::cout<<school->to_json_string()<<std::endl;
	delete school;
	return 0;
 }
