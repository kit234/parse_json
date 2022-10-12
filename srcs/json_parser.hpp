#ifndef __JSON_PARSER_HPP__
#define __JSON_PARSER_HPP__

#include <string>
#include <map>
#include <vector>

#define __JSON_PARSER_START_NAMESPACE namespace jsonparser {
#define __JSON_PARSER_END_NAMESPACE };

#if defined _MSC_VER
#define __JSON_PARSER_FORCEINLINE __forceinline
#elif defined __GNUC__
#define __JSON_PARSER_FORCEINLINE __inline__ __attribute__((always_inline))
#else
#define __JSON_PARSER_FORCEINLINE inline
#endif

__JSON_PARSER_START_NAMESPACE

class JsonBase;
class JsonObject;
class JsonArray;
class JsonString;
class JsonNumber;
class JsonBoolean;

template <typename T>
class __json_parser_create {
public:
	T* operator()(JsonBase* prev) const {
		T* res=new T();
		res->__set_prev(prev);
		return res;
	}
};

class JsonBase {
public:
	JsonBase() :__prev(nullptr){}
	virtual ~JsonBase(){}
	virtual std::string to_json_string() const=0;
	virtual void assign(const JsonBase&)=0;
	// clone self
	virtual JsonBase* clone() const=0;

	void __set_prev(JsonBase* prev) { __prev=prev; }

	JsonObject*  move_out_object  () { return (JsonObject* ) __prev; }
	JsonArray*   move_out_array   () { return (JsonArray*  ) __prev; }
	JsonString*  move_out_string  () { return (JsonString* ) __prev; }
	JsonNumber*  move_out_number  () { return (JsonNumber* ) __prev; }
	JsonBoolean* move_out_boolean () { return (JsonBoolean*) __prev; }
protected:
	JsonBase* __prev;
};
class JsonObject :public JsonBase {
public:
	JsonObject() :JsonBase(){}
	~JsonObject(){
		__release();
	}

	void erase(const std::string& key){
		delete __values[key];
		__values.erase(key);
	}

	JsonObject* create_object(const std::string& key){
		if (__values.count(key)>0) return nullptr;
		JsonObject* new_object=__json_parser_create<JsonObject>()(this);
		__values.insert(make_pair(key,(JsonBase*)new_object));
		return this;
	}

	JsonObject* create_array(const std::string& key){
		if (__values.count(key)>0) return nullptr;
		JsonArray* new_array=__json_parser_create<JsonArray>()(this);
		__values.insert(make_pair(key,(JsonBase*)new_array));
		return this;
	}

	bool has_field(const std::string& key) const {
		return __values.count(key)>0;
	}

	JsonObject* set(const std::string& key,JsonBase* obj){
		if (__values.count(key)>0){
			delete __values[key]; __values.erase(key);
		}
		obj->__set_prev(static_cast<JsonBase*>(this));
		__values.insert(std::make_pair(key,obj));
		return this;
	}

	JsonObject* out_object (const std::string& key,JsonObject**  target){
		if (*target!=nullptr) return this;
		*target=get_object(key);
		return this;
	}
	JsonObject* out_array  (const std::string& key,JsonArray**   target){
		if (*target!=nullptr) return this;
		*target=get_array(key);
		return this;
	}
	JsonObject* out_string (const std::string& key,JsonString**  target){
		if (*target!=nullptr) return this;
		*target=get_string(key);
		return this;
	}
	JsonObject* out_number (const std::string& key,JsonNumber**  target){
		if (*target!=nullptr) return this;
		*target=get_number(key);
		return this;
	}
	JsonObject* out_boolean(const std::string& key,JsonBoolean** target){
		if (*target!=nullptr) return this;
		*target=get_boolean(key);
		return this;
	}

	JsonObject*  get_object (const std::string& key) { return (JsonObject*) (__get(key)); }
	JsonArray*   get_array  (const std::string& key) { return (JsonArray*)  (__get(key)); }
	JsonString*  get_string (const std::string& key) { return (JsonString*) (__get(key)); }
	JsonNumber*  get_number (const std::string& key) { return (JsonNumber*) (__get(key)); }
	JsonBoolean* get_boolean(const std::string& key) { return (JsonBoolean*)(__get(key)); }

