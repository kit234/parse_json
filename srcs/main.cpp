#include <iostream>
#include <vector>

template <typename T>
class MyUnique {
public:
	MyUnique(T* ptr) :__ptr(ptr){}
	MyUnique(MyUnique&& r){
		this->__ptr=r.__ptr;
		r.__ptr=nullptr;
	}
	~MyUnique() { __release(); }
	template <typename U>
	decltype(auto) operator[](const U& idx) {
		return (*__ptr).operator[](idx);
	}
public:
	void operator=(MyUnique&& r){
		__release();
		this->__ptr=r.__ptr;
		r.__ptr=nullptr;
	}
private:
	MyUnique()=delete;
	MyUnique(const MyUnique&)=delete;
	void __release(){
		if (__ptr) delete __ptr;
		__ptr=nullptr;
	}
private:
	T* __ptr;
};

using namespace std;

int main(){
	MyUnique<vector<int>> v(new vector<int>({1,2,3}));
	MyUnique<vector<int>> v2(new vector<int>());
	v2=std::move(v);
	return 0;
}
