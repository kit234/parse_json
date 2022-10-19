#include "json_parser.hpp"

#include <iostream>
#include <fstream>
#include <ctime>
#include <filesystem>

using namespace std;
using namespace kit;

namespace fs=filesystem;

void test_fail(){
	cout<<"TEST FAIL JSON..."<<endl;
	fs::path dir("data/fail");
	for (fs::directory_iterator end,ite(dir);ite!=end;++ite){
		string filename=string("./data/fail/")+(ite->path().filename().string());
		try {
			ifstream fs(filename);
			string tmp,line;
			while (getline(fs,line)) { tmp+=line; tmp+='\n'; }
			fs.close();
			const char* json=tmp.c_str();
			UTF8Json obj=UTF8JsonParser::parse(json);
		}
		catch (...){
			continue;
		}
		cout<<"TEST FAILED"<<endl;
		exit(-1);
	}
	cout<<"TEST SUCCESS"<<endl;
}

void test_pass(){
	cout<<"TEST PASS JSON..."<<endl;
	fs::path dir("data/pass");
	for (fs::directory_iterator end,ite(dir);ite!=end;++ite){
		string filename=string("./data/pass/")+(ite->path().filename().string());
		try {
			ifstream fs(filename);
			string tmp,line;
			while (getline(fs,line)) { tmp+=line; tmp+='\n'; }
			fs.close();
			const char* json=tmp.c_str();
			UTF8Json obj=UTF8JsonParser::parse(json);
		}
		catch (ParserException& e){
			cout<<e.what()<<endl;
			cout<<"TEST FAILED"<<endl;
			exit(-1);
		}
	}
	cout<<"TEST SUCCESS"<<endl;
}

void test_large_json(){
	cout<<"TEST LARGE JSON..."<<endl;
	fs::path dir("data/large");
	for (fs::directory_iterator ite(dir),end;ite!=end;++ite){
		string filename=string("data/large/")+(ite->path().filename().string());
		ifstream fs;
		fs.open(filename.c_str());
		string line,tmp;
		while (getline(fs,line)) { tmp+=line; tmp+='\n'; }
		fs.close();
		cout<<"JSON SIZE: "<<tmp.size()<<" Byte,";
		const char* json=tmp.c_str();
		try {
			clock_t start_time=clock();
			Json<> obj=JsonParser<>::parse(json);
			clock_t end_time=clock();
			cout<<"Cost Time: "<<end_time-start_time<<" ms"<<endl;
		}
		catch (ParserException& e){
			cout<<e.what()<<endl;
			cout<<"TEST FAILED"<<endl;
			exit(-1);
		}
	}
	cout<<"TEST SUCCESS"<<endl;
}

int main(){
	test_fail();
	test_pass();
	test_large_json();
	return 0;
}