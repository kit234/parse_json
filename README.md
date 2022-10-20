- [Introduce](#introduce)
- [How to Use](#how-to-use)
- [Examples](#examples)
	- [JSON字符串与JSON对象的相互转化](#json字符串与json对象的相互转化)
	- [访问JSON对象](#访问json对象)
	- [构造JSON对象](#构造json对象)
	- [自定义JSON对象中存储的类型](#自定义json对象中存储的类型)
	- [自定义编码格式的JSON解析器](#自定义编码格式的json解析器)
	- [一些杂七杂八的小玩意](#一些杂七杂八的小玩意)
	- [自定义类型需要实现的基本函数](#自定义类型需要实现的基本函数)
		- [共有部分](#共有部分)
		- [Object\<T,U\>独有](#objecttu独有)
		- [Array\<T\>独有](#arrayt独有)
		- [String独有](#string独有)
		- [Number独有](#number独有)
		- [Boolean独有](#boolean独有)
# Introduce
使用现在C++编写的JSON库,旨在提供一个轻便且功能健全的JSON解析器。除了基本的解析功能,这个库还可以对GBK和UTF8等不同编码格式的JSON字符串进行解析,同时通过自定义类型,你还可以创建属于自定义编码格式的JSON字符串解析器。
# How to Use
只需要将srcs下的json_parser.hpp文件包含到自己项目即可。这个库要求至少要C++11。
# Examples
## JSON字符串与JSON对象的相互转化
你可以使用JsonParser的parse方法或者__json后缀来将JSON字符串解析成JSON对象,例如

    Json<> obj=JsonParser<>::parse(R"({"name":"kit","age":18})");
	Json<> stu=R"({"name":"kit","age":18})"__json;
你可以使用JSON对象的dump方法将JSON对象转化成JSON字符串,例如

    Json<> obj=JsonParser<>::parse(R"({"name":"kit","age":18})");
    std::string str=obj.dump();
## 访问JSON对象
这个JSON库提供了简单易懂的访问JSON对象的方法,例如

    Json<> obj=R"({"name":"kit","age":18})"__json;
	Json<> arr=R"([18,69,80])"__json;

	std::string name=obj["name"].as<std::string>();
	int age=obj.at("age").as<int>();

	for (size_t i=0;i<arr.size();++i){
		int num1=arr[i].as<int>();
		int num2=arr.at(i).as<int>();
	}
只有Object和Array的JSON对象才可以通过[]操作符或者at函数访问内容,[]操作符和at函数唯一的区别就是：当JSON对象是Object时,[]操作符会在没有内容的时候新建内容,而at函数在没有内容的时候汇报错,例如

	Json<> obj=R"({"name":"kit","age":18})"__json;

	obj["name1"]="kitty1";
	//obj.at("name2")="kitty2";  ERROR
## 构造JSON对象
除了解析JSON字符串构造JSON对象外,还可以从头开始构造JSON对象。  
这个库将JSON对象分为Object、Array、String、Number、Boolean和Null。  
你可以调用相对应的函数来生成相对应的空对象,例如

	Json<> obj=Json<>::object();  // empty object
	Json<> arr=Json<>::array() ;  // empty array
	Json<> str=Json<>::string();  // empty string
	Json<> num=Json<>::number();  // random number
	Json<> boo=Json<>::boolean(); // random boolean
	Json<> nul=Json<>::null();    // null
或者你可以在上面函数中加入参数来构造带初始值的JSON对象,例如

	Json<> str=Json<>::string("hello");
	Json<> num=Json<>::number(1e2);
	Json<> boo=Json<>::boolean(true);
你也可以利用[]操作符来构造或修改Object类型的JSON对象:

	Json<> obj;
	obj["name"]="kit";
	obj["age"]=18;
	obj["book"]["name"]="english";
你也可以使用类似STL的函数来构造或修改JSON对象:

	Json<> obj;
	obj.insert("name","kit");
	obj.insert("age",18);
	Json<> arr;
	arr.push_back(18.2);_
	arr.insert(0,"wuhu");
如果你想要构造比较复杂一点的JSON对象:
	
	{
	    "students": [
		    {
			    "name": "kit",
			    "age" : 18
		    },
		    {
			    "name": "kitty",
			    "age" : 16
		    }
	    ],
	    "year": "2017-9-23"
	}
你也可以像下面的例子一样构造JSON对象:
	
	Json<> obj;
    obj.insert("students")
           .push_back()
               .insert("name","kit")
               .insert("age",18)
               .move_out()
           .push_back()
               .insert("name","kitty")
               .insert("age",16)
               .move_out()
           .move_out()
       .insert("year","2017-9-23");
move_out函数可以让你返回"上一级",而缺少参数的insert和push_back函数会创建空对象,运用这些特性就可以像"流水线"一样构造出复杂的JSON对象了。
## 自定义JSON对象中存储的类型
这个库将JSON对象分为了Object、Array、String、Number、Boolean和null六类,其中除了null之外的五类都有实际的存储类型,这个库允许你自定义这5种类型的实际类型,同时提供了这5种类型的默认类型:

	using default_objtype     = std::map
	using default_arrtype     = std::vector
	using default_strtype     = std::string
	using default_numtype     = double
	using default_booleantype = bool
因此当你使用`Json<>`或者`JsonParser<>`的时候就是表示使用上述5种默认类型的JSON对象和Json解析器。  
实际上`Json`类和`JsonParser`类的模板参数一共有6个,前5个就是上述的5个实际类型,而第6个模板参数表示String类型和Number类型的转化:

	struct default_convertion {
		std::string operator()(double num) {
			return std::to_string(num);
		}
		double operator()(const std::string& str) {
			return std::stod(str);
		}
	};
因此如果你想要自定义实际类型的JSON对象,在修改了默认的String和Number类型的情况下,别忘记编写Convertion。
## 自定义编码格式的JSON解析器
在`Json`类和`JsonParser`类的5种实际存储类型中,String类型最为特殊,因为它可以影响JSON解析器可以解析的编码格式。如果你使用可以表示UTF8的字符串类型,那JSON对象和JSON解析器就可以支持UTF8编码格式。  
库中实现了比较基础的可以表示UTF8格式和GBK格式的字符串,分别为`default_utf8string`和`default_gbkstring`，同时也实现了相应的转化:`default_utf8convertion`和`default_gbkconvertion`。  
因此这个库实现可以解析UTF8格式和GBK格式的JSON解析器以及可以表示UTF8格式和GBK格式的JSON对象:

	const char* utf8_str=u8"{\"name\":\"hello\"}";
	const char* gbk_str ="{\"name\":\"你好\"}";

	UTF8Json utf8_json=UTF8JsonParser::parse(utf8_str);
	GBKJson  gbk_json =GBKJsonParser::parse(gbk_str);
如果你实现了可以表示自定义编码格式的字符串类型及其和数字类型的转化,那么也就表示你实现了可以解析自定义编码格式的JSON解析器以及可以表示自定义编码格式的JSON对象。
## 一些杂七杂八的小玩意
你可以使用`is_string_compatible`以及`is_string_compatible_v`来判断类型能否转化为JSON对象中的String类型。

	is_string_compatible_v<const char*>; // true
	is_string_cimpatible<int>::value; // false
你可以使用`is_number_compatible`以及`is_number_compatible_v`来判断类型能否转化为JSON对象中的Number类型。

	is_number_compatible_v<int>; // true
	is_number_compatible<char>::value; // false
你可以使用`is_boolean_compatible`以及`is_boolean_compatible_v`来判断类型能否转化为JSON对象中的Boolean类型。

	is_boolean_compatible_v<bool>; // true
	is_boolean_compatible<int>::value; // false
## 自定义类型需要实现的基本函数
### 共有部分
	Self(); // default constructor
	Self(const Self&); // assign constructor
	Self(Self&&); // move constructor
	Self& operator=(const Self&);
	Self& operator=(Self&&);
### Object\<T,U\>独有
	size_t size() const;
	void clear();
	bool empty() const;
	void insert(const std::pair<T,U>&);
	Iterator begin();
	Iterator end();
	ConstIterator begin() const;
	ConstIterator end() const;
### Array\<T\>独有
	size_t size() const;
	void clear();
	bool empty() const;
	void insert(size_t,auto&);
	void push_back(auto&);
	Iterator begin();
	Iterator end();
	ConstIterator begin() const;
	ConstIterator end() const;
### String独有
	size_t size() const;
	const char* c_str() const;
	void clear();
	bool empty() const;
	void pop_back();
	auto operator[](size_t) (const?);
	bool operator<(const Self&) const;
	Self& operator+=(auto ch);
	operator std::string() const;
### Number独有
	None
### Boolean独有
	None
