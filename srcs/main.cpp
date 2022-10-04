#include <iostream>
#include <vector>

#include "json_parser.hpp"

using namespace std;
using namespace jsonparser;

int main(){
	const char* json="{\"leader\":{\"name\":\"kitty\"},\"scores\":[60,70,80]}";
	JsonObject* obj=JsonParser::parse_for_object(json);
	JsonObject* leader=obj->get_object("leader");
	JsonArray*  scores=obj->get_array("scores");
	for (JsonBase* score:scores->as_vector()){
		JsonNumber* s=(JsonNumber*)score;
		cout<<s->as_int()<<endl;
	}
	cout<<obj->to_json_string()<<endl;
	delete obj;
	return 0;
}
