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

#include <cstdint>

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

 *	__is_string_const_array: if the T is const char*(&)[N]
*/
template <typename T>
struct __is_string_const_array :std::false_type {};
template <size_t N>
struct __is_string_const_array<const char(&)[N]> :std::true_type {};
template <typename T>
constexpr bool __is_string_const_array_v=__is_string_const_array<T>::value;

/*
*	__is_one_of: if the T is one of the types.
*/
template <typename T,typename U,typename... Ts>
struct __is_one_of
	:std::bool_constant<std::is_same_v<T,U>||__is_one_of<T,Ts...>::value>
{};
template <typename T,typename U>
struct __is_one_of<T,U>
	:std::is_same<T,U>
{};


/*
 *	__remove_extra: remove the (const,pointer,reference) of type.
*/
template <typename T>

struct __remove_extra
	:std::remove_const<std::remove_reference_t<std::remove_pointer_t<T>>>
{};
template <>
struct __remove_extra<const char*>
{
	using type=const char*;
};
template <size_t N>
struct __remove_extra<const char(&)[N]>
{
	using type=const char(&)[N];
};

template <typename T>
using __remove_extra_t=typename __remove_extra<T>::type;

/*
 *	__is_number_compatible: if the T is the number compatible
*/
template <typename T>

struct is_number_compatible
	:__is_one_of<T,__JSON_PARSER_NUMBER_COMPATIBLE>
{};

template <typename T>
constexpr bool is_number_compatible_v=is_number_compatible<T>::value;

/*
 *	is_string_compatible: if the T is string compatible
*/
#if defined __JSON_PARSER_CPP17
template <typename T>

struct is_string_compatible
	:std::bool_constant<__is_one_of<T,__JSON_PARSER_STRING_COMPATIBLE>::value||
		                __is_string_const_array_v<T>>
{};
#else
template <typename T>
struct is_string_compatible
	:std::bool_constant<__is_one_of<T,__JSON_PARSER_STRING_COMPATIBLE_NO_CONSTCHAR>::value||
	                    __is_string_const_array_v<T>>
{};
template <>
struct is_string_compatible<const char*>
	:std::true_type
{};

#endif
template <typename T>
constexpr bool is_string_compatible_v=is_string_compatible<T>::value;

/*
 *	is_boolean_compatible: if the T is boolean compatible
*/
template <typename T>

struct is_boolean_compatible
	:__is_one_of<T,__JSON_PARSER_BOOLEAN_COMPATIBLE>
{};

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
*	UTF8 String and Convertion
*/
class default_utf8string {
	friend std::ostream& operator<<(std::ostream&,const default_utf8string&);
public:
	default_utf8string() :__string() {}
	default_utf8string(const char* str) :__string(str) {}
	default_utf8string(const std::string& str) :__string(str) {}
	default_utf8string(const default_utf8string& r) :__string(r.__string) {}
	default_utf8string(default_utf8string&& r) :__string(std::move(r.__string)) {}
	~default_utf8string(){}

