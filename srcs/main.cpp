#include <iostream>
#include <vector>
#include <fstream>

#include "json_parser.hpp"

using namespace std;
using namespace jsonparser;

int main(){
	JsonObject* school=new JsonObject; // 新建一个JsonObject对象
	JsonArray* students=nullptr;
	school->create_array("students")->out_array("students",&students)->get_array("students") // 新建students字段数组并赋值给变量,然后进入字段
			->create_back_object()->get_back_object()  // 在students中添加第一个学生信息,并进入字段
				->set("name",R"("kit")"_json_string)
				->set("age",18_json_number)
				->set("sex",R"("m")"_json_string)
			->move_out_array()  // 返回students字段
			->create_back_object()->get_back_object()  // 在students字段中添加第二个学生,并进入字段
				->set("name",R"("kitty")"_json_string)
				->set("age",20_json_number)
				->set("sex",R"("m")"_json_string)
			->move_out_array()  // 返回students字段
		->move_out_object() // 返回最外层的JsonObject对象
		->set("year",R"("2019-9-16")"_json_string);  // 新建year字段

	std::cout<<"Student Total: "<<students->size()<<std::endl;
	std::cout<<school->to_json_string()<<std::endl; // Json对象转Json字符串
	delete school;
	return 0;
 }
