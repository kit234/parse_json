#ifndef __JSON_PARSER_HPP__
#define __JSON_PARSER_HPP__


/*
 *	Declare Macro
*/
#define __JSON_PARSER_NAMESPACE_START namespace kit {
#define __JSON_PARSER_NAMESPACE_END   }

/*	__JSON_PARSER_CPP17: defined if c++17 or later	*/
#if (__cplusplus>=201703L)
#define __JSON_PARSER_CPP17
#endif

#if defined _MSC_VER
#define __JSON_PARSER_FORCE_INLINE __forceinline
#elif defined __GNUC__
#define __JSON_PARSER_FORCE_INLINE __inline__ __attribute__((always_inline))
#else
#define __JSON_PARSER_FORCE_INLINE inline
#endif

#define __JSON_PARSER_NUMBER_COMPATIBLE unsigned int,int,long,unsigned long,float,double,unsigned long long,long double
#define __JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE(T) \
	std::enable_if_t<is_number_compatible_v<__remove_extra_t<T>>,T>
#define __JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE_U(T,U) \
	std::enable_if_t<is_number_compatible_v<__remove_extra_t<T>>,U>

#if defined __JSON_PARSER_CPP17
#define __JSON_PARSER_STRING_COMPATIBLE std::string,const char*
#define __JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE(T) \
	std::enable_if_t<is_string_compatible_v<__remove_extra_t<T>>,T>
#else
#define __JSON_PARSER_STRING_COMPATIBLE_NO_CONSTCHAR std::string
#define __JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_NO_CONSTCHAR(T) \
	std::enable_if_t<is_string_compatible_v<__remove_extra_t<T>>&&(!std::is_same_v<T,const char*>)&&(!std::is_same_v<T,const char[]>),T>
#define __JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_CONSTCHAR(T) \
	std::enable_if_t<std::is_same_v<T,const char*>,const char*>
#endif
#define __JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_U(T,U) \
	std::enable_if_t<is_string_compatible_v<__remove_extra_t<T>>,U>

#define __JSON_PARSER_BOOLEAN_COMPATIBLE bool
#define __JSON_PARSER_ENABLE_IF_BOOLEAN_COMPATIBLE(T) \
	std::enable_if_t<is_boolean_compatible_v<__remove_extra_t<T>>,T>
#define __JSON_PARSER_ENABLE_IF_BOOLEAN_COMPATIBLE_U(T,U) \
	std::enable_if_t<is_boolean_compatible_v<__remove_extra_t<T>>,U>

#include <map>
#include <vector>
#include <string>
#include <exception>

#if defined __JSON_PARSER_CPP17
#include <memory>
#endif

__JSON_PARSER_NAMESPACE_START

template <typename T>
__JSON_PARSER_FORCE_INLINE
void __destroy_at(T* ptr){
#if defined __JSON_PARSER_CPP17
	std::destroy_at(ptr);
#else
	ptr->~T();
#endif
}

/*
*	__is_one_of: if the T is one of the types.
*/
template <typename T,typename U,typename... Ts>
struct __is_one_of {
	static constexpr bool value=(std::is_same_v<T,U>)||(__is_one_of<T,Ts...>::value);
};
template <typename T,typename U>
struct __is_one_of<T,U> {
	static constexpr bool value=std::is_same_v<T,U>;
};

/*
 *	__remove_extra: remove the (const,pointer,reference) of type.
*/
template <typename T>
struct __remove_extra {
	using type=std::remove_const_t<std::remove_reference_t<std::remove_pointer_t<T>>>;
};
template <>
struct __remove_extra<const char*> {
	using type=const char*;
};
template <typename T>
using __remove_extra_t=typename __remove_extra<T>::type;

/*
 *	__is_number_compatible: if the T is the number compatible
*/
template <typename T>
struct is_number_compatible {
	static constexpr bool value=__is_one_of<T,__JSON_PARSER_NUMBER_COMPATIBLE>::value;
};
template <typename T>
constexpr bool is_number_compatible_v=is_number_compatible<T>::value;

/*
 *	is_string_compatible: if the T is string compatible
*/
#if defined __JSON_PARSER_CPP17
template <typename T>
struct is_string_compatible {
	static constexpr bool value=__is_one_of<T,__JSON_PARSER_STRING_COMPATIBLE>::value;
};
#else
template <typename T>
struct is_string_compatible {
	static constexpr bool value=__is_one_of<T,__JSON_PARSER_STRING_COMPATIBLE_NO_CONSTCHAR>::value;
};
template <>
struct is_string_compatible<const char*> {
	static constexpr bool value=true;
};
#endif
template <typename T>
constexpr bool is_string_compatible_v=is_string_compatible<T>::value;

/*
 *	is_boolean_compatible: if the T is boolean compatible
*/
template <typename T>
struct is_boolean_compatible {
	static constexpr bool value=__is_one_of<T,__JSON_PARSER_BOOLEAN_COMPATIBLE>::value;
};
template <typename T>
constexpr bool is_boolean_compatible_v=is_boolean_compatible<T>::value;

/*
 *	Declare
*/
template <template <typename,typename> typename,
	      template <typename>          typename,
	      typename,typename,typename,typename>
