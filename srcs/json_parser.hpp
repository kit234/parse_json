/**
 * DATE: 2022/9/21
*/
/*
 * CAUTION: OBJECT MUST BE DELETED WHEN YOU USE CLONE FUNCTION TO CREATE OBJECT !!!
*/
#ifndef __JSON_PARSER_HPP__
#define __JSON_PARSER_HPP__

#include <map>
#include <string>
#include <stack>
#include <vector>
#include <cassert>

#ifndef __DEFINE_T_IF_T_IS_BASE_OF_JSONOBJECT
#define __DEFINE_T_IF_T_IS_BASE_OF_JSONOBJECT(T) template <typename T,typename=std::enable_if_t<std::is_base_of_v<JsonObject,std::remove_all_extents_t<T>>>>
#endif

#ifndef __ENABLE_IF_T_IS_BASE_OF_JSONOBJECT
#define __ENABLE_IF_T_IS_BASE_OF_JSONOBJECT(T,type) std::enable_if_t<std::is_base_of_v<JsonObject,std::remove_all_extents_t<T>>,type>
#endif

#ifndef __ENABLE_IF_T_IS
#define __ENABLE_IF_T_IS(T,sametype,type) std::enable_if_t<std::is_same_v<std::remove_all_extents_t<T>,sametype>,type>
#endif

#ifndef __HEADER_DEFINE_T_IF_T_IS_BASE_OF_JSONOBJECT
#define __HEADER_DEFINE_T_IF_T_IS_BASE_OF_JSONOBJECT template <typename,typename>
#endif

class JsonObject;
class JsonString;
__HEADER_DEFINE_T_IF_T_IS_BASE_OF_JSONOBJECT
class JsonArray;
class JsonNumber;
struct __TreeNode;

__DEFINE_T_IF_T_IS_BASE_OF_JSONOBJECT(T)
struct __json_object_add_node {
	void operator()(const T& obj,__TreeNode* root){
		obj.add_node(root);
	}
};

struct __TreeNode {
	friend class JsonParser;
public:
	__TreeNode& create_object(const std::string& key){
		insert_node(key);
		return *this;
	}
	__TreeNode& create_array(const std::string& key){
		insert_node(key);
		operator[](key).insert_node("\"0");
		return *this;
	}
	__TreeNode& create_and_into_last_elem(){
		assert(this->is_array_node());
		if (this->is_empty_array_node()){
			return *((next.begin())->second);
		}
		else {
			size_t idx=next.size();
			std::string key='"'+std::to_string(idx);
			return *insert_node(key);
		}
	}
	template <typename T>
	__ENABLE_IF_T_IS_BASE_OF_JSONOBJECT(T,__TreeNode&)
		push_back(const T& value)
	{
		assert(this->is_array_node());
		if (this->is_empty_array_node()){
			__json_object_add_node<T>()(value,(next.begin())->second);
		}
		else {
			size_t idx=(this->next).size();
			std::string key='"'+std::to_string(idx);
			__TreeNode* idx_node=new __TreeNode; idx_node->key=key;
			__json_object_add_node<T>()(value,idx_node);
			(this->next).insert(std::make_pair(key,idx_node));
		}
		return *this;
	}

	template <typename T>
	__ENABLE_IF_T_IS_BASE_OF_JSONOBJECT(T,__TreeNode&)
		add(const std::string& key,const T& value)
	{
		assert(next.count(key)==0);
		__TreeNode* new_node=insert_node(key);
		__json_object_add_node<T>()(value,new_node);
		return *this;
	}

	template <typename T>
	__ENABLE_IF_T_IS_BASE_OF_JSONOBJECT(T,void)
		set(const std::string& key,const T& value)
	{
		assert(next.count(key)>0);
		__TreeNode* node=next[key];
		node->release_childs();
		__json_object_add_node<T>()(value,node);
	}

	template <typename T>
	__ENABLE_IF_T_IS_BASE_OF_JSONOBJECT(T,void)
		set(const T& value)
	{
		release_childs();
		__json_object_add_node<T>()(value,this);
	}

