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
	
	using jsonparser;

	class Student :public JsonObject {
	public:
		Student(const std::string& name,int age){
			this->set("name",new JsonString(name));
			this->set("age",new JsonNumber(age));
		}
	};

	int main(){
		JsonObject* school=new JsonObject; // 新建一个JsonObject对象
		JsonArray* students=nullptr;
		school->create_array("students")->to_array("students",&students) // 新建"students"的数组字段,并赋值给students变量
			->set("name",new JsonString("University")); // 新建一个name字段
		students->push_back(new Student("kit",18))      // 为students字段添加学生
			->push_back(new Student("kitty",20))
			->push_back(new Student("world",16));
		std::cout<<school->to_json_string()<<std::endl;
		delete school;
		return 0;
	}
## 2.4 注意事项
对于JsonParser构造出来的Json对象需要用delete销毁,除了传入Json对象方法的Json对象指针,其他构造出来的Json对象指针都需要进行销毁.
# 3. Demo
就是srcs下的main.cpp