class Json;
template <template <typename,typename> typename,
	      template <typename>          typename,
	      typename,typename,typename,typename>
class JsonParser;

/*
 *	Defalut Type
*/
template <typename T,typename U>
using default_objtype=std::map<T,U>;
template <typename T>
using default_arrtype=std::vector<T>;
using default_strtype=std::string;
using default_numtype=double;
using default_booleantype=bool;
class default_convertion {
public:
	std::string operator()(double num){
		return std::to_string(num);
	}
	double operator()(const std::string& str){
		return std::stod(str);
	}
};

/*
 *	ParserException
*/
class ParserException :public std::exception {
public:
	ParserException(const std::string& msg)
		: std::exception(msg.c_str()){}
};
/*
 *	JsonClassException
*/
class JsonClassException:public std::exception {
public:
	JsonClassException(const std::string& msg)
		: std::exception(msg.c_str()){}
};

/*
 * Json
*/
template <
	template <typename,typename> typename ObjType=default_objtype,
	template <typename>          typename ArrType=default_arrtype,
	typename StrType=default_strtype,
	typename NumType=default_numtype,
	typename BooleanType=default_booleantype,
	typename convertion=default_convertion
>
class Json {
	using Self=Json<ObjType,ArrType,StrType,NumType,BooleanType>;

	friend class JsonParser<ObjType,ArrType,StrType,NumType,BooleanType,convertion>;

	friend Self operator""__json(const char*,size_t);
	friend Self operator""__json(unsigned long long);
	friend Self operator""__json(long double);

	union {
		ObjType<StrType,Self*> __object;
		ArrType<Self*>         __array;
		StrType                __string;
		NumType                __number;
		BooleanType            __boolean;
	};

	Self* __prev;
public:
	enum class Type {
		OBJECT,ARRAY,STRING,NUMBER,BOOLEAN,NONE
	} __type;
public:
	Json() :__type(Type::NONE),__prev(nullptr){}
	Json(const Self& r){
		__construct_from_type(r.__type);
		__assign(&r);
	}
	Json(Self&& r){
		__construct_from_type(r.__type);
		__move(&r);
	}
	~Json(){
		__release();
	}

	/*
	 *	Other Construct
	*/
	// TODO: Other Construct

	/*
	 *	Iterator
	*/
	// TODO: Iterator

	Type get_type() const { return __type; }

	Self* clone() const {
		Self* res=new Self();
		res->__construct_from_type(this->__type);
		res->__assign(this);
		return res;
	}
	Self* move_to_heap() {
		Self* res=new Self();
		res->__construct_from_type(this->__type);
		res->__move(this);
		return res;
	}
	Self& move_out()       { return *__prev; }
	Self& move_out() const { return *__prev; }

	bool is_object () const { return __type==Type::OBJECT ; }
	bool is_array  () const { return __type==Type::ARRAY  ; }
	bool is_string () const { return __type==Type::STRING ; }
	bool is_number () const { return __type==Type::NUMBER ; }
	bool is_boolean() const { return __type==Type::BOOLEAN; }

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
		Self res; res.__construct_from_type(Type::NONE);
		return res;
	}

	StrType dump() const {
		switch (this->__type){
		case Type::OBJECT:
			return __dump_object();
		case Type::ARRAY:
			return __dump_array();
		case Type::STRING:
			return __dump_string();
		case Type::NUMBER:
			return __dump_number();
		case Type::BOOLEAN:
			return __dump_boolean();
		case Type::NONE:
			return __dump_null();
		default: break;
		}
		// Imposible
		return StrType();
	}
public:
	/*
	 *	as
	*/
	template <typename T>
	__JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE(T)
		as()
	{
		__check_type(Type::NUMBER);
		return static_cast<T>(__number);
	}
#if defined __JSON_PARSER_CPP17
	template <typename T>
	__JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE(T)
		as() const
	{
		__check_type(Type::STRING);
		if constexpr (std::is_same_v<T,const char*>){
			return __string.c_str();
		}
		else {
			return static_cast<T>(__string);
		}
	}
