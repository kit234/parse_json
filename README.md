# 1. ���
ʹ���ִ�C\++��д��JsonParser�⣬���л����Ľ����ַ���������ͬʱ֧���Զ���Json�����Լ�Json���ͺ�Json�ַ������໥ת����
# 2. ���ʹ��
ֻҪ��json_parser.hpp�����Լ�����Ŀ�оͺ��ˡ�
## 2.1 ������Json�ַ�������
ʹ��JsonParser��from_string����������Json�ַ���,����:

    JsonParser parser=JsonParser::from_string("{\"name\":\"kit\",\"age\":18,\"scores\":{60,70,80}}");
����ʹ��[]��������������е�Ԫ�أ�����:

    const JsonParser::Value& value=parser.get_root();
	string name=value["name"].as<string>();
	int age=value["age"].as<int>();
	int score=value["scores"][0].as<int>();
�����ʹ����ͨ��forѭ�����������飬����:

	for (int i=0;i<value["scores"].size();++i){
		int s=value["scores"][i].as<int>();
	}
���������Ҫʹ����ǿforѭ��������������е��鷳�ˣ�����:

	/*
	* �㲻��������
	* for (int score:value["scores"].as<vector<int>>()){
	* 	int s=score;
	* }
	*/
	// �����Ҫʹ����ǿforѭ��,�������ת��ΪJsonArray��ת��Ϊvector
	for (JsonNumber* score:value["scores"].as<JsonArray<JsonNumber>>().as<vector<JsonNumber*>>()){
		int s=score->as<int>();
	}
## 2.2 �Զ���Json����
�����ṩ��5�л�����Json���ͣ�JsonString��JsonNumber��JsonBoolean��JsonArray��JsonObject�������ʹ����5�л�����Json��������Լ���Json���ͣ�  
�����������:

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
ʹ��START_DEFINE_FIELD��DEFINE_FIELD��END_DEFINE_FIELD������Json�����а�����Ԫ�أ�ʹ��SET_FIELD����Json�����е�Ԫ�ظ�ֵ

	Student* student=new Student("kit",18,true,{60,70,80});
	// ����2�ֻ�ȡ�����ǵȼ۵�
	JsonString* name1=GET_FIELD(student,JsonString,"name");
	JsonString* name2=student->get_field<JsonString>("name");
	// ����2�ָ�ֵ�����ǵȼ۵�
	SET_FIELD(student,JsonString,"name","kit");
	student->get_field<JsonString>("name")->set("kit");
	// ע��: GET_FIELD��SET_FIELDֻ�е�student��ָ���ǲſ���ʹ��
	delete student;
## 2.3 Json������Json�ַ������໥ת��
������������:

	JsonParser parser=JsonParser::from_string("{\"name\":\"kit\",\"age\":18,\"scores\":{60,70,80}}");
	// Json�ַ���תJson����
	Student* student=new Student; student->from_json_value(parser.get_root());
	// Json����תJson�ַ���
	JsonParser temp=student->to_json_parser();

	cout<<temp.to_json_string()<<endl;
	delete student;
## 2.4 ������޸�Json�ַ���
����Դ�ͷ��ʼ����Json�ַ���������:

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
create_object��create_array����ͨ�������Key������յ�Json���ͺͿյ�Json����,  
create_and_into_last_elem���������������й����һ��Ԫ�ز����ƶ����Ǹ�Ԫ����ȥ��  
move_out���������ƶ�����һ������  
����Ĵ��봴����Json�ַ���Ϊ:

	{
	"route":{
		"paths":[{"steps":["123,321","456,654"]}],
		"wuhu":"ye"
		}
	}
�������������ʾ�ķ���������add������set���������磺

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
������Ҫע����ǣ���Щ������ģ�����ֻ����Json���͡�
# 3. Demo
��Ŀ�е�main.cpp����Demo