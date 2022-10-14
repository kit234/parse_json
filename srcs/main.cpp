#include "json_parser.hpp"
#include <iostream>

using namespace std;
using namespace kit;

int main(){
	Json<> obj;
	obj.insert("students")
			.push_back()
				.insert("name","kit")
				.insert("age",18)
				.insert("is_man",true)
			.move_out()
			.push_back()
				.insert("name","kitty")
				.insert("age",16)
			.move_out()
		.move_out()
		.insert("year","2019-9-23");
	obj["students"].insert(0).insert("name","wuhu").insert("age",20);
	cout<<obj<<endl;
	return 0;
}