	size_t size() const {
		size_t res=0;
		for (size_t i=0;i<__string.size();){
			size_t byte=__get_byte(static_cast<uint8_t>(__string[i]));
			i+=byte;
			++res;
		}
		return res;
	}
	const char* c_str() const { return __string.c_str(); }
	void clear() { __string.clear(); }
	bool empty() const { return __string.empty(); }
	void pop_back() {
		size_t src_idx=__get_src_idx(__get_src_idx(size()-1));
		size_t byte=__get_byte(static_cast<uint8_t>(__string[src_idx]));
		do {
			__string.pop_back();
			--byte;
		} while (byte>0);
	}

public:
	default_utf8string& operator=(const default_utf8string& r){
		this->__string=r.__string;
		return *this;
	}
	default_utf8string& operator=(default_utf8string&& r){
		this->__string=std::move(r.__string);
		return *this;
	}
	default_utf8string& operator+=(uint32_t ch){
		size_t byte=__get_byte(ch);
		do {
			uint8_t idx_byte=__get_ith_byte(ch,byte);
			__string+=idx_byte;
			--byte;
		} while (byte>0);
		return *this;
	}
	default_utf8string& operator+=(const default_utf8string& r){
		__string+=r.__string;
		return *this;
	}
	uint32_t operator[](size_t idx) const {
		size_t src_idx=__get_src_idx(idx);
		size_t byte=__get_byte(static_cast<uint8_t>(__string[src_idx]));
		uint32_t res=0;
		do {
			res<<=8;
			res+=static_cast<uint8_t>(__string[src_idx]);
			++src_idx;
			--byte;
		} while (byte>0);
		return res;
	}
	bool operator<(const default_utf8string& r) const { return __string<r.__string; }
	operator std::string() const { return __string; }
private:
	size_t __get_src_idx(size_t idx) const {
		size_t res=0,n=__string.size();
		for (size_t i=0;res<n&&i<idx;++i){
			uint8_t ch=static_cast<uint8_t>(__string[res]);
			size_t byte=__get_byte(ch);
			res+=byte;
		}
		return res;
	}
	size_t __get_byte(uint8_t ch) const {
		if (ch>static_cast<uint8_t>(0xF0)) return static_cast<size_t>(4);
		if (ch>static_cast<uint8_t>(0xE0)) return static_cast<size_t>(3);
		if (ch>static_cast<uint8_t>(0xC0)) return static_cast<size_t>(2);
		return static_cast<size_t>(1);
	}
	size_t __get_byte(uint32_t ch) const {
		if (ch>=static_cast<uint32_t>(0xF0000000)) return static_cast<size_t>(4);
		if (ch>=static_cast<uint32_t>(0x00E00000)) return static_cast<size_t>(3);
		if (ch>=static_cast<uint32_t>(0x0000C000)) return static_cast<size_t>(2);
		return static_cast<size_t>(1);
	}
	uint8_t __get_ith_byte(uint32_t ch,size_t i) const {
		switch (i){
		case 1:
			return static_cast<uint8_t>(ch&(0xFF));
		case 2:
			return static_cast<uint8_t>((ch&(0xFF00))>>8);
		case 3:
			return static_cast<uint8_t>((ch&(0xFF0000))>>16);
		case 4:
			return static_cast<uint8_t>((ch&(0xFF000000))>>24);
		default: break;
		}
		// Imposible
		return static_cast<uint8_t>(0);
	}
private:
	std::string __string;
};
struct default_utf8convertion {
	default_utf8string operator()(double num){
		return default_utf8string(std::to_string(num));
	}
	double operator()(const default_utf8string& str){
		return std::stod(str.c_str());
	}
};
__JSON_PARSER_FORCE_INLINE
std::ostream& operator<<(std::ostream& os,const default_utf8string& str){
	os<<str.__string;
	return os;
}

/*
 *	GBK String and Convertion
*/
class default_gbkstring {
	friend std::ostream& operator<<(std::ostream&,const default_gbkstring&);
public:
	default_gbkstring() :__string() {}
	default_gbkstring(const char* str) :__string(str) {}
	default_gbkstring(const std::string& str) :__string(str) {}
	default_gbkstring(const default_gbkstring& r) :__string(r.__string) {}
	default_gbkstring(default_gbkstring&& r) :__string(std::move(r.__string)) {}
	~default_gbkstring(){}

