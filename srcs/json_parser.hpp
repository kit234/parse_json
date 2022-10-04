#ifndef __JSON_PARSER_HPP__
#define __JSON_PARSER_HPP__

#include <string>
#include <map>
#include <vector>

#include <iostream>

#define __JSON_PARSER_START_NAMESPACE namespace jsonparser {
#define __JSON_PARSER_END_NAMESPACE };

__JSON_PARSER_START_NAMESPACE

class JsonBase;
class JsonObject;
class JsonArray;
class JsonString;
class JsonNumber;
class JsonBoolean;

class JsonBase {
public:
	virtual std::string to_json_string() const=0;
	virtual void assign(const JsonBase*)=0;
	// clone self
	virtual JsonBase* clone() const=0;
};
class JsonObject :public JsonBase {
public:
	JsonObject() :JsonBase(){}
	~JsonObject(){
		__release();
	}
	void set(const std::string& key,JsonBase* obj){
		if (__values.count(key)>0){
			delete __values[key]; __values.erase(key);
		}
		__values.insert(std::make_pair(key,obj));
	}

	JsonObject*  get_object (const std::string& key) { return (JsonObject*) (__get(key)); }
	JsonArray*   get_array  (const std::string& key) { return (JsonArray*)  (__get(key)); }
	JsonString*  get_string (const std::string& key) { return (JsonString*) (__get(key)); }
	JsonNumber*  get_number (const std::string& key) { return (JsonNumber*) (__get(key)); }
	JsonBoolean* get_boolean(const std::string& key) { return (JsonBoolean*)(__get(key)); }

	const JsonObject*  get_object (const std::string& key) const
	{ return (const JsonObject*) (__get(key)); }
	const JsonArray*   get_array  (const std::string& key) const
	{ return (const JsonArray*)  (__get(key)); }
	const JsonString*  get_string (const std::string& key) const
	{ return (const JsonString*) (__get(key)); }
	const JsonNumber*  get_number (const std::string& key) const
	{ return (const JsonNumber*) (__get(key)); }
	const JsonBoolean* get_boolean(const std::string& key) const
	{ return (const JsonBoolean*)(__get(key)); }

	std::string to_json_string() const override {
		std::string result;
		result+='{';
		for (std::map<std::string,JsonBase*>::const_iterator p=__values.begin();p!=__values.end();++p){
			const std::string& key=p->first;
			const JsonBase*  value=p->second;
			result+=('"'+key+'"');
			result+=':';
			result+=value->to_json_string();
			result+=',';
		}
		result.pop_back(); // remove tail ','
		result+='}';

		return result;
	}
	void assign(const JsonBase* r) override {
		if (this==r) return;
		__release();
		const JsonObject* rhs=static_cast<const JsonObject*>(r);
		const std::map<std::string,JsonBase*>* rhs_values=&(rhs->__values);

		for (std::map<std::string,JsonBase*>::const_iterator p=rhs_values->begin();p!=rhs_values->end();++p){
			const std::string& key=p->first;
			const JsonBase*  value=p->second;
			(this->__values).insert(std::make_pair(key,value->clone()));
		}
	}
	JsonBase* clone() const override {
		JsonObject* result=new JsonObject;

		for (std::map<std::string,JsonBase*>::const_iterator p=__values.begin();p!=__values.end();++p){
			const std::string& key=p->first;
			const JsonBase*  value=p->second;
			(result->__values).insert(std::make_pair(key,value->clone()));
		}

		return static_cast<JsonBase*>(result);
	}
private:
	void __release() {
		for (std::map<std::string,JsonBase*>::iterator p=__values.begin();p!=__values.end();++p){
			delete (p->second);
		}
		__values.clear();
	}
	JsonBase* __get(const std::string& key) { return __values[key]; }
	const JsonBase* __get(const std::string& key) const { return __values.at(key); }
private:
	std::map<std::string,JsonBase*> __values;
};
class JsonArray :public JsonBase {
public:
	JsonArray() :JsonBase() {}
	~JsonArray(){
		__release();
	}
	void push_back(JsonBase* obj){
		__array.push_back(obj);
	}

