#include "json_parser.hpp"
#include <iostream>
#include <string>

using namespace std;
using namespace kit;

// string to json class
void string2json(){
	// use JsonParser::parse
	const char* json=R"({"name":"kit","age":18})";
	Json<> obj1=JsonParser<>::parse(json);

	// use literal
	Json<> obj2=R"({"name":"kit","age":18})"__json;
}

