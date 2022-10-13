#include "json_parser.hpp"
#include <iostream>

using namespace std;
using namespace kit;

int main(){
	Json<> obj=JsonParser<>::parse(R"({"name" : "kit","age":18 })");
	cout<<obj<<endl;
	return 0;
}