	JsonObject*  get_object (size_t idx) { return (JsonObject*) (__get(idx)); }
	JsonArray*   get_array  (size_t idx) { return (JsonArray*)  (__get(idx)); }
	JsonString*  get_string (size_t idx) { return (JsonString*) (__get(idx)); }
	JsonNumber*  get_number (size_t idx) { return (JsonNumber*) (__get(idx)); }
	JsonBoolean* get_boolean(size_t idx) { return (JsonBoolean*)(__get(idx)); }

	const JsonObject*  get_object (size_t idx) const
	{ return (const JsonObject*) (__get(idx)); }
	const JsonArray*   get_array  (size_t idx) const
	{ return (const JsonArray*)  (__get(idx)); }
	const JsonString*  get_string (size_t idx) const
	{ return (const JsonString*) (__get(idx)); }
	const JsonNumber*  get_number (size_t idx) const
	{ return (const JsonNumber*) (__get(idx)); }
	const JsonBoolean* get_boolean(size_t idx) const
	{ return (const JsonBoolean*)(__get(idx)); }

	std::vector<JsonBase*> as_vector() { return __array; }

	const std::vector<JsonBase*> as_vector() const { return __array; }

	std::string to_json_string() const override {
		std::string result;

		result+='[';
		for (std::vector<JsonBase*>::const_iterator p=__array.begin();p!=__array.end();++p){
			result+=(*p)->to_json_string();
			result+=',';
		}
		result.pop_back(); // remove tail ','
		result+=']';

		return result;
	}
	void assign(const JsonBase* r) override {
		if (this==r) return;
		__release();
		const JsonArray* rhs=static_cast<const JsonArray*>(r);
		const std::vector<JsonBase*>* rhs_array=&(rhs->__array);

		for (std::vector<JsonBase*>::const_iterator p=rhs_array->begin();p!=rhs_array->end();++p){
			(this->__array).push_back((*p)->clone());
		}
	}
	JsonBase* clone() const override {
		JsonArray* result=new JsonArray;

		for (std::vector<JsonBase*>::const_iterator p=__array.begin();p!=__array.end();++p){
			(result->__array).push_back((*p)->clone());
		}

		return static_cast<JsonBase*>(result);
	}
private:
	void __release(){
		for (std::vector<JsonBase*>::iterator p=__array.begin();p!=__array.end();++p){
			delete (*p);
		}
		__array.clear();
	}
	JsonBase* __get(size_t idx){
		return __array[idx];
	}
	const JsonBase* __get(size_t idx) const {
		return __array.at(idx);
	}
private:
	std::vector<JsonBase*> __array;
};
class JsonString :public JsonBase {
public:
	JsonString() :JsonBase(){}
	JsonString(const std::string& str) :__string(str){}

	const std::string as_string() const { return __string; }

	std::string to_json_string() const override {
		std::string result;

		result+='"'; result+=__string; result+='"';

		return result;
	}
	void assign(const JsonBase* r) override {
		if (this==r) return;
		const JsonString* rhs=static_cast<const JsonString*>(r);

		this->__string=rhs->__string;
	}
	JsonBase* clone() const override {
		JsonString* result=new JsonString;

		result->__string=this->__string;

		return static_cast<JsonBase*>(result);
	}
private:
	std::string __string;
};
class JsonNumber :public JsonBase {
public:
	JsonNumber() :JsonBase(), __number(0){}
	JsonNumber(double number) :__number(number){}

	double as_double() const { return static_cast<double>(__number); }
	int    as_int()    const { return static_cast<int>   (__number); }
	float  as_float()  const { return static_cast<float> (__number); }

	std::string to_json_string() const override {
		std::string result;

		result+=std::to_string(__number);

		return result;
	}
	void assign(const JsonBase* r) override {
		if (this==r) return;
		const JsonNumber* rhs=static_cast<const JsonNumber*>(r);

		this->__number=rhs->__number;
	}
	JsonBase* clone() const override {
		JsonNumber* result=new JsonNumber;

		result->__number=this->__number;

		return static_cast<JsonBase*>(result);
	}
private:
	double __number;
};
class JsonBoolean :public JsonBase {
public:
	JsonBoolean() :JsonBase(), __boolean(false) {}
	JsonBoolean(bool boolean) :__boolean(boolean){}