	bool has_member(const std::string& key) const {
		return next.count(key)>0;
	}

	template <typename T>
	__ENABLE_IF_T_IS_BASE_OF_JSONOBJECT(T,T)
		as() const
	{
		T res; res.from_json_value(*this);
		return res;
	}
	template <typename T>
	__ENABLE_IF_T_IS(T,std::string,const std::string&)
		as() const
	{
		return (next.begin())->first;
	}
	template <typename T>
	__ENABLE_IF_T_IS(T,double,double)
		as() const
	{
		return stod((next.begin())->first);
	}
	template <typename T>
	__ENABLE_IF_T_IS(T,int,int)
		as() const
	{
		return stoi((next.begin())->first);
	}
	template <typename T>
	__ENABLE_IF_T_IS(T,bool,bool)
		as() const
	{
		return as<std::string>()=="true"?true:false;
	}

	size_t size() const {
		size_t count=0;
		for (auto& p:next){
			if ((p.second)->next.size()!=0) ++count;
		}
		return count;
	}
	__TreeNode& operator[](const std::string& key) {
		assert(next.count(key)>0);
		return *(next.at(key));
	}
	__TreeNode& operator[](size_t idx){
		assert(idx<size());
		return *(next.at('"'+std::to_string(idx)));
	}
	const __TreeNode& operator[](const std::string& key) const {
		assert(next.count(key)>0);
		return *(next.at(key));
	}
	const __TreeNode& operator[](size_t idx) const {
		assert(idx<size());
		return *(next.at('"'+std::to_string(idx)));
	}
	__TreeNode& move_out() {
		assert(prev!=nullptr);
		return *prev;
	}
	const __TreeNode& move_out() const {
		assert(prev!=nullptr);
		return *prev;
	}

private:
	__TreeNode* insert_node(const std::string& key){
		__TreeNode* new_node=new __TreeNode;
		new_node->key=key; new_node->prev=this;
		(new_node->next).clear();
		next.insert(make_pair(key,new_node));
		return new_node;
	}
	bool is_list_node() const {
		return key.front()=='"';
	}
	bool is_array_node() const {
		if (next.empty()) return false;
		for (auto& p:next){
			if (!(p.second->is_list_node())) return false;
		}
		return true;
	}
	bool is_empty_array_node() const {
		if (!is_array_node()) return false;
		if (next.size()>1)    return false;
		return this->size()==0;
	}
	// return true if this is String,Number,Boolean
	bool is_single_object() const {
		if (is_array_node()) return false;
		return (next.size())==0;
	}
	void release_childs(){
		for (auto& p:next){
			p.second->release_childs();
			delete p.second;
		}
		next.clear();
	}
public:
	std::string key;
	std::map<std::string,__TreeNode*> next;
	__TreeNode* prev;
};

class JsonParser {
public:
	using Value=__TreeNode;
public:
	JsonParser(JsonParser&& rhs) noexcept {
		_root=rhs._root;
		rhs._root=nullptr;
	}
	~JsonParser(){
		release(_root);
	}

	static JsonParser get_empty(){
		JsonParser reader;
		reader._root=new Value; reader._root->key="root"; reader._root->prev=nullptr;
		return reader;
	}
	static JsonParser from_string(const std::string& json){
		JsonParser reader; reader.parse(json);
		return reader;
	}

