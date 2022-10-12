#ifndef __JSON_PARSER_HPP__
#define __JSON_PARSER_HPP__


#define __JSON_PARSER_NAMESPACE_START namespace kit {
#define __JSON_PARSER_NAMESPACE_END   }

#if defined _MSC_VER
#define __JSON_PARSER_FORCEINLINE __forceinline
#elif defined __GNUC__
#define __JSON_PARSER_FORCEINLINE __inline__ __attribute__((always_inline))
#else
#define __JSON_PARSER_FORCEINLINE inline
#endif

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <exception>

__JSON_PARSER_NAMESPACE_START

template <typename T>
__JSON_PARSER_FORCEINLINE
void __destroy_at(T* ptr){
#if (__cplusplus>=201703L)
	std::destroy_at(ptr);
#else
	ptr->~T();
#endif
}

/*
 * JsonClass
*/
template <
	template <typename,typename> typename ObjType,
	template <typename>          typename ArrType,
	typename StrType,
	typename NumType,
	typename BooleanType
>
class JsonClass {

	using Self=JsonClass<ObjType,ArrType,StrType,NumType,BooleanType>;

	union {
		ObjType<StrType,Self*> __object;
		ArrType<Self*>         __array;
		StrType                __string;
		NumType                __number;
		BooleanType            __boolean;
	};

	enum class Type {
		OBJECT,ARRAY,STRING,NUMBER,BOOLEAN,NONE
	} __type;
public:
	JsonClass() :__type(Type::NONE){}
	JsonClass(const Self& r){
		__construct_from_type(r.__type);
		__assign(&r);
	}
	JsonClass(Self&& r){
		__construct_from_type(r.__type);
		__move(&r);
	}
	~JsonClass(){
		__release();
	}

	Self* clone() const {
		Self* res=new Self();
		res->__construct_from_type(this->__type);
		res->__assign(this);
		return res;
	}

	static Self object(){
		Self res; res.__construct_from_type(Type::OBJECT);
		return res;
	}
	static Self array(){
		Self res; res.__construct_from_type(Type::ARRAY);
		return res;
	}
	static Self string(){
		Self res; res.__construct_from_type(Type::STRING);
		return res;
	}
	static Self number(){
		Self res; res.__construct_from_type(Type::NUMBER);
		return res;
	}
	static Self boolean(){
		Self res; res.__construct_from_type(Type::BOOLEAN);
		return res;
	}
	static Self null(){
		Self res; ers.__construct_from_type(Type::NONE);
		return res;
	}
public:
	Self& operator=(const Self& r){
		__release();
		__construct_from_type(r.__type);
		__assign(&r);
		return *this;
	}
	Self& operator=(Self&& r){
		__release();
		__construct_from_type(r.__type);
		__move(&r);
		return *this;
	}
private:
	void __release(){
		switch (__type){
		case Type::OBJECT:
			__release_object();  break;
		case Type::ARRAY:
			__release_array();   break;
		case Type::STRING:
			__release_string();  break;
		case Type::NUMBER:
			__release_number();  break;
		case Type::BOOLEAN:
			__release_boolean(); break;
		default: break;
		}
	}
	void __release_object(){
		for (auto& p:__object){
			delete p.second;
		}
		__destroy_at(&(__object));
	}
	void __release_array(){
		for (auto& p:__array){
			delete p;
		}
		__destroy_at(&(__array));
	}
	void __release_string(){
		__destroy_at(&(__string));
	}
	void __release_number(){
		__destroy_at(&(__number));
	}
	void __release_boolean(){
		__destroy_at(&(__boolean));
	}

	void __construct_from_type(const Type& type){
		this->__type=type;
		switch (this->__type){
		case Type::OBJECT:
			__construct_object();  break;
		case Type::ARRAY:
			__construct_array();   break;
		case Type::STRING:
			__construct_string();  break;
		case Type::NUMBER:
			__construct_number();  break;
		case Type::BOOLEAN:
			__construct_boolean(); break;
		default: break;
		}
	}
	void __construct_object(){
		new (&(__object)) ObjType<StrType,Self*>();
	}
	void __construct_array(){
		new (&(__array)) ArrType<Self*>();
	}
	void __construct_string(){
		new (&(__string)) StrType();
	}
	void __construct_number(){
		new (&(__number)) NumType();
	}
	void __construct_boolean(){
		new (&(__boolean)) BooleanType();
	}

