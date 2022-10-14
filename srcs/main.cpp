#include "json_parser.hpp"

#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;
using namespace kit;

void test_large_json(){
	cout<<"TEST LARGE JSON..."<<endl;
	ifstream fs("canada.json");
	string json,line;
	while (getline(fs,line)) json+=line;
	const char* text=json.c_str();
	cout<<"START PARSE JSON..."<<endl;
	clock_t start_time=clock();
	try {
		Json<> obj=JsonParser<>::parse(text);
	}
	catch (exception& e){
		fs.close();
		cout<<e.what()<<endl;
		cout<<"TEST FAILED"<<endl;
		exit(-1);
	}
	clock_t end_time=clock();
	cout<<"COST TIME: "<<end_time-start_time<<"ms"<<endl;
	fs.close();
	cout<<"TEST SUCCESS"<<endl;
}

void test_one_json_per_line(){
	cout<<"TEST ONE JSON PER LINE..."<<endl;
	ifstream fs("one-json-per-line.jsons");
	string line;
	while (getline(fs,line)){
		const char* json=line.c_str();
		try {
			Json<> obj=JsonParser<>::parse(json);
		}
		catch (exception& e){
			fs.close();
			cout<<e.what()<<endl;
			cout<<"TEST FAILED"<<endl;
			exit(-1);
		}
	}
	fs.close();
	cout<<"TEST SUCCESS"<<endl;
}

int main(){
	test_large_json();
	cout<<endl;
	test_one_json_per_line();
	return 0;
}