	std::string to_json_string() const {
		return nodes_to_string(_root->next);
	}
	Value& get_root() {
		assert(_root!=nullptr);
		return *_root;
	}
	JsonParser copy() const {
		return JsonParser::from_string(this->to_json_string());
	}
private:
	JsonParser() noexcept {}
	JsonParser(const JsonParser&)=delete;
	JsonParser& operator=(const JsonParser&)=delete;
public:
	JsonParser& operator=(JsonParser&& rhs) noexcept {
		release(_root);
		_root=rhs._root;
		rhs._root=nullptr;
		return *this;
	}
private:
	void parse(const std::string& json){
		if (!_root) release(_root);
		_root=new Value; _root->key="root"; _root->prev=nullptr;
		size_t l=0,count=json.size();
		std::string buffer;
		std::stack<Value*> node_stack;
		std::vector<int> brace_count;
		bool is_in_str=false;
		node_stack.push(_root);
		if (json.front()=='{') { ++l; --count; }
		if (json.back()=='}')  { --count; }
		for (char ch:json.substr(l,count)){
			if (ch=='"') is_in_str=!is_in_str;
			if (is_in_str) {
				buffer+=ch; continue;
			}
			if (ch==' ') continue;
			switch (ch){
			case ':': {
				Value* node=insert_node(buffer,node_stack,false);
				node_stack.push(node);
				break;
			}
			case '{': {
				if (!brace_count.empty()) ++(brace_count.back());
				break;
			}
			case '}': {
				if (!buffer.empty()) insert_node(buffer,node_stack,false);
				node_stack.pop();
				if (!brace_count.empty()) --(brace_count.back());
				break;
			}
			case '[': {
				brace_count.push_back(0);
				buffer="\"0";
				Value* node=insert_node(buffer,node_stack,true);
				node_stack.push(node);
				break;
			}
			case ']': {
				brace_count.pop_back();
				if (!buffer.empty()) insert_node(buffer,node_stack,false);
				node_stack.pop();
				break;
			}
			case ',': {
				if (!brace_count.empty()){
					if (!buffer.empty()) insert_node(buffer,node_stack,false);
					if (brace_count.back()>0) { node_stack.pop(); break; }
					int idx=get_cur_list_idx(node_stack)+1;
					node_stack.pop();
					buffer='"'+std::to_string(idx);
					Value* node=insert_node(buffer,node_stack,true);
					node_stack.push(node);
				}
				else {
					insert_node(buffer,node_stack,false);
					node_stack.pop();
				}
				break;
			}
			default: buffer+=ch;
			};
		}
	}
	void release(Value* node){
		if (!node) return;
		node->release_childs();
		delete node;
		node=nullptr;
	}
	Value* insert_node(std::string& buffer,std::stack<Value*>& node_stack,bool is_list){
		if (!is_list) fix_buffer(buffer);
		Value* node=(node_stack.top())->insert_node(buffer);
		buffer.clear();
		return node;
	}
	int get_cur_list_idx(const std::stack<Value*> node_stack){
		int cur_idx=stoi(((node_stack.top())->key).substr(1));
		return cur_idx;
	}
	void fix_buffer(std::string& buffer){
		size_t l=0,count=buffer.size();
		if (buffer.front()=='"') { ++l; --count; }
		if (buffer.back()=='"')  { --count; }
		buffer=buffer.substr(l,count);
	}
	std::string nodes_to_string(const std::map<std::string,Value*>& nodes) const {
		if (nodes.empty()) return std::string();
		bool is_list=true;
		for (auto& p:nodes){
			if (!(p.second)->is_list_node()){
				is_list=false; break;
			}
		}
		std::string result;
		if (is_list){
			result+='[';
			for (auto& p:nodes){
				if (p!=(*nodes.begin())) result+=',';
				result+=nodes_to_string((p.second)->next);
			}
			result+=']';
		}
		else {
			bool only_have_one_single=(nodes.size()==1); // only have one single object(String,Number,Boolean)
			for (auto& p:nodes){
				if (!only_have_one_single) break;
				if (!((p.second)->is_single_object())){
					only_have_one_single=false;
				}
			}
			if (!only_have_one_single) result+='{';
			for (auto& p:nodes){
				if (p!=(*nodes.begin())) result+=',';
				result+=('"'+p.first+'"');
				if ((p.second->next).size()>0) result+=':';
				result+=nodes_to_string((p.second)->next);
			}
			if (!only_have_one_single) result+='}';
		}
		return result;
	}
private:
	Value* _root;
};

