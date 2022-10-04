#ifndef __JSON_PARSER_HPP__
#define __JSON_PARSER_HPP__

#include <string>
#include <map>
#include <vector>

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
	// return true if JsonString,JsonNumber,JsonBoolean
	virtual bool is_simple() const { return false; }
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

	JsonObject*  get_object (const std::string& key) { return static_cast<JsonObject*> (__get(key)); }
	JsonArray*   get_array  (const std::string& key) { return static_cast<JsonArray*>  (__get(key)); }
	JsonString*  get_string (const std::string& key) { return static_cast<JsonString*> (__get(key)); }
	JsonNumber*  get_number (const std::string& key) { return static_cast<JsonNumber*> (__get(key)); }
	JsonBoolean* get_boolean(const std::string& key) { return static_cast<JsonBoolean*>(__get(key)); }

	const JsonObject*  get_object (const std::string& key) const
	{ return static_cast<const JsonObject*> (__get(key)); }
	const JsonArray*   get_array  (const std::string& key) const
	{ return static_cast<const JsonArray*>  (__get(key)); }
	const JsonString*  get_string (const std::string& key) const
	{ return static_cast<const JsonString*> (__get(key)); }
	const JsonNumber*  get_number (const std::string& key) const
	{ return static_cast<const JsonNumber*> (__get(key)); }
	const JsonBoolean* get_boolean(const std::string& key) const
	{ return static_cast<const JsonBoolean*>(__get(key)); }

	std::string to_json_string() const override {
		bool is_only_one_simple=(__values.size()==1);
		for (std::map<std::string,JsonBase*>::const_iterator p=__values.begin();p!=__values.end();++p){
			if (!is_only_one_simple) break;
			if (!(p->second->is_simple())) {
				is_only_one_simple=false;
			}
		}

		std::string result;
		if (!is_only_one_simple) result+='{';
		for (std::map<std::string,JsonBase*>::const_iterator p=__values.begin();p!=__values.end();++p){
			const std::string& key=p->first;
			const JsonBase*  value=p->second;
			result+=key; result+=':';
			result+=value->to_json_string();
			result+=',';
		}
		if (!is_only_one_simple) result+='}';
		result.pop_back(); // to remove tail ','

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
	bool is_simple() const override { return false; }
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

	JsonObject*  get_object (size_t idx) { return static_cast<JsonObject*> (__get(idx)); }
	JsonArray*   get_array  (size_t idx) { return static_cast<JsonArray*>  (__get(idx)); }
	JsonString*  get_string (size_t idx) { return static_cast<JsonString*> (__get(idx)); }
	JsonNumber*  get_number (size_t idx) { return static_cast<JsonNumber*> (__get(idx)); }
	JsonBoolean* get_boolean(size_t idx) { return static_cast<JsonBoolean*>(__get(idx)); }

	const JsonObject*  get_object (size_t idx) const
	{ return static_cast<const JsonObject*> (__get(idx)); }
	const JsonArray*   get_array  (size_t idx) const
	{ return static_cast<const JsonArray*>  (__get(idx)); }
	const JsonString*  get_string (size_t idx) const
	{ return static_cast<const JsonString*> (__get(idx)); }
	const JsonNumber*  get_number (size_t idx) const
	{ return static_cast<const JsonNumber*> (__get(idx)); }
	const JsonBoolean* get_boolean(size_t idx) const
	{ return static_cast<const JsonBoolean*>(__get(idx)); }

	std::vector<JsonBase*> as_vector() { return __array; }

	const std::vector<JsonBase*> as_vector() const { return __array; }

	std::string to_json_string() const override {
		std::string result;

		result+='[';
		for (std::vector<JsonBase*>::const_iterator p=__array.begin();p!=__array.end();++p){
			result+=(*p)->to_json_string();
			result+=',';
		}
		result+=']';
		result.pop_back(); // to remove tail ','

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
	bool is_simple() const override { return false; }
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
	bool is_simple() const override { return true; }
private:
	std::string __string;
};
class JsonNumber :public JsonBase {
public:
	JsonNumber() :JsonBase(){}

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
	bool is_simple() const override { return true; }
private:
	double __number;
};
class JsonBoolean :public JsonBase {
public:
	JsonBoolean() :JsonBase() {}

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
	bool is_simple() const override { return true; }
private:
	bool __boolean;
};

__JSON_PARSER_END_NAMESPACE

#endif
