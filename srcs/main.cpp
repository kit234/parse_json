#include <iostream>
#include <vector>
#include <fstream>

#include "json_parser.hpp"

using namespace std;
using namespace jsonparser;

int main(){
	JsonObject* school=new JsonObject; // �½�һ��JsonObject����
	JsonArray* students=nullptr;
	school->create_array("students")->out_array("students",&students)->get_array("students") // �½�students�ֶ����鲢��ֵ������,Ȼ������ֶ�
			->create_back_object()->get_back_object()  // ��students����ӵ�һ��ѧ����Ϣ,�������ֶ�
				->set("name",R"("kit")"_json_string)
				->set("age",18_json_number)
				->set("sex",R"("m")"_json_string)
			->move_out_array()  // ����students�ֶ�
			->create_back_object()->get_back_object()  // ��students�ֶ�����ӵڶ���ѧ��,�������ֶ�
				->set("name",R"("kitty")"_json_string)
				->set("age",20_json_number)
				->set("sex",R"("m")"_json_string)
			->move_out_array()  // ����students�ֶ�
		->move_out_object() // ����������JsonObject����
		->set("year",R"("2019-9-16")"_json_string);  // �½�year�ֶ�

	std::cout<<"Student Total: "<<students->size()<<std::endl;
	std::cout<<school->to_json_string()<<std::endl; // Json����תJson�ַ���
	delete school;
	return 0;
 }