class JsonObject {
	__HEADER_DEFINE_T_IF_T_IS_BASE_OF_JSONOBJECT
	friend struct __json_object_add_node;
	__HEADER_DEFINE_T_IF_T_IS_BASE_OF_JSONOBJECT
	friend class JsonArray; // to use JsonObject::add_node
public:
	virtual ~JsonObject(){ release(); }
	template <typename T>
	__ENABLE_IF_T_IS_BASE_OF_JSONOBJECT(T,void)
		define_field(const std::string& key)
	{
		assert(_values.count(key)==0);
		_values[key]=new T();
	}
	template <typename T>
	__ENABLE_IF_T_IS_BASE_OF_JSONOBJECT(T,T*)
		get_field(const std::string& key)
	{
		assert(_values.count(key)>0);
		return static_cast<T*>(_values.at(key));
	}
	template <typename T>
	__ENABLE_IF_T_IS_BASE_OF_JSONOBJECT(T,const T*)
		get_field(const std::string& key) const
	{
		assert(_values.count(key)>0);
		return static_cast<const T*>(_values.at(key));
	}
	virtual void set(JsonObject*)=0;
public:
	virtual std::string to_json_string() const {
		std::string result; result+='{';
		for (auto p=_values.begin();p!=_values.end();++p){
			if (p!=_values.begin()) result+=',';
			result+='"'+(p->first)+'"'; result+=':';
			result+=(p->second)->to_json_string();
		}
		result+='}';
		return result;
	}
	virtual void from_json_value(const JsonParser::Value& v){
		for (auto& field:_values){
			const std::string& key=field.first;
			(field.second)->from_json_value(v[key]);
		}
	}
	void from_string(const std::string& json){
		from_json_value(JsonParser::from_string(json).get_root());
	}
	JsonParser to_json_parser() const {
		JsonParser reader=JsonParser::from_string(to_json_string());
		return reader;
	}
	virtual JsonObject* clone()const=0;
	void release(){
		for (auto& p:_values) delete p.second;
		_values.clear();
	}
	virtual void add_node(__TreeNode* root) const {
		for (auto& p:_values){
			__TreeNode* node=new __TreeNode; node->key=p.first;
			(p.second)->add_node(node);
			(root->next).insert(make_pair(p.first,node));
		}
	};
protected:
	std::map<std::string,JsonObject*> _values;
};
class JsonString:public JsonObject {
	__HEADER_DEFINE_T_IF_T_IS_BASE_OF_JSONOBJECT
	friend struct __json_object_add_node;
	friend class JsonArray<JsonString,void>; // to use JsonString::add_node
public:
	JsonString()=default;
	JsonString(const char* str):_str(str){}
	JsonString(const std::string& str):_str(str){}
	JsonString(const JsonString& rhs):_str(rhs._str){}
	std::string to_json_string() const override { return ('"'+_str+'"'); }
	template <typename T>
	__ENABLE_IF_T_IS(T,std::string,std::string)
		as() const { return _str; }
	void set(JsonObject* obj) override { set((JsonString*)obj); }
	void set(const std::string& str) { _str=str; }
	void set(const char* str) { _str=str; }
	void set(const JsonString& rhs){ _str=rhs._str; }
	void set(const JsonString* rhs){ _str=rhs->_str; }
	void from_json_value(const JsonParser::Value& v) override {
		_str=v.as<std::string>();
	}
	JsonObject* clone() const override {
		return new JsonString(_str);
	}
private:
	void add_node(__TreeNode* root) const override {
		__TreeNode* node=new __TreeNode; node->key=_str;
		(root->next).insert(make_pair(_str,node));
	}
public:
	JsonString& operator=(const char* str) { _str=str; return *this; }
	JsonString& operator=(const std::string& str) { _str=str; return *this; }
	JsonString& operator=(const JsonString& rhs) { _str=rhs._str; return *this; }
	operator std::string(){ return _str; }
private:
	std::string _str;
};
__DEFINE_T_IF_T_IS_BASE_OF_JSONOBJECT(T)
class JsonArray:public JsonObject {
	__HEADER_DEFINE_T_IF_T_IS_BASE_OF_JSONOBJECT
	friend struct __json_object_add_node;
public:
	JsonArray(){}
	JsonArray(const std::initializer_list<T>& l){
		for (const T& e:l){
			_arr.push_back(new T(e));
		}
	}
	JsonArray(const std::initializer_list<T*>& l){
		for (T* e:l){
			_arr.push_back(e);
		}
	}
	~JsonArray(){
		release();
	}
	void push_back(const T& val) { _arr.push_back(new T(val)); }
	void push_back(T* val) { _arr.push_back(val); }
	void pop_back() { delete _arr.back(); _arr.pop_back(); }
	size_t size() const { return _arr.size(); }
	void clear() { release(); }
	T* at(size_t idx) { return _arr.at(idx); }
	const T* at(size_t idx) const { return _arr.at(idx); }