	bool as_bool() const { return __boolean; }

	std::string to_json_string() const override {
		std::string result;

		if (__boolean) result="true" ;
		else           result="false";

		return result;
	}
	void assign(const JsonBase* r) override {
		if (this==r) return;
		const JsonBoolean* rhs=static_cast<const JsonBoolean*>(r);

		this->__boolean=rhs->__boolean;
	}
	JsonBase* clone() const override {
		JsonBoolean* result=new JsonBoolean;

		result->__boolean=this->__boolean;

		return static_cast<JsonBase*>(result);
	}
private:
	bool __boolean;
};

class JsonParser {
public:
	static JsonBase* parse(const char* json){
		size_t temp=0;
		return __parse_json(json,0,temp);
	}
	static JsonObject* parse_for_object(const char* json){
		return (JsonObject*)parse(json);
	}
	static JsonArray* parse_for_array(const char* json){
		return (JsonArray*)parse(json);
	}
private:
	static JsonBase* __parse_json_object(const char* json,size_t l,size_t& new_idx){
		JsonObject* result=new JsonObject;
		size_t idx=l+1;
		std::string key; key.clear();
		while (json[idx]!='}'){
			if (json[idx]==',') { ++idx; continue; }
			if (json[idx]==':'){
				size_t new_idx=0;
				key=key.substr(1,key.size()-2); // remove the '"' of key
				result->set(key,__parse_json(json,idx+1,new_idx));
				key.clear();
				idx=new_idx+1; continue;
			}
			key+=json[idx];
			++idx;
		}
		new_idx=idx;
		return static_cast<JsonBase*>(result);
	}
	static JsonBase* __parse_json_array(const char* json,size_t l,size_t& new_idx){
		JsonArray* result=new JsonArray;
		size_t idx=l+1;
		while (json[idx]!=']'){
			if (json[idx]==',') { ++idx; continue; }
			size_t new_idx=0;
			result->push_back(__parse_json(json,idx,new_idx));
			idx=new_idx+1;
		}
		new_idx=idx;
		return static_cast<JsonBase*>(result);
	}
	static JsonBase* __parse_json_string(const char* json,size_t l,size_t& new_idx){
		std::string temp; temp.clear();
		size_t idx=l+1;
		while (json[idx]!='"'){
			temp+=json[idx]; ++idx;
		}
		new_idx=idx;
		return static_cast<JsonBase*>(new JsonString(temp));
	}
	static JsonBase* __parse_json_number(const char* json,size_t l,size_t& new_idx){
		std::string temp; temp.clear();
		size_t idx=l;
		while (std::isdigit(json[idx])||json[idx]=='.'){
			temp+=json[idx]; ++idx;
		}
		new_idx=idx-1;
		return static_cast<JsonBase*>(new JsonNumber(std::stod(temp)));
	}
	static JsonBase* __parse_json_boolean(const char* json,size_t l,size_t& new_idx){
		if (json[l]=='t'){
			new_idx=l+3;
			return static_cast<JsonBase*>(new JsonBoolean(true));
		}
		else{
			new_idx=l+4;
			return static_cast<JsonBase*>(new JsonBoolean(false));
		}
	}

	static JsonBase* __parse_json(const char* json,size_t l,size_t& new_idx){
		char ch=json[l];
		if (ch=='{')          return __parse_json_object (json,l,new_idx);
		if (ch=='[')          return __parse_json_array  (json,l,new_idx);
		if (ch=='"')          return __parse_json_string (json,l,new_idx);
		if (std::isdigit(ch)) return __parse_json_number (json,l,new_idx);
		if (ch=='t'||ch=='f') return __parse_json_boolean(json,l,new_idx);
		return nullptr;
	}
};

__JSON_PARSER_END_NAMESPACE

#endif
