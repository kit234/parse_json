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

	// ʹ����������Json�ַ���ת��Json����
	JsonObject* person=R"({"name":"kit","age":18})"_json_object;

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
	
	JsonObject* school=new JsonObject; // �½�һ��JsonObject����
	JsonArray* students=nullptr;
	school->create_array("students")->out_array("students",&students)->get_array("students") // �½�students�ֶ����鲢��ֵ������,Ȼ������ֶ�
			->create_back_object()->get_back_object()  // ��students����ӵ�һ��ѧ����Ϣ,�������ֶ�
				->set("name",R"("kit")"_json_string)
				->set("age",18_json_number)
				->set("sex",R"("m")"_json_string)
			->move_out_array()  // ����students�ֶ�
			->create_back_object()->get_back_object()  // ��students�ֶ�����ӵڶ���ѧ��,�������ֶ�
				->set("name",R"("kitty")"_json_string)
				->set("age",20_json_number)
				->set("sex",R"("m")"_json_string)
			->move_out_array()  // ����students�ֶ�
		->move_out_object() // ����������JsonObject����
		->set("year",R"("2019-9-16")"_json_string);  // �½�year�ֶ�

	std::cout<<"Student Total: "<<students->size()<<std::endl;
	std::cout<<school->to_json_string()<<std::endl; // Json����תJson�ַ���
	delete school;
## 2.4 ע������
����JsonParser���������Json������Ҫ��delete����,���˴���Json���󷽷���Json����ָ��,�������������Json����ָ�붼��Ҫ��������.
# 3. Demo
����srcs�µ�main.cpp