	size_t size() const {
		size_t res=0;
		for (size_t i=0;i<__string.size();){
			size_t byte=__get_byte(static_cast<uint8_t>(__string[i]));
			i+=byte;
			++res;
		}
		return res;
	}
	const char* c_str() const { return __string.c_str(); }
	void clear() { __string.clear(); }
	bool empty() const { return __string.empty(); }
	void pop_back() {
		size_t src_idx=__get_src_idx(__get_src_idx(size()-1));
		size_t byte=__get_byte(static_cast<uint8_t>(__string[src_idx]));
		do {
			__string.pop_back();
			--byte;
		} while (byte>0);
	}
public:
	default_gbkstring& operator=(const default_gbkstring& r){
		this->__string=r.__string;
		return *this;
	}
	default_gbkstring& operator=(default_gbkstring&& r){
		this->__string=std::move(r.__string);
		return *this;
	}
	default_gbkstring& operator+=(uint16_t ch){
		size_t byte=__get_byte(ch);
		do {
			uint8_t idx_byte=__get_ith_byte(ch,byte);
			__string+=idx_byte;
			--byte;
		} while (byte>0);
		return *this;
	}
	default_gbkstring& operator+=(const default_gbkstring& r){
		__string+=r.__string;
		return *this;
	}
	uint16_t operator[](size_t idx) const {
		size_t src_idx=__get_src_idx(idx);
		size_t byte=__get_byte(static_cast<uint8_t>(__string[src_idx]));
		uint16_t res=0;
		do {
			res<<=8;
			res+=static_cast<uint8_t>(__string[src_idx]);
			++src_idx;
			--byte;
		} while (byte>0);
		return res;
	}
	bool operator<(const default_gbkstring& r) const { return __string<r.__string; }
	operator std::string() const { return __string; }
private:
	size_t __get_src_idx(size_t idx) const {
		size_t res=0,n=__string.size();
		for (size_t i=0;res<n&&i<idx;++i){
			uint8_t ch=static_cast<uint8_t>(__string[res]);
			size_t byte=__get_byte(ch);
			res+=byte;
		}
		return res;
	}
	size_t __get_byte(uint8_t ch) const {
		if ((ch&static_cast<uint8_t>(0x80))) return static_cast<size_t>(2);
		return static_cast<size_t>(1);
	}
	size_t __get_byte(uint16_t ch) const {
		if ((ch&static_cast<uint8_t>(0x80))) return static_cast<size_t>(2);
		return static_cast<size_t>(1);
	}
	uint8_t __get_ith_byte(uint16_t ch,size_t i) const {
		switch (i){
		case 1:
			return static_cast<uint8_t>(ch&(0xFF));
		case 2:
			return static_cast<uint8_t>((ch&(0xFF00))>>8);
		default: break;
		}
		// Imposible
		return static_cast<uint8_t>(0);
	}
private:
	std::string __string;
};
struct default_gbkconvertion {
	default_gbkstring operator()(double num){
		return default_gbkstring(std::to_string(num));
	}
	double operator()(const default_gbkstring& str){
		return std::stod(str.c_str());
	}
};
__JSON_PARSER_FORCE_INLINE
std::ostream& operator<<(std::ostream& os,const default_gbkstring& str){
	os<<str.__string;
	return os;
}


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
 *	typedef Json