	std::string to_json_string() const override {
		std::string result="[";
		for (size_t i=0;i<_arr.size();++i){
			if (i!=0) result+=',';
			result+=_arr[i]->to_json_string();
		}
		result+=']';
		return result;
	}
	template <typename U>
	__ENABLE_IF_T_IS(U,std::vector<T*>,std::vector<T*>)
		as() const { return _arr; }

	void set(const std::vector<T*>& v){
		release();
		for (T* obj:v) _arr.push_back(obj);
	}
	void set(const std::vector<T>& v){
		release();
		for (const T& obj:v) _arr.push_back((T*)(obj.clone()));
	}
	template <typename U>
	void set(const std::vector<U>& v){
		release();
		for (const U& e:v){
			_arr.push_back(new T(e));
		}
	}
	void set(const std::initializer_list<T*>& v){
		release();
		for (T* e:v) _arr.push_back(e);
	}
	void set(const std::initializer_list<T>& v){
		release();
		for (const T& e:v) _arr.push_back((T*)(e.clone()));
	}
	template <typename U>
	void set(const std::initializer_list<U>& v){
		release();
		for (const U& e:v){
			_arr.push_back(new T(e));
		}
	}
	void set(const JsonArray<T>& rhs){
		release();
		for (T* obj:rhs._arr){
			_arr.push_back((T*)(obj->clone()));
		}
	}
	void set(const JsonArray<T>* rhs){
		release();
		for (T* obj:rhs->_arr)
			_arr.push_back(static_cast<T*>(obj->clone()));
	}
	void set(JsonObject* obj) override { set((JsonArray<T>*)obj); }
	void set(JsonArray<T>&& rhs){
		release();
		_arr=rhs._arr; rhs._arr.clear();
	}
	void from_json_value(const JsonParser::Value& v) override {
		release();
		for (size_t i=0;i<v.size();++i){
			T* r=new T; r->from_json_value(v[i]);
			_arr.push_back(r);
		}
	}
	JsonObject* clone() const override {
		JsonArray<T>* array=new JsonArray<T>;
		for (const T* obj:_arr)
			(array->_arr).push_back(static_cast<T*>(obj->clone()));
		return array;
	}
public:
	operator std::vector<T*>() const { return _arr; }
private:
	void release(){
		for (auto& e:_arr) delete e;
		_arr.clear();
	}
	void add_node(__TreeNode* root) const override {
		if (_arr.empty()){
			std::string idx="\"0";
			__TreeNode* node=new __TreeNode; node->key=idx;
			(root->next).insert(make_pair(idx,node));
		}
		for (size_t i=0;i<_arr.size();++i){
			std::string idx='"'+std::to_string(i);
			__TreeNode* node=new __TreeNode; node->key=idx;
			_arr[i]->add_node(node);
			(root->next).insert(make_pair(idx,node));
		}
	}
private:
	std::vector<T*> _arr;
};
class JsonNumber:public JsonObject {
	__HEADER_DEFINE_T_IF_T_IS_BASE_OF_JSONOBJECT
	friend struct __json_object_add_node;
	friend class JsonArray<JsonNumber,void>; // to use JsonNumber::add_node
public:
	JsonNumber():_number(0.0){}
	JsonNumber(double number):_number(number){}
	JsonNumber(const JsonNumber& rhs):_number(rhs._number){}
	std::string to_json_string() const override {
		return std::to_string(_number);
	}
	template <typename T>
	__ENABLE_IF_T_IS(T,double,double)
		as() const { return _number; }
	template <typename T>
	__ENABLE_IF_T_IS(T,int,int)
		as() const { return static_cast<int>(_number); }
	void set(double number) { _number=number; }
	void set(const JsonNumber& rhs) { _number=rhs._number; }
	void set(const JsonNumber* rhs) { _number=rhs->_number; }
	void set(JsonObject* obj) override { set((JsonNumber*)obj); }
	void from_json_value(const JsonParser::Value& v) override {
		_number=v.as<double>();
	}
	JsonObject* clone() const override {
		return new JsonNumber(_number);
	}
private:
	void add_node(__TreeNode* root) const override {
		std::string num=std::to_string(_number);
		__TreeNode* node=new __TreeNode; node->key=num;
		(root->next).insert(make_pair(num,node));
	}
public:
	operator double() const { return _number; }
	JsonNumber& operator=(double number) { _number=number; return *this; }
private:
	double _number;
};
class JsonBoolean: public JsonObject {
	__HEADER_DEFINE_T_IF_T_IS_BASE_OF_JSONOBJECT
	friend struct __json_object_add_node;
	friend class JsonArray<JsonBoolean,void>; // to use JsonBoolean::add_node
public:
	JsonBoolean() :_val(false){}
	JsonBoolean(bool val) :_val(val){}
	JsonBoolean(const JsonBoolean& rhs) :_val(rhs._val){}
	std::string to_json_string() const override { return _val?"true":"false"; }
	template <typename T>
	__ENABLE_IF_T_IS(T,bool,bool)
		as() const { return _val; }
	void set(bool val) { _val=val; }
	void set(const JsonBoolean& rhs) { _val=rhs._val; }
	void set(const JsonBoolean* rhs) { _val=rhs->_val; }
	void set(JsonObject* obj) { set((JsonBoolean*)obj); }
	void from_json_value(const JsonParser::Value& value) override {
		if (value.as<bool>()) _val=true;
		else                  _val=false;
	}
	JsonObject* clone() const override { return new JsonBoolean(_val); }
private:
	void add_node(__TreeNode* root) const override {
		std::string key=(_val?"true":"false");
		__TreeNode* node=new __TreeNode; node->key=key;
		(root->next).insert(make_pair(key,node));
	}
public:
	operator bool() const { return _val; }
	JsonBoolean& operator=(const JsonBoolean& rhs) { this->_val=rhs._val; return *this; }
private:
	bool _val;
};

