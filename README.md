# 1. 简介
使用现代C\++编写的JsonParser库，具有基本的解析字符串操作，同时支持自定义Json类型以及Json类型和Json字符串的相互转化。
# 2. 如何使用
只要将json_parser.hpp引入自己的项目中就好了。
## 2.1 基本的Json字符串解析
使用JsonParser的from_string函数来解析Json字符串,例如:

    JsonParser parser=JsonParser::from_string("{\"name\":\"kit\",\"age\":18,\"scores\":{60,70,80}}");
可以使用[]运算符来引用其中的元素，例如:

    const JsonParser::Value& value=parser.get_root();
	string name=value["name"].as<string>();
	int age=value["age"].as<int>();
	int score=value["scores"][0].as<int>();
你可以使用普通的for循环来遍历数组，例如:

	for (int i=0;i<value["scores"].size();++i){
		int s=value["scores"][i].as<int>();
	}
不过如果想要使用增强for循环来遍历数组就有点麻烦了，例如:

	/*
	* 你不能这样做
	* for (int score:value["scores"].as<vector<int>>()){
	* 	int s=score;
	* }
	*/
	// 如果想要使用增强for循环,你必须先转化为JsonArray再转化为vector
	for (JsonNumber* score:value["scores"].as<JsonArray<JsonNumber>>().as<vector<JsonNumber*>>()){
		int s=score->as<int>();
	}
## 2.2 自定义Json类型
库中提供了5中基本的Json类型：JsonString、JsonNumber、JsonBoolean、JsonArray和JsonObject，你可以使用这5中基本的Json类型来搭建自己的Json类型，  
基本框架如下:

	class Student:public JsonObject {
	public:
	JSON_OBJECT(Student)
	Student(const string& name,int age,bool is_man,const std::initializer_list<double>& scores){
		DEFINE_FIELDS;
		SET_FIELD(this,JsonString,"name",name);
		SET_FIELD(this,JsonNumber,"age",age);
		SET_FIELD(this,JsonBoolean,"is_man",is_man);
		SET_FIELD(this,JsonArray<JsonNumber>,"scores",scores);
	}
	private:
	START_DEFINE_FIELD
	DEFINE_FIELD(JsonString,"name");
	DEFINE_FIELD(JsonNumber,"age");
	DEFINE_FIELD(JsonBoolean,"is_man");
	DEFINE_FIELD(JsonArray<JsonNumber>,"scores");
	END_DEFINE_FIELD
	};
使用START_DEFINE_FIELD、DEFINE_FIELD和END_DEFINE_FIELD来定义Json类型中包含的元素，使用SET_FIELD来给Json类型中的元素赋值

	Student* student=new Student("kit",18,true,{60,70,80});
	// 以下2种获取方法是等价的
	JsonString* name1=GET_FIELD(student,JsonString,"name");
	JsonString* name2=student->get_field<JsonString>("name");
	// 以下2种赋值方法是等价的
	SET_FIELD(student,JsonString,"name","kit");
	student->get_field<JsonString>("name")->set("kit");
	// 注意: GET_FIELD和SET_FIELD只有当student是指针是才可以使用
	delete student;
## 2.3 Json类型与Json字符串的相互转化
具体例子如下:

	JsonParser parser=JsonParser::from_string("{\"name\":\"kit\",\"age\":18,\"scores\":{60,70,80}}");
	// Json字符串转Json类型
	Student* student=new Student; student->from_json_value(parser.get_root());
	// Json类型转Json字符串
	JsonParser temp=student->to_json_parser();

	cout<<temp.to_json_string()<<endl;
	delete student;
## 2.4 构造或修改Json字符串
你可以从头开始构造Json字符串，例如:

	JsonParser p=JsonParser::get_empty();
	JsonParser::Value& value=p.get_root();
	value.create_object("route")["route"]
		 .create_array("paths")["paths"].create_and_into_last_elem()
		 .create_array("steps")["steps"]
		 .push_back<JsonString>("123,321")
		 .push_back<JsonString>("456,654")
	     .move_out().move_out().move_out().move_out()
		 .add<JsonString>("wuhu","ye");
	cout<<p.to_json_string()<<endl;
create_object和create_array方法通过传入的Key构造出空的Json类型和空的Json数组,  
create_and_into_last_elem方法可以在数组中构造出一个元素并且移动到那个元素中去。  
move_out方法就是移动到上一层来。  
上面的代码创建的Json字符串为:

	{
	"route":{
		"paths":[{"steps":["123,321","456,654"]}],
		"wuhu":"ye"
		}
	}
除了上面代码演示的方法，还有add方法和set方法，例如：

	Student* student=new Student("kit",18,true,{60,70,80});
	JsonParser p=JsonParser::get_empty();
	JsonParser::Value& value=p.get_root();
	value.add<JsonString>("name","kit")
		 .add<JsonNumber>("age",18)
		 .add<JsonArray<JsonNumber>>("scores",JsonArray<JsonNumber>({60,70,80}))
	     .add<Student>("student",*student);
	value.set<JsonString>("name","kitty");
	value["age"].set<JsonNumber>(20);
	cout<<p.to_json_string()<<endl;
不过需要注意的是，这些方法的模板参数只能是Json类型。
# 3. Demo
项目中的main.cpp就是Demo