	const JsonObject*  get_object (const std::string& key) const
	{ return (const JsonObject* ) (__get(key)); }
	const JsonArray*   get_array  (const std::string& key) const
	{ return (const JsonArray*  ) (__get(key)); }
	const JsonString*  get_string (const std::string& key) const
	{ return (const JsonString* ) (__get(key)); }
	const JsonNumber*  get_number (const std::string& key) const
	{ return (const JsonNumber* ) (__get(key)); }
	const JsonBoolean* get_boolean(const std::string& key) const
	{ return (const JsonBoolean*) (__get(key)); }

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
	void assign(const JsonBase& r) override {
		if (this==&r) return;
		__release();
		const JsonObject& rhs=static_cast<const JsonObject&>(r);
		const std::map<std::string,JsonBase*>* rhs_values=&(rhs.__values);

		for (std::map<std::string,JsonBase*>::const_iterator p=rhs_values->begin();p!=rhs_values->end();++p){
			const std::string& key=p->first;
			const JsonBase*  value=p->second;
			(this->__values).insert(std::make_pair(key,value->clone()));
		}
		this->__prev=rhs.__prev;
	}
	JsonBase* clone() const override {
		JsonObject* result=__json_parser_create<JsonObject>()(this->__prev);

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
	JsonArray(const std::initializer_list<JsonBase*>& arr) :JsonBase(),__array(arr){}
	~JsonArray(){
		__release();
	}

	JsonArray* push_back(JsonBase* obj){
		obj->__set_prev(static_cast<JsonBase*>(this));
		__array.push_back(obj);
		return this;
	}

	JsonArray* pop_back(){
		delete __array.back();
		__array.pop_back();
		return this;
	}

	size_t size()   const { return __array.size(); }

	size_t length() const { return __array.size(); }

	JsonArray* erase(size_t idx){
		delete __array[idx];
		__array.erase(__array.begin()+idx);
		return this;
	}

	JsonArray* insert(size_t idx,JsonBase* obj){
		obj->__set_prev(static_cast<JsonBase*>(this));
		__array.insert(__array.begin()+idx,obj);
		return this;
	}

	JsonArray* clear(){
		__release();
		return this;
	}

	JsonArray* create_back_object(){
		JsonObject* new_object=__json_parser_create<JsonObject>()(this);
		__array.push_back((JsonBase*)new_object);
		return this;
	}
	JsonArray* create_back_array(){
		JsonArray* new_array=__json_parser_create<JsonArray>()(this);
		__array.push_back((JsonBase*)new_array);
		return this;
	}

	JsonArray* create_front_object(){
		JsonObject* new_object=__json_parser_create<JsonObject>()(this);
		__array.insert(__array.begin(),(JsonBase*)new_object);
		return this;
	}
	JsonArray* create_front_array(){
		JsonArray* new_array=__json_parser_create<JsonArray>()(this);
		__array.insert(__array.begin(),(JsonBase*)new_array);
		return this;
	}

	JsonArray* create_object(size_t idx){
		JsonObject* new_object=__json_parser_create<JsonObject>()(this);
		__array.insert(__array.begin()+idx,(JsonBase*)new_object);
		return this;
	}
	JsonArray* create_array(size_t idx){
		JsonArray* new_array=__json_parser_create<JsonArray>()(this);
		__array.insert(__array.begin()+idx,(JsonBase*)new_array);
		return this;
	}

	JsonArray* out_object (size_t idx,JsonObject**  target){
		if (*target!=nullptr) return this;
		*target=get_object(idx);
		return this;
	}
	JsonArray* out_array  (size_t idx,JsonArray**   target){
		if (*target!=nullptr) return this;
		*target=get_array(idx);
		return this;
	}
	JsonArray* out_string (size_t idx,JsonString**  target){
		if (*target!=nullptr) return this;
		*target=get_string(idx);
		return this;
	}
	JsonArray* out_number (size_t idx,JsonNumber**  target){
		if (*target!=nullptr) return this;
		*target=get_number(idx);
		return this;
	}
	JsonArray* out_boolean(size_t idx,JsonBoolean** target){
		if (*target!=nullptr) return this;
		*target=get_boolean(idx);
		return this;
	}

	JsonObject*  get_back_object()  { return (JsonObject*)  __array.back(); }
	JsonArray*   get_back_array ()  { return (JsonArray*)   __array.back(); }
	JsonString*  get_back_string()  { return (JsonString*)  __array.back(); }
	JsonNumber*  get_back_number()  { return (JsonNumber*)  __array.back(); }
	JsonBoolean* get_back_boolean() { return (JsonBoolean*) __array.back(); }

	const JsonObject*  get_back_object () const
	{ return (const JsonObject* ) (__array.back()); }
	const JsonArray*   get_back_array  () const
	{ return (const JsonArray*  ) (__array.back()); }
	const JsonString*  get_back_string () const
	{ return (const JsonString* ) (__array.back()); }
	const JsonNumber*  get_back_number () const
	{ return (const JsonNumber* ) (__array.back()); }
	const JsonBoolean* get_back_boolean() const
	{ return (const JsonBoolean*) (__array.back()); }

	JsonObject*  get_front_object () { return (JsonObject* ) (__array.front()); }
	JsonArray*   get_front_array  () { return (JsonArray*  ) (__array.front()); }
	JsonString*  get_front_string () { return (JsonString* ) (__array.front()); }
	JsonNumber*  get_front_number () { return (JsonNumber* ) (__array.front()); }
	JsonBoolean* get_front_boolean() { return (JsonBoolean*) (__array.front()); }

	const JsonObject*  get_front_object () const
	{ return (const JsonObject* ) (__array.front()); }
	const JsonArray*   get_front_array  () const
	{ return (const JsonArray*  ) (__array.front()); }
	const JsonString*  get_front_string () const
	{ return (const JsonString* ) (__array.front()); }
	const JsonNumber*  get_front_number () const
	{ return (const JsonNumber* ) (__array.front()); }
	const JsonBoolean* get_front_boolean() const
	{ return (const JsonBoolean*) (__array.front()); }

	JsonObject*  get_object (size_t idx) { return (JsonObject*) (__get(idx)); }
	JsonArray*   get_array  (size_t idx) { return (JsonArray*)  (__get(idx)); }
	JsonString*  get_string (size_t idx) { return (JsonString*) (__get(idx)); }
	JsonNumber*  get_number (size_t idx) { return (JsonNumber*) (__get(idx)); }
	JsonBoolean* get_boolean(size_t idx) { return (JsonBoolean*)(__get(idx)); }

	const JsonObject*  get_object (size_t idx) const
	{ return (const JsonObject* ) (__get(idx)); }
	const JsonArray*   get_array  (size_t idx) const
	{ return (const JsonArray*  ) (__get(idx)); }
	const JsonString*  get_string (size_t idx) const
	{ return (const JsonString* ) (__get(idx)); }
	const JsonNumber*  get_number (size_t idx) const
	{ return (const JsonNumber* ) (__get(idx)); }
	const JsonBoolean* get_boolean(size_t idx) const
	{ return (const JsonBoolean*) (__get(idx)); }

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
	void assign(const JsonBase& r) override {
		if (this==&r) return;
		__release();
		const JsonArray& rhs=static_cast<const JsonArray&>(r);
		const std::vector<JsonBase*>* rhs_array=&(rhs.__array);

		for (std::vector<JsonBase*>::const_iterator p=rhs_array->begin();p!=rhs_array->end();++p){
			(this->__array).push_back((*p)->clone());
		}
		this->__prev=rhs.__prev;
	}
	JsonBase* clone() const override {
		JsonArray* result=__json_parser_create<JsonArray>()(this->__prev);

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

	JsonString* assign(const std::string& str) {
		this->__string=str;
		return this;
	}

	std::string to_json_string() const override {
		std::string result;

		result+='"'; result+=__string; result+='"';

		return result;
	}
	void assign(const JsonBase& r) override {
		if (this==&r) return;
		const JsonString& rhs=static_cast<const JsonString&>(r);

		this->__string=rhs.__string;
		this->__prev=rhs.__prev;
	}
	JsonBase* clone() const override {
		JsonString* result=__json_parser_create<JsonString>()(this->__prev);

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

	JsonNumber* assign(double number){
		this->__number=number;
		return this;
	}

	double as_double() const { return static_cast<double>(__number); }
	int    as_int()    const { return static_cast<int>   (__number); }
	float  as_float()  const { return static_cast<float> (__number); }

	std::string to_json_string() const override {
		std::string result;

		result+=std::to_string(__number);

		return result;
	}
	void assign(const JsonBase& r) override {
		if (this==&r) return;
		const JsonNumber& rhs=static_cast<const JsonNumber&>(r);

		this->__number=rhs.__number;
		this->__prev=rhs.__prev;
	}
	JsonBase* clone() const override {
		JsonNumber* result=__json_parser_create<JsonNumber>()(this->__prev);

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

	JsonBoolean* assign(bool boolean){
		this->__boolean=boolean;
		return this;
	}

	bool as_bool() const { return __boolean; }

	std::string to_json_string() const override {
		std::string result;

		if (__boolean) result="true" ;
		else           result="false";

		return result;
	}
	void assign(const JsonBase& r) override {
		if (this==&r) return;
		const JsonBoolean& rhs=static_cast<const JsonBoolean&>(r);

		this->__boolean=rhs.__boolean;
		this->__prev=rhs.__prev;
	}
	JsonBase* clone() const override {
		JsonBoolean* result=__json_parser_create<JsonBoolean>()(this->__prev);

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
		return __parse_json(json,0,temp,nullptr);
	}
	static JsonObject* parse_to_object(const char* json){
		return (JsonObject*)parse(json);
	}
	static JsonArray* parse_to_array(const char* json){
		return (JsonArray*)parse(json);
	}
	static JsonString* parse_to_string(const char* json){
		return (JsonString*)parse(json);
	}
	static JsonNumber* parse_to_number(const char* json){
		return (JsonNumber*)parse(json);
	}
	static JsonBoolean* parse_to_boolean(const char* json){
		return (JsonBoolean*)parse(json);
	}
private:
	static JsonBase* __parse_json_object(const char* json,size_t l,size_t& new_idx,JsonBase* prev){
		JsonObject* result=__json_parser_create<JsonObject>()(prev);
		size_t idx=l+1;
		std::string key; key.clear();
		while (json[idx]!='}'){
			if (json[idx]==',') { ++idx; continue; }
			if (key.empty()&&__is_blank(json,idx)) { ++idx; continue; }
			if (json[idx]==':'){
				size_t new_idx=0;
				key=key.substr(1,key.size()-2); // remove the '"' of key
				result->set(key,__parse_json(json,idx+1,new_idx,static_cast<JsonBase*>(result)));
				key.clear();
				idx=new_idx+1; continue;
			}
			key+=json[idx];
			++idx;
		}
		new_idx=idx;
		return static_cast<JsonBase*>(result);
	}
	static JsonBase* __parse_json_array(const char* json,size_t l,size_t& new_idx,JsonBase* prev){
		JsonArray* result=__json_parser_create<JsonArray>()(prev);
		size_t idx=l+1;
		while (json[idx]!=']'){
			if (json[idx]==',') { ++idx; continue; }
			size_t new_idx=0;
			result->push_back(__parse_json(json,idx,new_idx,static_cast<JsonBase*>(result)));
			idx=new_idx+1;
		}
		new_idx=idx;
		return static_cast<JsonBase*>(result);
	}
	static JsonBase* __parse_json_string(const char* json,size_t l,size_t& new_idx,JsonBase* prev){
		std::string temp; temp.clear();
		size_t idx=l+1;
		while (json[idx]!='"'){
			temp+=json[idx]; ++idx;
		}
		new_idx=idx;
		JsonString* result=__json_parser_create<JsonString>()(prev);
		result->assign(temp);
		return static_cast<JsonBase*>(result);
	}
	static JsonBase* __parse_json_number(const char* json,size_t l,size_t& new_idx,JsonBase* prev){
		std::string temp; temp.clear();
		size_t idx=l;
		while (__is_digit(json[idx])||json[idx]=='.'||json[idx]=='e'){
			temp+=json[idx]; ++idx;
		}
		new_idx=idx-1;
		JsonNumber* result=__json_parser_create<JsonNumber>()(prev);
		result->assign(std::stod(temp));
		return static_cast<JsonBase*>(new JsonNumber(std::stod(temp)));
	}
	static JsonBase* __parse_json_boolean(const char* json,size_t l,size_t& new_idx,JsonBase* prev){
		JsonBoolean* result=__json_parser_create<JsonBoolean>()(prev);
		if (json[l]=='t'){
			new_idx=l+3;
			result->assign(true);
		}
		else{
			new_idx=l+4;
			result->assign(false);
		}
		return static_cast<JsonBase*>(result);
	}

	static JsonBase* __parse_json(const char* json,size_t l,size_t& new_idx,JsonBase* prev){
		while (__is_blank(json,l)) ++l;
		char ch=json[l];
		if (ch=='{')             return __parse_json_object (json,l,new_idx,prev);
		if (ch=='[')             return __parse_json_array  (json,l,new_idx,prev);
		if (ch=='"')             return __parse_json_string (json,l,new_idx,prev);
		if (__is_digit(json[l])) return __parse_json_number (json,l,new_idx,prev);
		if (ch=='t'||ch=='f')    return __parse_json_boolean(json,l,new_idx,prev);
		return nullptr;
	}

	static inline bool __is_blank(const char* json,size_t l){
		return (json[l]==' ')||(json[l]=='\t')||(json[l]=='\r\n')||(json[l]=='\n');
	}
	static inline bool __is_digit(char ch){
		return ch>='0'&&ch<='9';
	}
};

__JSON_PARSER_FORCEINLINE JsonBase*    operator"" _json        (const char* s,size_t){
	return JsonParser::parse(s);
}
__JSON_PARSER_FORCEINLINE JsonObject*  operator"" _json_object (const char* s,size_t){
	return JsonParser::parse_to_object(s);
}
__JSON_PARSER_FORCEINLINE JsonArray*   operator"" _json_array  (const char* s,size_t){
	return JsonParser::parse_to_array(s);
}
__JSON_PARSER_FORCEINLINE JsonString*  operator"" _json_string (const char* s,size_t){
	return JsonParser::parse_to_string(s);
}
__JSON_PARSER_FORCEINLINE JsonNumber*  operator"" _json_number (const char* s,size_t){
	return JsonParser::parse_to_number(s);
}
__JSON_PARSER_FORCEINLINE JsonBoolean* operator"" _json_boolean(const char* s,size_t){
	return JsonParser::parse_to_boolean(s);
}

__JSON_PARSER_FORCEINLINE JsonNumber*  operator"" _json_number(unsigned long long i){
	JsonNumber* res=__json_parser_create<JsonNumber>()(nullptr);
	res->assign(static_cast<double>(i));
	return res;
}
__JSON_PARSER_FORCEINLINE JsonNumber*  operator"" _json_number(long double i){
	JsonNumber* res=__json_parser_create<JsonNumber>()(nullptr);
	res->assign(static_cast<double>(i));
	return res;
}

__JSON_PARSER_FORCEINLINE JsonBoolean* operator"" _json_boolean(unsigned long long b){
	JsonBoolean* res=__json_parser_create<JsonBoolean>()(nullptr);
	res->assign((b!=0.0));
	return res;
}
__JSON_PARSER_FORCEINLINE JsonBoolean* operator"" _json_boolean(long double b){
	JsonBoolean* res=__json_parser_create<JsonBoolean>()(nullptr);
	res->assign((b!=0.0));
	return res;
}

__JSON_PARSER_END_NAMESPACE

#endif