#else
	template <typename T>
	__JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_NO_CONSTCHAR(T)
		as() const
	{
		__check_type(Type::STRING);
		return static_cast<T>(__string);
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_CONSTCHAR(T)
		as() const
	{
		__check_type(Type::STRING);
		return __string.c_str();
	}
#endif
	template <typename T>
	__JSON_PARSER_ENABLE_IF_BOOLEAN_COMPATIBLE(T)
		as() const
	{
		__check_type(Type::BOOLEAN);
		return static_cast<T>(__boolean);
	}

	/*
	 *	at
	*/
	Self& at(const StrType& key){
		__check_type(Type::OBJECT);
		return *(__object[key]);
	}
	const Self& at(const StrType& key) const {
		__check_type(Type::OBJECT);
		return *(__object.at(key));
	}
	Self& at(size_t idx){
		__check_type(Type::ARRAY);
		return *(__array[idx]);
	}
	const Self& at(size_t idx) const {
		return *(__array.at(idx));
	}

	/*
	 *	size
	*/
	size_t size() const {
		__check_type_is_one_of(Type::OBJECT,Type::ARRAY,Type::STRING);
		switch (__type){
		case Type::OBJECT:
			return __object.size();
		case Type::ARRAY:
			return __array.size();
		case Type::STRING:
			return __string.size();
		default: break;
		}
		// Imposible
		return 0;
	}

	/*
	 *	clear
	*/
	void clear(){
		__check_type_is_one_of(Type::OBJECT,Type::ARRAY,Type::STRING);
		switch (__type){
		case Type::OBJECT:
			__object.clear(); break;
		case Type::ARRAY:
			__array.clear();  break;
		case Type::STRING:
			__string.clear(); break;
		default: break;
		}
	}

	/*
	 *	insert
	*/
	Self& insert(const StrType& key){
		__construct_type_if_null(Type::OBJECT);
		__check_type(Type::OBJECT);
		__check_key_exist(key);
		Self* obj=Self::null().move_to_heap(); obj->__prev=this;
		__object.insert(std::make_pair(key,obj));
		return *obj;
	}
	Self& insert(const StrType& key,const Self& value){
		__construct_type_if_null(Type::OBJECT);
		__check_type(Type::OBJECT);
		__check_key_exist(key);
		__object.insert(std::make_pair(key,value.clone()));
		return *this;
	}
	Self& insert(const StrType& key,Self&& value){
		__construct_type_if_null(Type::OBJECT);
		__check_type(Type::OBJECT);
		__check_key_exist(key);
		__object.insert(std::make_pair(key,value.move_to_heap()));
		return *this;
	}
#if defined __JSON_PARSER_CPP17
	template <typename T>
	Self& insert(const StrType& key,const T& value){
		__construct_type_if_null(Type::OBJECT);
		__check_type(Type::OBJECT);
		__check_key_exist(key);
		Self* obj=nullptr;
		if constexpr (is_string_compatible_v<T>){
			obj=Self::string().move_to_heap();
			obj->__string=StrType(value);
		}
		if constexpr (is_number_compatible_v<T>){
			obj=Self::number().move_to_heap();
			obj->__number=NumType(value);
		}
		if constexpr (is_boolean_compatible_v<T>){
			obj=Self::number().move_to_heap();
			obj->__number=BooleanType(value);
		}
		if (obj!=nullptr) {
			obj->__prev=this;
			__object.insert(std::make_pair(key,obj));
		}
		return *this;
	}
	template <typename T>
	Self& insert(const StrType& key,T&& value){
		__construct_type_if_null(Type::OBJECT);
		__check_type(Type::OBJECT);
		__check_key_exist(key);
		Self* obj=nullptr;
		if constexpr (is_string_compatible_v<T>){
			obj=Self::string().move_to_heap();
			obj->__string=StrType(std::move(value));
		}
		if constexpr (is_number_compatible_v<T>){
			obj=Self::number().move_to_heap();
			obj->__number=NumType(std::move(value));
		}
		if constexpr (is_boolean_compatible_v<T>){
			obj=Self::number().move_to_heap();
			obj->__number=BooleanType(std::move(value));
		}
		if (obj!=nullptr) {
			obj->__prev=this;
			__object.insert(std::make_pair(key,obj));
		}
		return *this;
	}
#else
	template <typename T>
	__JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_U(T,Self&)
		insert(const StrType& key,const T& value)
	{
		__construct_type_if_null(Type::OBJECT);
		__check_type(Type::OBJECT);
		__check_key_exist(key);
		Self* obj=Self::string().move_to_heap(); obj->__prev=this;
		obj->__string=StrType(value);
		__object.insert(std::make_pair(key,obj));
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_U(T,Self&)
		insert(const StrType& key,T&& value)
	{
		__construct_type_if_null(Type::OBJECT);
		__check_type(Type::OBJECT);
		__check_key_exist(key);
		Self* obj=Self::string().move_to_heap(); obj->__prev=this;
		obj->__string=StrType(std::move(value));
		__object.insert(std::make_pair(key,obj));
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE_U(T,Self&)
		insert(const StrType& key,const T& value)
	{
		__construct_type_if_null(Type::OBJECT);
		__check_type(Type::OBJECT);
		__check_key_exist(key);
		Self* obj=Self::number().move_to_heap(); obj->__prev=this;
		obj->__number=NumType(value);
		__object.insert(std::make_pair(key,obj));
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE_U(T,Self&)
		insert(const StrType& key,T&& value)
	{
		__construct_type_if_null(Type::OBJECT);
		__check_type(Type::OBJECT);
		__check_key_exist(key);
		Self* obj=Self::number().move_to_heap(); obj->__prev=this;
		obj->__number=NumType(std::move(value));
		__object.insert(std::make_pair(key,obj));
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_BOOLEAN_COMPATIBLE_U(T,Self&)
		insert(const StrType& key,const T& value)
	{
		__construct_type_if_null(Type::OBJECT);
		__check_type(Type::OBJECT);
		__check_key_exist(key);
		Self* obj=Self::boolean().move_to_heap(); obj->__prev=this;
		obj->__boolean=BooleanType(value);
		__object.insert(std::make_pair(key,obj));
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_BOOLEAN_COMPATIBLE_U(T,Self&)
		insert(const StrType& key,T&& value)
	{
		__construct_type_if_null(Type::OBJECT);
		__check_type(Type::OBJECT);
		__check_key_exist(key);
		Self* obj=Self::boolean().move_to_heap(); obj->__prev=this;
		__object.insert(std::make_pair(key,obj));
		return *this;
	}
#endif // __JSON_PARSER_CPP17
	// can't convert const char&[n] to const char*
	Self& insert(const StrType& key,const char* value){
		__construct_type_if_null(Type::OBJECT);
		__check_type(Type::OBJECT);
		__check_key_exist(key);
		Self* obj=Self::string().move_to_heap(); obj->__prev=this;
		obj->__string=StrType(value);
		__object.insert(std::make_pair(key,obj));
		return *this;
	}

	Self& insert(size_t idx){
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::null().move_to_heap(); obj->__prev=this;
		__array.insert(__array.begin()+idx,obj);
		return *obj;
	}
	Self& insert(size_t idx,const Self& value){
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		__array.insert(__array.begin()+idx,value.clone());
		return *this;
	}
	Self& insert(size_t idx,Self&& value){
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		__array.insert(__array.begin()+idx,value.move_to_heap());
		return *this;
	}
#if defined __JSON_PARSER_CPP17
	template <typename T>
	Self& insert(size_t idx,const T& value){
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=nullptr;
		if constexpr (is_string_compatible_v<T>){
			obj=Self::string().move_to_heap();
			obj->__string=StrType(value);
		}
		if constexpr (is_number_compatible_v<T>){
			obj=Self::number().move_to_heap();
			obj->__number=NumType(value);
		}
		if constexpr (is_boolean_compatible_v<T>){
			obj=Self::boolean().move_to_heap();
			obj->__boolean=BooleanType(value);
		}
		if (obj!=nullptr){
			obj->__prev=this;
			__array.insert(__array.begin()+idx,obj);
		}
		return *this;
	}
	template <typename T>
	Self& insert(size_t idx,T&& value){
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=nullptr;
		if constexpr (is_string_compatible_v<T>){
			obj=Self::string().move_to_heap();
			obj->__string=StrType(std::move(value));
		}
		if constexpr (is_number_compatible_v<T>){
			obj=Self::number().move_to_heap();
			obj->__number=NumType(std::move(value));
		}
		if constexpr (is_boolean_compatible_v<T>){
			obj=Self::boolean().move_to_heap();
			obj->__boolean=BooleanType(std::move(value));
		}
		if (obj!=nullptr){
			obj->__prev=this;
			__array.insert(__array.begin()+idx,obj);
		}
		return *this;
	}
#else
	template <typename T>
	__JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_U(T,Self&)
		insert(size_t idx,const T& value)
	{
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::string().move_to_heap(); obj->__prev=this;
		obj->__string=StrType(value);
		__array.insert(__array.begin()+idx,obj);
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_U(T,Self&)
		insert(size_t idx,T&& value)
	{
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::string().move_to_heap(); obj->__prev=this;
		obj->__string=StrType(std::move(value));
		__array.insert(__array.begin()+idx,obj);
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE_U(T,Self&)
		insert(size_t idx,const T& value)
	{
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::number().move_to_heap(); obj->__prev=this;
		obj->__number=NumType(value);
		__array.insert(__array.begin()+idx,obj);
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE_U(T,Self&)
		insert(size_t idx,T&& value)
	{
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::number().move_to_heap(); obj->__prev=this;
		obj->__number=NumType(std::move(value));
		__array.insert(__array.begin()+idx,obj);
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_BOOLEAN_COMPATIBLE_U(T,Self&)
		insert(size_t idx,const T& value)
	{
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::boolean().move_to_heap(); obj->__prev=this;
		obj->__boolean=BooleanType(value);
		__array.insert(__array.begin()+idx,obj);
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_BOOLEAN_COMPATIBLE_U(T,Self&)
		insert(size_t idx,T&& value)
	{
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::boolean().move_to_heap(); obj->__prev=this;
		obj->__boolean=BooleanType(std::move(value));
		__array.insert(__array.begin()+idx,obj);
		return *this;
	}
#endif // __JSON_PARSER_CPP17
	// can't convert const char&[n] to const char*
	Self& insert(size_t idx,const char* value)
	{
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::string().move_to_heap(); obj->__prev=this;
		obj->__string=StrType(value);
		__array.insert(__array.begin()+idx,obj);
		return *this;
	}

	/*
	 *	push_back
	*/
	Self& push_back(){
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::null().move_to_heap(); obj->__prev=this;
		__array.push_back(obj);
		return *obj;
	}
	Self& push_back(const Self& value){
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		__array.push_back(value.clone());
		return *this;
	}
	Self& push_back(Self&& value){
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		__array.push_back(value.move_to_heap());
		return *this;
	}
#if defined __JSON_PARSER_CPP17
	template <typename T>
	Self& push_back(const T& value){
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=nullptr;
		if constexpr (is_string_compatible_v<T>){
			obj=Self::string().move_to_heap();
			obj->__string=StrType(value);
		}
		if constexpr (is_number_compatible_v<T>){
			obj=Self::number().move_to_heap();
			obj->__number=NumType(value);
		}
		if constexpr (is_boolean_compatible_v<T>){
			obj=Self::boolean().move_to_heap();
			obj->__boolean=BooleanType(value);
		}
		if (obj!=nullptr){
			obj->__prev=nullptr;
			__array.push_back(obj);
		}
		return *this;
	}
	template <typename T>
	Self& push_back(T&& value){
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=nullptr;
		if constexpr (is_string_compatible_v<T>){
			obj=Self::string().move_to_heap();
			obj->__string=StrType(std::move(value));
		}
		if constexpr (is_number_compatible_v<T>){
			obj=Self::number().move_to_heap();
			obj->__number=NumType(std::move(value));
		}
		if constexpr (is_boolean_compatible_v<T>){
			obj=Self::boolean().move_to_heap();
			obj->__boolean=BooleanType(std::move(value));
		}
		if (obj!=nullptr){
			obj->__prev=nullptr;
			__array.push_back(obj);
		}
		return *this;
	}
#else
	template <typename T>
	__JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_U(T,Self&)
		push_back(const T& value)
	{
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::string().move_to_heap(); obj->__prev=this;
		obj->__string=StrType(value);
		__array.push_back(obj);
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_U(T,Self&)
		push_back(T&& value)
	{
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::string().move_to_heap(); obj->__prev=this;
		obj->__string=StrType(std::move(value));
		__array.push_back(obj);
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE_U(T,Self&)
		push_back(const T& value)
	{
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::number().move_to_heap(); obj->__prev=this;
		obj->__number=NumType(value);
		__array.push_back(obj);
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE_U(T,Self&)
		push_back(T&& value)
	{
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::number().move_to_heap(); obj->__prev=this;
		obj->__number=NumType(std::move(value));
		__array.push_back(obj);
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_BOOLEAN_COMPATIBLE_U(T,Self&)
		push_back(const T& value)
	{
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::boolean().move_to_heap(); obj->__prev=this;
		obj->__number=BooleanType(value);
		__array.push_back(obj);
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_BOOLEAN_COMPATIBLE_U(T,Self&)
		push_back(T&& value)
	{
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::boolean().move_to_heap(); obj->__prev=this;
		obj->__number=BooleanType(std::move(value));
		__array.push_back(obj);
		return *this;
	}
#endif // __JSON_PARSER_CPP17
	// can't convert const char&[n] to const char*
	Self& push_back(const char* value){
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		Self* obj=Self::string().move_to_heap(); obj->__prev=this;
		obj->__string=StrType(value);
		__array.push_back(obj);
		return *this;
	}

	/*
	 *	pop_back
	*/
	Self& pop_back(){
		__check_type(Type::ARRAY);
		__array.pop_back();
		return *this;
	}

	/*
	 *	erase
	*/
	Self& erase(const StrType& key){
		__check_type(Type::OBJECT);
		__object.erase(key);
		return *this;
	}
	Self& erase(size_t idx){
		__check_type(Type::ARRAY);
		__array.erase(__array.begin()+idx);
		return *this;
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

#if defined __JSON_PARSER_CPP17
	template <typename T>
	Self& operator=(const T& value){
		__release();
		if constexpr (is_string_compatible_v<T>){
			__construct_from_type(Type::STRING);
			__string=StrType(value);
		}
		if constexpr (is_number_compatible_v<T>){
			__construct_from_type(Type::NUMBER);
			__number=NumType(value);
		}
		if constexpr (is_boolean_compatible_v<T>){
			__construct_from_type(Type::BOOLEAN);
			__boolean=BooleanType(value);
		}
		return *this;
	}
	template <typename T>
	Self& operator=(T&& value){
		__release();
		if constexpr (is_string_compatible_v<T>){
			__construct_from_type(Type::STRING);
			__string=StrType(std::move(value));
		}
		if constexpr (is_number_compatible_v<T>){
			__construct_from_type(Type::NUMBER);
			__number=NumType(std::move(value));
		}
		if constexpr (is_boolean_compatible_v<T>){
			__construct_from_type(Type::BOOLEAN);
			__boolean=BooleanType(std::move(value));
		}
		return *this;
	}
#else
	template <typename T>
	__JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE_U(T,Self&)
		operator=(const T& num)
	{
		__release();
		__construct_from_type(Type::NUMBER);
		__number=NumType(num);
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE_U(T,Self&)
		operator=(T&& num)
	{
		__release();
		__construct_from_type(Type::NUMBER);
		__number=NumType(std::move(num));
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_U(T,Self&)
		operator=(const T& str)
	{
		__release();
		__construct_from_type(Type::STRING);
		__string=StrType(str);
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_U(T,Self&)
		operator=(T&& str)
	{
		__release();
		__construct_from_type(Type::STRING);
		__string=StrType(std::move(str));
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_BOOLEAN_COMPATIBLE_U(T,Self&)
		operator=(const T& boolean)
	{
		__release();
		__construct_from_type(Type::BOOLEAN);
		__boolean=BooleanType(boolean);
		return *this;
	}
	template <typename T>
	__JSON_PARSER_ENABLE_IF_BOOLEAN_COMPATIBLE_U(T,Self&)
		operator=(T&& boolean)
	{
		__release();
		__construct_from_type(Type::BOOLEAN);
		__boolean=BooleanType(std::move(boolean));
		return *this;
	}
#endif // __JSON_PARSER_CPP17
	// can't convertion const char&[n] to const char*
	Self& operator=(const char* str){
		__release();
		__construct_from_type(Type::STRING);
		__string=StrType(str);
		return *this;
	}

	/*
	 *	operator[]
	*/
	Self& operator[](const StrType& key){
		__construct_type_if_null(Type::OBJECT);
		__check_type(Type::OBJECT);
		if (__object.count(key)==0){
			Self* null_obj=null().move_to_heap();
			null_obj->__prev=this;
			__object.insert(std::make_pair(key,null_obj));
		}
		return *(__object[key]);
	}
	const Self& operator[](const StrType& key) const {
		__check_type(Type::OBJECT);
		return *(__object.at(key));
	}

	Self& operator[](size_t idx){
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		return *(__array[idx]);
	}
	const Self& operator[](size_t idx) const {
		__check_type(Type::ARRAY);
		return *(__array.at(idx));
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
		new (&(__object))  ObjType<StrType,Self*>();
	}
	void __construct_array(){
		new (&(__array))   ArrType<Self*>();
	}
	void __construct_string(){
		new (&(__string))  StrType();
	}
	void __construct_number(){
		new (&(__number))  NumType();
	}
	void __construct_boolean(){
		new (&(__boolean)) BooleanType();
	}

	void __assign(const Self* r){
		this->__type=r->__type;
		this->__prev=r->__prev;
		switch (r->__type){
		case Type::OBJECT:
			__assign_object(r);  break;
		case Type::ARRAY:
			__assign_array(r);   break;
		case Type::STRING:
			__assign_string(r);  break;
		case Type::NUMBER:
			__assign_number(r);  break;
		case Type::BOOLEAN:
			__assign_boolean(r); break;
		default: break;
		}
	}
	void __assign_object(const Self* r){
		for (auto& p:r->__object){
			const StrType& key=p.first;
			Self* value=p.second;
			(this->__object).insert(std::make_pair(key,value->clone()));
		}
	}
	void __assign_array(const Self* r){
		for (auto& p:r->__array){
			(this->__array).push_back(p->clone());
		}
	}
	void __assign_string(const Self* r){
		this->__string=r->__string;
	}
	void __assign_number(const Self* r){
		this->__number=r->__number;
	}
	void __assign_boolean(const Self* r){
		this->__boolean=r->__boolean;
	}

	void __move(Self* r){
		this->__type=r->__type;
		this->__prev=r->__prev;
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
			__move_boolean(r); break;
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

	StrType __dump_object() const {
		StrType res;
		res+='{';
		for (auto& p:__object){
			res+='"'; res+=p.first; res+='"';
			res+=':';
			res+=(p.second)->dump();
			res+=',';
		}
		res.pop_back(); // remove tail ','
		res+='}';
		return res;
	}
	StrType __dump_array() const {
		StrType res;
		res+='[';
		for (auto& p:__array){
			res+=(p->dump());
			res+=',';
		}
		res.pop_back(); // remove tail ','
		res+=']';
		return res;
	}
	StrType __dump_string() const {
		StrType res;
		res+='"'; res+=__string; res+='"';
		return res;
	}
	StrType __dump_number() const {
		return convertion()(__number);
	}
	StrType __dump_boolean() const {
		if (__boolean)
			return StrType("true");
		else
			return StrType("false");
	}
	StrType __dump_null() const {
		return StrType("null");
	}

	void __check_type(const Type& type) const {
		if (this->__type!=type){
			std::string err_msg=std::string("Check Type Error: ")+__type_to_string(__type)+
				std::string(" want: ")+__type_to_string(type);
			throw JsonClassException(err_msg);
		}
	}

	template <typename... Ts>
	void __check_type_is_one_of(const Ts&... types) const {
		if (!__return_true_if_type_is_one_of(this->__type,types...))
			throw JsonClassException("Check Type Error");
	}
	template <typename T,typename... Ts>
	std::enable_if_t<std::is_same_v<T,Type>,bool>
		__return_true_if_type_is_one_of(const T& tf,const Ts&... types) const
	{
		return (this->__type==tf)||
			   (__return_true_if_type_is_one_of(this->__type,types...));
	}
	template <typename T>
	std::enable_if_t<std::is_same_v<T,Type>,bool>
		__return_true_if_type_is_one_of(const T& tf) const
	{
		return (this->__type==tf);
	}

	void __construct_type_if_null(const Type& type) {
		if (this->__type==Type::NONE)
			__construct_from_type(type);
	}

	void __check_key_exist(const StrType& key){
		if (__object.count(key)>0){
			std::string err_msg=std::string("Key Exist: ")+key;
			throw JsonClassException(err_msg);
		}
	}

	std::string __type_to_string(const Type& type){
		switch (type){
		case Type::OBJECT : return "OBJECT";
		case Type::ARRAY  : return "ARRAY" ;
		case Type::STRING : return "STRING";
		case Type::NUMBER : return "NUMBER";
		case Type::BOOLEAN: return "BOOLEAN";
		case Type::NONE   : return "NULL";
		}
	}
};

/*
 * JsonParser
*/
template <
	template <typename,typename> typename ObjType=default_objtype,
	template <typename>          typename ArrType=default_arrtype,
	typename StrType=default_strtype,
	typename NumType=default_numtype,
	typename BooleanType=default_booleantype,
	typename convertion=default_convertion
>
class JsonParser {
	using JsonClass=Json<ObjType,ArrType,StrType,NumType,BooleanType,convertion>;
public:
	static JsonClass parse(const char* json){
		size_t tmp=0,n=std::strlen(json);
		JsonClass res=__parse(json,0,tmp,n);
		while ((++tmp)<n){
			if (!__is_blank(json[tmp]))
				throw ParserException("Json String Too Long");
		}
		return res;
	}
private:
	static JsonClass __parse(const char* json,size_t l,size_t& new_idx,size_t n){
		__skip_blank(json,l,n);
		if (json[l]=='{')
			return __parse_object(json,l,new_idx,n);
		if (json[l]=='[')
			return __parse_array(json,l,new_idx,n);
		if (json[l]=='"')
			return __parse_string(json,l,new_idx,n);
		if (__is_digit(json[l])||json[l]=='-')
			return __parse_number(json,l,new_idx,n);
		if (json[l]=='t'||json[l]=='f')
			return __parse_boolean(json,l,new_idx,n);
		if (json[l]=='n')
			return __parse_null(json,l,new_idx,n);
		std::string err_msg=std::string("Parser Unknown Character: ")+json[l]
			+std::string("idx: ")+std::to_string(l);
		throw ParserException(err_msg);
	}
	static JsonClass __parse_object(const char* json,size_t l,size_t& new_idx,size_t n){
		JsonClass res=JsonClass::object();
		StrType key;
		new_idx=l+1;
		while (json[new_idx]!='}'){
			if (json[new_idx]==','){
				__peek(json,new_idx,n);
				__skip_blank(json,new_idx,n);
				__check_prefix(json[new_idx]);
				continue;
			}
			if (key.empty()){
				__skip_blank(json,new_idx,n);
			}
			if (json[new_idx]==':'){
				size_t tmp_new_idx=0;
				__peek(json,new_idx,n);
				__check_and_fix_key(key);
				JsonClass* obj=__parse(json,new_idx,tmp_new_idx,n).move_to_heap();
				obj->__prev=&res;
				res.__object.insert(std::make_pair(key,obj));
				key.clear();
				new_idx=tmp_new_idx+1;
				__skip_blank(json,new_idx,n);
				continue;
			}
			key+=json[new_idx]; __peek(json,new_idx,n);
		}
		if (!key.empty()){
			throw ParserException("Key Not Empty");
		}
		return res;
	}
	static JsonClass __parse_array(const char* json,size_t l,size_t& new_idx,size_t n){
		JsonClass res=JsonClass::array();
		new_idx=l+1;
		while (json[new_idx]!=']'){
			if (json[new_idx]==','){
				__peek(json,new_idx,n);
				__skip_blank(json,new_idx,n);
				__check_prefix(json[new_idx]);
				continue;
			}
			size_t tmp_new_idx=0;
			JsonClass* obj=__parse(json,new_idx,tmp_new_idx,n).move_to_heap();
			obj->__prev=&res;
			res.__array.push_back(obj);
			new_idx=tmp_new_idx+1;
			__skip_blank(json,new_idx,n);
		}
		return res;
	}
	static JsonClass __parse_string(const char* json,size_t l,size_t& new_idx,size_t n){
		JsonClass res=JsonClass::string();
		new_idx=l+1;
		while (json[new_idx]!='"'){
			res.__string+=json[new_idx];
			__peek(json,new_idx,n);
		}
		__check_string(res.__string);
		return res;
	}
	static JsonClass __parse_number(const char* json,size_t l,size_t& new_idx,size_t n){
		JsonClass res=JsonClass::number();
		StrType tmp;
		new_idx=l;
		auto is_number_char=[](char ch){
			return __is_digit(ch)||ch=='.'||ch=='e'||ch=='-'||ch=='E'||ch=='+';
		};
		while ((new_idx<n)&&is_number_char(json[new_idx])){
			tmp+=json[new_idx]; __peek(json,new_idx,n);
		}
		__check_number(tmp);
		--new_idx;
		res.__number=convertion()(tmp);
		return res;
	}
	static JsonClass __parse_boolean(const char* json,size_t l,size_t& new_idx,size_t n){
		JsonClass res=JsonClass::boolean();
		new_idx=l;
		if (json[new_idx]=='t'){
			__peek(json,new_idx,n,'t');
			__peek(json,new_idx,n,'r');
			__peek(json,new_idx,n,'u');
			__peek(json,new_idx,n,'e');
			res.__boolean=true;
		}
		else {
			__peek(json,new_idx,n,'f');
			__peek(json,new_idx,n,'a');
			__peek(json,new_idx,n,'l');
			__peek(json,new_idx,n,'s');
			__peek(json,new_idx,n,'e');
			res.__boolean=false;
		}
		--new_idx;
		return res;
	}
	static JsonClass __parse_null(const char* json,size_t l,size_t& new_idx,size_t n){
		new_idx=l;
		__peek(json,new_idx,n,'n');
		__peek(json,new_idx,n,'u');
		__peek(json,new_idx,n,'l');
		__peek(json,new_idx,n,'l');
		--new_idx;
		return JsonClass::null();
	}

	static inline bool __is_digit(char ch){
		return ch>='0'&&ch<='9';
	}
	static inline bool __is_blank(char ch){
		return ch==' '||ch=='\n'||ch=='\t'||ch=='\r\n';
	}
	static void __skip_blank(const char* json,size_t& idx,size_t n){
		while (__is_blank(json[idx])){
			__peek(json,idx,n);
		}
	}
	static void __peek(const char* json,size_t& idx,size_t n,char ch){
		if (idx>=n){
			throw ParserException("Index Overstep the Boundary");
		}
		if (json[idx]!=ch){
			std::string err_msg=std::string("Parser Unknown Character: '")+json[idx]
				+std::string("' want: '")+ch+std::string("' idx: ")+std::to_string(idx);
			throw ParserException(err_msg);
		}
		++idx;
	}
	static void __peek(const char* json,size_t& idx,size_t n){
		if (idx>=n)
			throw ParserException("Index Overstep the Boundary");
		++idx;
	}

	static void __check_and_fix_key(StrType& key){
		if (key[0]!='"'||key.back()!='"'){
			std::string err_msg=std::string("Parser Error Key: ")+std::string(key);
			throw ParserException(err_msg);
		}
		key=key.substr(1,key.size()-2);
		__check_string(key);
	}

	static void __check_number(const StrType& num){
		bool is_first_number=true;
		if (num.size()==1) return;
		for (size_t i=0;i<num.size();++i){
			if (num[i]=='0'){
				if (is_first_number&&num[i+1]!='.'){
					std::string err_msg=std::string("number cannot have leading zeroes: ")+
						std::string(num);
					throw ParserException(err_msg);
				}
			}
			is_first_number=false;
		}
	}

	static void __check_string(const StrType& str){
		for (size_t i=0;i<str.size();++i){
			if (str[i]=='\\'){
				std::string err_msg=std::string("Illegal backslash escape ")+std::string(str);
				if (i>=str.size()-1||!__check_backslash(str[i+1]))
					throw ParserException(err_msg);
			}
			if (str[i]=='\t'){
				throw ParserException("tab character in string "+std::string(str));
			}
			if (str[i]=='\n'||str[i]=='\r\n'){
				throw ParserException("string line break "+std::string(str));
			}
		}
	}
	static bool __check_backslash(char ch){
		if (ch=='\\') return true;
		if (ch=='"')  return true;
		if (ch=='b')  return true;
		if (ch=='f')  return true;
		if (ch=='n')  return true;
		if (ch=='r')  return true;
		if (ch=='t')  return true;
		if (ch=='/')  return true;
		if (ch=='u')  return true;
		return false;
	}

	static void __check_prefix(char ch){
		if (ch=='{') return;
		if (ch=='[') return;
		if (ch=='"') return;
		if (__is_digit(ch)||ch=='-') return;
		if (ch=='t'||ch=='f') return;
		if (ch=='n') return;
		std::string err_msg=std::string("Error Prefix: '")+ch+std::string("'");
		throw ParserException(err_msg);
	}
};

/*
 *	literal
*/
__JSON_PARSER_FORCE_INLINE
Json<> operator""__json(const char* json,size_t){
	return JsonParser<>::parse(json);
}
__JSON_PARSER_FORCE_INLINE
Json<> operator""__json(unsigned long long num){
	Json<> res=Json<>::number();
	res.__number=default_numtype(num);
	return res;
}
__JSON_PARSER_FORCE_INLINE
Json<> operator""__json(long double num){
	Json<> res=Json<>::number();
	res.__number=default_numtype(num);
	return res;
}

template <template <typename,typename> typename ObjType=default_objtype,
	      template <typename>          typename ArrType=default_arrtype,
	      typename StrType=default_strtype,
	      typename NumType=default_numtype,
	      typename BooleanType=default_booleantype,
	      typename convertion=default_convertion>
std::ostream& operator<<(std::ostream& os,
						 const Json<ObjType,ArrType,StrType,NumType,BooleanType,convertion>& obj)
{
	os<<obj.dump();
	return os;
}

template <template <typename,typename> typename ObjType=default_objtype,
	template <typename>                typename ArrType=default_arrtype,
	typename StrType=default_strtype,
	typename NumType=default_numtype,
	typename BooleanType=default_booleantype,
	typename convertion=default_convertion>
std::istream& operator>>(std::istream& is,
						 Json<ObjType,ArrType,StrType,NumType,BooleanType,convertion>& obj)
{
	std::string json; is>>json;
	obj=JsonParser<ObjType,ArrType,StrType,NumType,BooleanType,convertion>::parse(json.c_str());
	return is;
}

__JSON_PARSER_NAMESPACE_END

#endif
