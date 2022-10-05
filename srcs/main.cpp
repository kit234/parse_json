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
	JsonObject* school=new JsonObject; // �½�һ��JsonObject����
	JsonArray* students=nullptr;
	school->create_array("students")->to_array("students",&students) // �½�"students"�������ֶ�,����ֵ��students����
		->set("name",new JsonString("University")); // �½�һ��name�ֶ�
	students->push_back(new Student("kit",18))      // Ϊstudents�ֶ����ѧ��
		->push_back(new Student("kitty",20))
		->push_back(new Student("world",16));
	std::cout<<school->to_json_string()<<std::endl;
	delete school;
	return 0;
 }