*/
using DefaultJson=Json<default_objtype,default_arrtype,default_strtype,default_numtype,default_booleantype,default_convertion>;
using DefaultJsonParser=JsonParser<default_objtype,default_arrtype,default_strtype,default_numtype,default_booleantype,default_convertion>;
using UTF8Json=Json<default_objtype,default_arrtype,default_utf8string,default_numtype,default_booleantype,default_utf8convertion>;
using UTF8JsonParser=JsonParser<default_objtype,default_arrtype,default_utf8string,default_numtype,default_booleantype,default_utf8convertion>;
using GBKJson=Json<default_objtype,default_arrtype,default_gbkstring,default_numtype,default_booleantype,default_gbkconvertion>;
using GBKJsonParser=JsonParser<default_objtype,default_arrtype,default_gbkstring,default_numtype,default_booleantype,default_gbkconvertion>;


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
	using Self=Json<ObjType,ArrType,StrType,NumType,BooleanType,convertion>;

	friend class JsonParser<ObjType,ArrType,StrType,NumType,BooleanType,convertion>;

	friend DefaultJson operator""__json(const char*,size_t);
	friend DefaultJson operator""__json(unsigned long long);
	friend DefaultJson operator""__json(long double);

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
	Json() noexcept :__prev(nullptr),__type(Type::NONE) {}
	Json(const Self& r) {
		__construct_from_type(r.__type);
		__assign(&r);
	}
	Json(Self&& r) {
		__construct_from_type(r.__type);
		__move(&r);
	}
	~Json(){
		__release();
	}

	/*
	 *	Other Construct
	*/
	Json(const Type& type) :__prev(nullptr) {
		__construct_from_type(type);
	}
	// TODO: Other Construct

	Type get_type() const noexcept { return __type; }

	Self* clone() const {
		Self* res=new Self(this->__type);
		res->__assign(this);
		return res;
	}
	Self* move_to_heap() {
		Self* res=new Self(this->__type);
		res->__move(this);
		return res;
	}
	Self& move_out()       noexcept { return *__prev; }
	Self& move_out() const noexcept { return *__prev; }

	bool is_object () const noexcept { return __type==Type::OBJECT ; }
	bool is_array  () const noexcept { return __type==Type::ARRAY  ; }
	bool is_string () const noexcept { return __type==Type::STRING ; }
	bool is_number () const noexcept { return __type==Type::NUMBER ; }
	bool is_boolean() const noexcept { return __type==Type::BOOLEAN; }

	const ObjType<StrType,Self*>& raw_object () const noexcept { return __object ; }
	const ArrType<Self*>&         raw_array  () const noexcept { return __array  ; }
	const StrType&                raw_string () const noexcept { return __string ; }
	const NumType&                raw_number () const noexcept { return __number ; }
	const BooleanType&            raw_boolean() const noexcept { return __boolean; }

	bool contain(const StrType& key) const {
		__check_type(Type::OBJECT);
		return __object.count(key)>0;
	}

	static inline Self object() {
		return Self(Type::OBJECT);
	}
	static inline Self array() {
		return Self(Type::ARRAY);
	}
	static inline Self string() {
		return Self(Type::STRING);
	}
	static inline Self number() {
		return Self(Type::NUMBER);
	}
	static inline Self boolean() {
		return Self(Type::BOOLEAN);
	}
	static inline Self null() {
		return Self(Type::NONE);
	}

	template <typename T>
	static inline
	__JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_U(T,Self)
		string(const T& str)
	{
		Self res(Type::STRING);
		res.__string=StrType(str);
		return res;
	}
	template <typename T>
	static inline
	__JSON_PARSER_ENABLE_IF_STRING_COMPATIBLE_U(T,Self)
		string(T&& str)
	{
		Self res(Type::STRING);
		res.__string=StrType(std::move(str));
		return res;
	}
	template <typename T>
	static inline
	__JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE_U(T,Self)
		number(const T& num)
	{
		Self res(Type::NUMBER);
		res.__number=NumType(num);
		return res;
	}
	template <typename T>
	static inline
		__JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE_U(T,Self)
		number(T&& num)
	{
		Self res(Type::NUMBER);
		res.__number=NumType(std::move(num));
		return res;
	}
	template <typename T>
	static inline
	__JSON_PARSER_ENABLE_IF_BOOLEAN_COMPATIBLE_U(T,Self)
		boolean(const T& boo)
	{
		Self res(Type::BOOLEAN);
		res.__boolean=boo;
		return res;
	}
	template <typename T>
	static inline
		__JSON_PARSER_ENABLE_IF_BOOLEAN_COMPATIBLE_U(T,Self)
		boolean(T&& boo)
	{
		Self res(Type::BOOLEAN);
		res.__boolean=BooleanType(std::move(boo));
		return res;
	}

	StrType dump(size_t indent=0) const {
		return __dump(indent,1);
	}