	void __assign(Self* r){
		this->__type=r.__type;
		switch (r.__type){
		case Type::OBJECT:
			__assign_object(r); break;
		case Type::ARRAY:
			__assign_array(r); break;
		case Type::STRING:
			__assign_string(r); break;
		case Type::NUMBER:
			__assign_number(r); break;
		case Type::BOOLEAN:
			__assign_boolean(); break;
		default: break;
		}
	}
	void __assign_object(Self* r){
		for (auto& p:r->__object){
			const StrType& key=p.first;
			Self* value=p.second;
			(this->__object).insert(std::make_pair(key,value->clone()));
		}
	}
	void __assign_array(Self* r){
		for (auto& p:r->__array){
			(this->__array).push_back(p->clone());
		}
	}
	void __assign_string(Self* r){
		this->__string=r->__string;
	}
	void __assign_number(Self* r){
		this->__number=r->__number;
	}
	void __assign_boolean(Self* r){
		this->__boolean=r->__boolean;
	}

	void __move(Self* r){
		this->__type=r->__type;
		switch (r->__type){
		case Type::OBJECT:
			__move_object(r); break;
		case Type::ARRAY:
			__move_array(r); break;
		case Type::STRING:
			__move_string(r); break;
		case Type::NUMBER:
			__move_number(r); break;
		case Type::BOOLEAN:
			__move_boolean(); break;
		default: break;
		}
		r->__type=Type::NONE;
	}
	void __move_object(Self* r){
		this->__object=std::move(r->__object);
	}
	void __move_array(Self* r){
		this->__array=std::move(r->__array);
	}
	void __move_string(Self* r){
		this->__string=std::move(r->__string);
	}
	void __move_number(Self* r){
		this->__number=std::move(r->__number);
	}
	void __move_boolean(Self* r){
		this->__boolean=std::move(r->__boolean);
	}
};


/*
 * JsonParser
*/
template <
	template <typename,typename> typename ObjType,
	template <typename>          typename ArrType,
	typename StrType,
	typename NumType,
	typename BooleanType
>
class JsonParser {    // TODO: add Exception
	using Json=JsonClass<ObjType,ArrType,StrType,NumType,BooleanType>;
public:
	static Json parse(const char* json){
		size_t tmp;
		return __parse(json,0,tmp);
	}
private:
	static Json __parse(const char* json,size_t l,size_t& new_idx){
		while (__is_blank(json[l])) ++l;
		if (json[l]=='{')
			return __parse_object(json,l,new_idx);
		if (json[l]=='[')
			return __parse_array(json,l,new_idx);
		if (json[l]=='"')
			return __parse_string(json,l,new_idx);
		if (__is_digit(json[l]))
			return __parse_number(json,l,new_idx);
		if (json[l]=='t'||json[l]=='f')
			return __parse_boolean(json,l,new_idx);
		if (json[l]=='n')
			return __parse_null(json,l,new_idx);
		return Json::null();
	}
	static Json __parse_object(const char* json,size_t l,size_t& new_idx){

	}
	static Json __parse_array(const char* json,size_t l,size_t& new_idx){

	}
	static Json __parse_string(const char* json,size_t l,size_t& new_idx){
		Json res=Json::string();
		new_idx=l+1;
		while (json[new_idx]!='"'){
			res.__string+=json[new_idx]; ++new_idx;
		}
		return res;
	}
	static Json __parse_number(const char* json,size_t l,size_t& new_idx){

	}
	static Json __parse_boolean(const char* json,size_t l,size_t& new_idx){

	}
	static Json __parse_null(const char* json,size_t l,size_t& new_idx){

	}

	static inline bool __is_digit(char ch){
		return ch>='0'&&ch<='9';
	}
	static inline bool __is_blank(char ch){
		return ch==' '||ch=='\n'||ch=='\t';
	}

	static inline double __string2double(const StrType& str){

	}
};

template <typename T,typename U>
using __default_objtype=std::map<T,U>;
template <typename T>
using __default_arrtype=std::vector<T>;
using __default_strtype=std::string;
using __default_numtype=double;
using __default_booleantype=bool;

using Json=JsonClass<__default_objtype,
					 __default_arrtype,
					 __default_strtype,
					 __default_numtype,
                     __default_booleantype>;

using Parser=JsonParser<__default_objtype,
	                    __default_arrtype,
	                    __default_strtype,
	                    __default_numtype,
	                    __default_booleantype>;


__JSON_PARSER_NAMESPACE_END

#endif