#ifndef JSON_OBJECT
#define JSON_OBJECT(classname) \
	classname(){ define_fields(); } \
	classname(const classname& rhs) { define_fields(); set(&rhs); } \
	void set(const classname& rhs)  { set(&rhs); } \
	void set(const classname* rhs)  { for (auto& p:rhs->_values) { _values[p.first]->set(p.second); }} \
	void set(JsonObject* obj) override { set((classname*)obj); } \
	classname& operator=(const classname& rhs) { set(&rhs); return *this; } \
	JsonObject* clone() const override { classname* r=new classname; r->set(this); return r; }
#endif

#ifndef START_DEFINE_FIELD
#define START_DEFINE_FIELD void define_fields(){
#endif
#ifndef END_DEFINE_FIELD
#define END_DEFINE_FIELD }
#endif
#ifndef DEFINE_FIELD
#define DEFINE_FIELD(fieldtype,fieldname) define_field<fieldtype>(fieldname);
#endif

#ifndef SET_FIELD
#define SET_FIELD(varname,fieldtype,fieldname,...) \
	(varname)->get_field<fieldtype>(fieldname)->set(__VA_ARGS__);
#endif
#ifndef GET_FIELD
#define GET_FIELD(varname,fieldtype,fieldname) \
	((varname)->get_field<fieldtype>(fieldname))
#endif

#ifndef DEFINE_FIELDS
#define DEFINE_FIELDS define_fields();
#endif

#endif