public:
	/*
	 *	as
	*/
	template <typename T>
	__JSON_PARSER_ENABLE_IF_NUMBER_COMPATIBLE(T)
		as() const
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
		return *(__object.at(key));
	}
	const Self& at(const StrType& key) const {
		__check_type(Type::OBJECT);
		return *(__object.at(key));
	}
	Self& at(size_t idx){
		__check_type(Type::ARRAY);
		return *(__array.at(idx));
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
		return *(__object.at(key));
	}
	const Self& operator[](const StrType& key) const {
		__check_type(Type::OBJECT);
		return *(__object.at(key));
	}

	Self& operator[](size_t idx){
		__construct_type_if_null(Type::ARRAY);
		__check_type(Type::ARRAY);
		return *(__array.at(idx));
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
		case Type::NUMBER:
		case Type::BOOLEAN:
		case Type::NONE:
			break;
		default: break;
		}
		__destroy();
	}
	void __release_object(){
		for (auto& p:__object){
			delete p.second;
		}
	}
	void __release_array(){
		for (auto& p:__array){
			delete p;
		}
	}

	void __destroy(){
		switch (__type){
		case Type::OBJECT:
			__destroy_object();  break;
		case Type::ARRAY:
			__destroy_array();   break;
		case Type::STRING:
			__destroy_string();  break;
		case Type::NUMBER:
			__destroy_number();  break;
		case Type::BOOLEAN:
			__destroy_boolean(); break;
		case Type::NONE:
			break;
		default: break;
		}
	}
	void __destroy_object(){
		__destroy_at(&(__object));
	}
	void __destroy_array(){
		__destroy_at(&(__array));
	}
	void __destroy_string(){
		__destroy_at(&(__string));
	}
	void __destroy_number(){
		__destroy_at(&(__number));
	}
	void __destroy_boolean(){
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
		case Type::NONE:
			break;
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
		case Type::NONE:
			break;
		default: break;
		}
		r->__destroy();
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

	StrType __dump(size_t indent,size_t wrap_count) const {
		switch (this->__type){
		case Type::OBJECT:
			return __dump_object(indent,wrap_count);
		case Type::ARRAY:
			return __dump_array(indent,wrap_count);
		case Type::STRING:
			return __dump_string(indent,wrap_count);
		case Type::NUMBER:
			return __dump_number(indent,wrap_count);
		case Type::BOOLEAN:
			return __dump_boolean(indent,wrap_count);
		case Type::NONE:
			return __dump_null(indent,wrap_count);
		default: break;
		}
		// Imposible
		return StrType();
	}
	void __indent_wrap(StrType& str,size_t indent,size_t wrap_count) const {
		if (indent==0) return;
		str+='\n';
		for (size_t i=0;i<wrap_count*indent;++i) str+=" ";
	}
	StrType __dump_object(size_t indent,size_t wrap_count) const {
		StrType res;
		res+='{';
		for (auto p=__object.begin();p!=__object.end();++p){
			if (p!=__object.begin()) res+=',';
			__indent_wrap(res,indent,wrap_count);
			res+='"'; res+=p->first; res+='"';
			res+=": ";
			res+=(p->second)->__dump(indent,wrap_count+1);
		}
		__indent_wrap(res,indent,wrap_count-1);
		res+='}';
		return res;
	}
	StrType __dump_array(size_t indent,size_t wrap_count) const {
		StrType res;
		res+='[';
		for (auto p=__array.begin();p!=__array.end();++p){
			if (p!=__array.begin()) res+=',';
			__indent_wrap(res,indent,wrap_count);
			res+=((*p)->__dump(indent,wrap_count+1));
		}
		__indent_wrap(res,indent,wrap_count-1);
		res+=']';
		return res;
	}
	StrType __dump_string(size_t,size_t) const {
		StrType res;
		res+='"'; res+=__string; res+='"';
		return res;
	}
	StrType __dump_number(size_t,size_t) const {
		return convertion()(__number);
	}
	StrType __dump_boolean(size_t,size_t) const {
		if (__boolean)
			return StrType("true");
		else
			return StrType("false");
	}
	StrType __dump_null(size_t,size_t) const {
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

	std::string __type_to_string(const Type& type) const {
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
	static JsonClass parse(const StrType& json){
		size_t tmp=0,n=json.size();
		JsonClass res=__parse(json,0,tmp,n);
		while ((++tmp)<n){
			if (!__is_blank(json,tmp,n))
				throw ParserException("Json String Too Long");
		}
		return res;
	}
private:
	static JsonClass __parse(const StrType& json,size_t l,size_t& new_idx,size_t n){
		__skip_blank(json,l,n);
		if (json[l]=='{')
			return __parse_object(json,l,new_idx,n);
		if (json[l]=='[')
			return __parse_array(json,l,new_idx,n);
		if (json[l]=='"')
			return __parse_string(json,l,new_idx,n);
		if (__is_digit(json,l,n)||json[l]=='-')
			return __parse_number(json,l,new_idx,n);
		if (json[l]=='t'||json[l]=='f')
			return __parse_boolean(json,l,new_idx,n);
		if (json[l]=='n')
			return __parse_null(json,l,new_idx,n);
		std::string err_msg=std::string("Parser Unknown Character");
		throw ParserException(err_msg);
	}
	static JsonClass __parse_object(const StrType& json,size_t l,size_t& new_idx,size_t n){
		JsonClass res=JsonClass::object();
		new_idx=l;
		__peek(json,new_idx,n,'{');
		__skip_blank(json,new_idx,n);
		__check_overstep_boundary(json,new_idx,n);
		if (json[new_idx]=='}'){
			__peek(json,new_idx,n);
		}
		else {
			bool quit=false;
			while (!quit){
				__skip_blank(json,new_idx,n);
				__check_overstep_boundary(json,new_idx,n);
				size_t tmp_idx=0;
				StrType key=__parse_string(json,new_idx,tmp_idx,n).__string;
				new_idx=tmp_idx+1;
				__skip_blank(json,new_idx,n);
				__check_overstep_boundary(json,new_idx,n);
				__peek(json,new_idx,n,':');
				__skip_blank(json,new_idx,n);
				__check_overstep_boundary(json,new_idx,n);
				JsonClass* value=__parse(json,new_idx,tmp_idx,n).move_to_heap();
				value->__prev=&res;
				res.__object.insert(std::make_pair(key,value));
				new_idx=tmp_idx+1;
				__skip_blank(json,new_idx,n);
				__check_overstep_boundary(json,new_idx,n);
				if (json[new_idx]==','){
					__peek(json,new_idx,n);
				}
				else {
					__peek(json,new_idx,n,'}');
					quit=true;
				}
			}
		}
		--new_idx;
		return res;
	}
	static JsonClass __parse_array(const StrType& json,size_t l,size_t& new_idx,size_t n){
		JsonClass res=JsonClass::array();
		new_idx=l;
		__peek(json,new_idx,n,'[');
		__skip_blank(json,new_idx,n);
		__check_overstep_boundary(json,new_idx,n);
		if (json[new_idx]==']'){
			__peek(json,new_idx,n);
		}
		else {
			bool quit=false;
			while (!quit){
				__skip_blank(json,new_idx,n);
				__check_overstep_boundary(json,new_idx,n);
				size_t tmp_idx=0;
				JsonClass* value=__parse(json,new_idx,tmp_idx,n).move_to_heap();
				value->__prev=&res;
				res.__array.push_back(value);
				new_idx=tmp_idx+1;
				__skip_blank(json,new_idx,n);
				__check_overstep_boundary(json,new_idx,n);
				if (json[new_idx]==','){
					__peek(json,new_idx,n);
				}
				else {
					__peek(json,new_idx,n,']');
					quit=true;
				}
			}
		}
		--new_idx;
		return res;
	}
	static JsonClass __parse_string(const StrType& json,size_t l,size_t& new_idx,size_t n){
		JsonClass res=JsonClass::string();
		new_idx=l;
		__peek(json,new_idx,n,'"');
		bool quit=false;
		while (!quit){
			__check_overstep_boundary(json,new_idx,n);
			switch (json[new_idx]){
			case '"': {
				__peek(json,new_idx,n); quit=true; break;
			}
			case '\\': {
				res.__string+=json[new_idx];
				__peek(json,new_idx,n);
				__check_control_character(json,new_idx,n);
				res.__string+=json[new_idx];
				__peek(json,new_idx,n);
				break;
			}
			default: {
				if (__is_blank(json,new_idx,n)&&json[new_idx]!=' '){
					throw ParserException("String Contain Control Character");
				}
				res.__string+=json[new_idx];
				__peek(json,new_idx,n);
				break;
			}
			}
		}
		--new_idx;
		return res;
	}
	static JsonClass __parse_number(const StrType& json,size_t l,size_t& new_idx,size_t n){
		JsonClass res=JsonClass::number();
		StrType tmp; tmp.clear();
		new_idx=l;
		if (json[new_idx]=='-'){
			tmp+='-'; __peek(json,new_idx,n);
		}
		if (__is_digit(json,new_idx,n)&&json[new_idx]!='0'){
			tmp+=json[new_idx]; __peek(json,new_idx,n);
			while (new_idx<n&&__is_digit(json,new_idx,n)){
				tmp+=json[new_idx];
				__peek(json,new_idx,n);
			}
		}
		else {
			__peek(json,new_idx,n,'0'); tmp+='0';
		}
		if (new_idx<n&&json[new_idx]=='.'){
			tmp+='.'; __peek(json,new_idx,n);
			do {
				if (__is_digit(json,new_idx,n)){
					tmp+=json[new_idx];
					__peek(json,new_idx,n);
				}
			} while (new_idx<n&&__is_digit(json,new_idx,n));
		}
		if (new_idx<n&&(json[new_idx]=='e'||json[new_idx]=='E')){
			tmp+=json[new_idx]; __peek(json,new_idx,n);
			if (new_idx<n&&(json[new_idx]=='+'||json[new_idx]=='-')){
				tmp+=json[new_idx]; __peek(json,new_idx,n);
			}
			do {
				__check_digit(json,new_idx,n);
				tmp+=json[new_idx]; __peek(json,new_idx,n);
			} while (new_idx<n&&__is_digit(json,new_idx,n));
		}
		res.__number=convertion()(tmp);
		--new_idx;
		return res;
	}
	static JsonClass __parse_boolean(const StrType& json,size_t l,size_t& new_idx,size_t n){
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
	static JsonClass __parse_null(const StrType& json,size_t l,size_t& new_idx,size_t n){
		new_idx=l;
		__peek(json,new_idx,n,'n');
		__peek(json,new_idx,n,'u');
		__peek(json,new_idx,n,'l');
		__peek(json,new_idx,n,'l');
		--new_idx;
		return JsonClass::null();
	}

	static inline bool __is_digit(const StrType& json,size_t idx,size_t n){
		__check_overstep_boundary(json,idx,n);
		auto ch=json[idx];
		return ch>='0'&&ch<='9';
	}
	static inline bool __is_blank(const StrType& json,size_t idx,size_t n){
		__check_overstep_boundary(json,idx,n);
		auto ch=json[idx];
		return ch==' '||ch=='\n'||ch=='\t'||ch=='\r\n';
	}
	static void __skip_blank(const StrType& json,size_t& idx,size_t n){
		while (idx<n&&__is_blank(json,idx,n)){
			__peek(json,idx,n);
		}
	}
	static void __peek(const StrType& json,size_t& idx,size_t n,uint32_t ch){
		__check_overstep_boundary(json,idx,n);
		if (json[idx]!=ch){
			std::string err_msg=std::string("Parser Unknown Character")+char(ch);
			throw ParserException(err_msg);
		}
		++idx;
	}
	static void __peek(const StrType& json,size_t& idx,size_t n){
		__check_overstep_boundary(json,idx,n);
		++idx;
	}
	static void __check_overstep_boundary(const StrType& json,size_t idx,size_t n){
		if (idx>=n){
			throw ParserException("Index Overstep the Boundary");
		}
	}

	static void __check_control_character(const StrType& json,size_t idx,size_t n){
		__check_overstep_boundary(json,idx,n);
		auto ch=json[idx];
		if (ch=='"')  return;
		if (ch=='\\') return;
		if (ch=='/')  return;
		if (ch=='b')  return;
		if (ch=='f')  return;
		if (ch=='n')  return;
		if (ch=='r')  return;
		if (ch=='t')  return;
		if (ch=='u')  return;
		throw ParserException(std::string("Parser Unknown Control Character"));
	}

	static void __check_digit(const StrType& json,size_t idx,size_t n){
		if (!__is_digit(json,idx,n)){
			throw ParserException("Want Digit");
		}
	}
};

/*
 *	literal
*/
__JSON_PARSER_FORCE_INLINE
DefaultJson operator""__json(const char* json,size_t){
	return DefaultJsonParser::parse(json);
}
__JSON_PARSER_FORCE_INLINE
DefaultJson operator""__json(unsigned long long num){
	DefaultJson res=DefaultJson::number();
	res.__number=default_numtype(num);
	return res;
}
__JSON_PARSER_FORCE_INLINE
DefaultJson operator""__json(long double num){
	DefaultJson res=DefaultJson::number();
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
