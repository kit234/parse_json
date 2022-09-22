#include <iostream>

#include "json_parser.hpp"

using namespace std;

class Student:public JsonObject {
public:
	JSON_OBJECT(Student)
	Student(const string& name,int age,bool is_man,const std::initializer_list<double>& scores){
		DEFINE_FIELDS;
		SET_FIELD(this,JsonString,"name",name);
		SET_FIELD(this,JsonNumber,"age",age);
		SET_FIELD(this,JsonBoolean,"is_man",is_man);
		SET_FIELD(this,JsonArray<JsonNumber>,"scores",scores);
	}
private:
	START_DEFINE_FIELD
	DEFINE_FIELD(JsonString,"name");
	DEFINE_FIELD(JsonNumber,"age");
	DEFINE_FIELD(JsonBoolean,"is_man");
	DEFINE_FIELD(JsonArray<JsonNumber>,"scores");
	END_DEFINE_FIELD
};
class ClassRoom:public JsonObject {
public:
	JSON_OBJECT(ClassRoom)
private:
	START_DEFINE_FIELD
	DEFINE_FIELD(Student,"leader");
	DEFINE_FIELD(JsonArray<Student>,"students");
	END_DEFINE_FIELD
};

ostream& operator<<(ostream& os,const Student& student){
	os<<std::boolalpha;
	os<<"Name: "<<GET_FIELD(&student,JsonString,"name")->as<string>()<<endl;
	os<<"Age: "<<GET_FIELD(&student,JsonNumber,"age")->as<int>()<<endl;
	os<<"Is Man: "<<GET_FIELD(&student,JsonBoolean,"is_man")->as<bool>()<<endl;
	int sum_score=0;
	auto scores=GET_FIELD(&student,JsonArray<JsonNumber>,"scores");
	os<<"Scores: ";
	for (JsonNumber* score:scores->as<vector<JsonNumber*>>()){
		sum_score+=score->as<int>();
		os<<score->as<int>()<<' ';
	}
	os<<endl;
	double average_score=sum_score/(static_cast<double>(scores->size()));
	os<<"Average Score: "<<average_score<<endl;
	return os;
}


int main(){
	JsonParser parser=JsonParser::get_empty();
	JsonParser::Value& value=parser.get_root();
	value.add<Student>("leader",Student("XiaoHong",18,true,{80,90,95}))
		 .add<JsonArray<Student>>("students", JsonArray<Student>({
			new Student("Jason",  18, true,  {60,70,80}),
			new Student("Kitty",  17, false, {65,75,85}),
			new Student("HALTME", 16, true,  {90,85,95})}));
	unique_ptr<ClassRoom> class_room=unique_ptr<ClassRoom>(new ClassRoom);
	class_room->from_json_value(value);
	auto leader  =GET_FIELD(class_room,Student,"leader");
	auto students=GET_FIELD(class_room,JsonArray<Student>,"students");
	cout<<"Leader Info:"<<endl;
	cout<<*leader<<endl;
	for (Student* student:students->as<vector<Student*>>()){
		cout<<endl<<"Student Info:"<<endl;
		cout<<*student<<endl;
	}
	return 0;
}
