# 1. Json Parser
ʹ���ִ�C++��д��Json�����������Խ�Json�ַ���ת��ΪJson���󣬻��߽�Json����ת��ΪJson�ַ�����
# 2. How To Use
ֻ��Ҫ��srcs�µ�json_parser.hpp���뵽�Լ�����Ŀ�м���,����jsonparser���ƿռ�.
## 2.1 Json�����Json�ַ������໥ת��
����ʹ��JsonParser\::parse_to_object���Խ�Json����ת��ΪJson�ַ���,Ҳ����ʹ��Json�����to_json_string��Json����ת��ΪJson�ַ���,����:

	using jsonparser;

	std::string json="{\"name\":\"kit\",\"age\":18,\"scores\":[60,70,80]}";

	// Json�ַ���תJson����
	JsonObject* student=JsonParser::parse_for_object(json.c_str());

	// Json����תJson�ַ���
	std::string json_string=student->to_json_string();

	delete student;

## 2.2 ����Json����
һ����Json�����Ϊ���������,�ֱ���:JsonObject,JsonArray,JsonString,JsonNumber��JsonBoolean,��5����ʵ����JsonBase�ӿ�.���Ե���JsonObject��JsonArray��get_XXX���������Json������ֶ�,����:

	using jsonparser;

	JsonString* name=student->get_string("name");   // ��ȡJson�����е�name�ֶ�
	std::cout<<name->as_string()<<std::endl;        // ��name�ֶ�ת��Ϊstd::string�����
	JsonArray* scores=student->get_array("scores"); // ��ȡJson�����е�scores�ֶ�
	for (size_t i=0;i<scores->size();++i){          // ���scores�ֶ��е���Ϣ
		std::cout<<scores->get_number(i)->as_int()<<std::endl;
	}
����JsonArray,��Ҳ����ʹ����ǿforѭ��������Ԫ��,����:

	using jsonparser;

	std::string json="{\"name\":\"kit\",\"age\":18,\"scores\":[60,70,80]}";

	// Json�ַ���תJson����
	JsonObject* student=JsonParser::parse_to_object(json.c_str());

	JsonArray* scores=student->get_array("scores"); // ��ȡJson�����е�scores�ֶ�
	for (JsonBase* score:scores->as_vector()){      // ʹ����ǿforѭ������Ԫ��
		std::cout<<((JsonNumber*)score)->as_int()<<std::endl;
	}

	delete student;
## 2.3 ������޸�Json����
������޸����е�Json����,����:

	using jsonparser;

	std::string json="{\"name\":\"kit\",\"age\":18,\"scores\":[60,70,80]}";

	// Json�ַ���תJson����
	JsonObject* student=JsonParser::parse_to_object(json.c_str());

	student->set("name",new JsonString("kitty")); // ��name�ֶ��޸�Ϊ"kitty"
	student->set("sex",new JsonString("f"));      // ����һ��sex�ֶ�,�ֶ�ֵΪ"f"
	student->get_array("scores")->push_back(new JsonNumber(90)); // ��scores�ֶ�����������һ��ֵ

	delete student;
��Ҳ���Դ�ͷ�����һ��Json����,����:
	
	using jsonparser;

	class Student :public JsonObject {
	public:
		Student(const std::string& name,int age){
			this->set("name",new JsonString(name));
			this->set("age",new JsonNumber(age));
		}
	};

	int main(){
		JsonObject* school=new JsonObject; // �½�һ��JsonObject����
		JsonArray* students=nullptr;
		school->create_array("students")->to_array("students",&students) // �½�"students"�������ֶ�,����ֵ��students����
			->set("name",new JsonString("University")); // �½�һ��name�ֶ�
		students->push_back(new Student("kit",18))      // Ϊstudents�ֶ����ѧ��
			->push_back(new Student("kitty",20))
			->push_back(new Student("world",16));
		std::cout<<school->to_json_string()<<std::endl;
		delete school;
		return 0;
	}
## 2.4 ע������
����JsonParser���������Json������Ҫ��delete����,���˴���Json���󷽷���Json����ָ��,�������������Json����ָ�붼��Ҫ��������.
# 3. Demo
����srcs�µ�main.cpp