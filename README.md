# 1. Json Parser
使用现代C++编写的Json解析器，可以将Json字符串转化为Json对象，或者将Json对象转化为Json字符串。
# 2. How To Use
只需要把srcs下的json_parser.hpp导入到自己的项目中即可,包含jsonparser名称空间.
## 2.1 Json对象和Json字符串的相互转化
可以使用JsonParser\::parse_to_object可以将Json对象转化为Json字符串,也可以使用Json对象的to_json_string将Json对象转化为Json字符串,例如:

	using jsonparser;

	std::string json="{\"name\":\"kit\",\"age\":18,\"scores\":[60,70,80]}";

	// Json字符串转Json对象
	JsonObject* student=JsonParser::parse_for_object(json.c_str());

	// Json对象转Json字符串
	std::string json_string=student->to_json_string();

	// 使用字面量将Json字符串转成Json对象
	JsonObject* person=R"({"name":"kit","age":18})"_json_object;

	delete student;

## 2.2 访问Json对象
一共将Json对象分为了五个类型,分别是:JsonObject,JsonArray,JsonString,JsonNumber和JsonBoolean,这5个都实现了JsonBase接口.可以调用JsonObject和JsonArray的get_XXX方法来获得Json对象的字段,例如:

	using jsonparser;

	JsonString* name=student->get_string("name");   // 获取Json对象中的name字段
	std::cout<<name->as_string()<<std::endl;        // 将name字段转化为std::string并输出
	JsonArray* scores=student->get_array("scores"); // 获取Json对象中的scores字段
	for (size_t i=0;i<scores->size();++i){          // 输出scores字段中的信息
		std::cout<<scores->get_number(i)->as_int()<<std::endl;
	}
对于JsonArray,你也可以使用增强for循环来遍历元素,例如:

	using jsonparser;

	std::string json="{\"name\":\"kit\",\"age\":18,\"scores\":[60,70,80]}";

	// Json字符串转Json对象
	JsonObject* student=JsonParser::parse_to_object(json.c_str());

	JsonArray* scores=student->get_array("scores"); // 获取Json对象中的scores字段
	for (JsonBase* score:scores->as_vector()){      // 使用增强for循环遍历元素
		std::cout<<((JsonNumber*)score)->as_int()<<std::endl;
	}

	delete student;
## 2.3 构造或修改Json对象
你可以修改现有的Json对象,例如:

	using jsonparser;

	std::string json="{\"name\":\"kit\",\"age\":18,\"scores\":[60,70,80]}";

	// Json字符串转Json对象
	JsonObject* student=JsonParser::parse_to_object(json.c_str());

	student->set("name",new JsonString("kitty")); // 将name字段修改为"kitty"
	student->set("sex",new JsonString("f"));      // 新增一个sex字段,字段值为"f"
	student->get_array("scores")->push_back(new JsonNumber(90)); // 在scores字段数组中新增一个值

	delete student;
你也可以从头构造出一个Json对象,例如:
	
	JsonObject* school=new JsonObject; // 新建一个JsonObject对象
	JsonArray* students=nullptr;
	school->create_array("students")->out_array("students",&students)->get_array("students") // 新建students字段数组并赋值给变量,然后进入字段
			->create_back_object()->get_back_object()  // 在students中添加第一个学生信息,并进入字段
				->set("name",R"("kit")"_json_string)
				->set("age",18_json_number)
				->set("sex",R"("m")"_json_string)
			->move_out_array()  // 返回students字段
			->create_back_object()->get_back_object()  // 在students字段中添加第二个学生,并进入字段
				->set("name",R"("kitty")"_json_string)
				->set("age",20_json_number)
				->set("sex",R"("m")"_json_string)
			->move_out_array()  // 返回students字段
		->move_out_object() // 返回最外层的JsonObject对象
		->set("year",R"("2019-9-16")"_json_string);  // 新建year字段

	std::cout<<"Student Total: "<<students->size()<<std::endl;
	std::cout<<school->to_json_string()<<std::endl; // Json对象转Json字符串
	delete school;
## 2.4 注意事项
对于JsonParser构造出来的Json对象需要用delete销毁,除了传入Json对象方法的Json对象指针,其他构造出来的Json对象指针都需要进行销毁.
# 3. Demo
就是srcs下的main.cpp