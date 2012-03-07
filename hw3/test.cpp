#include <sstream>
#include <iostream>

using namespace std;
int main(){
	
	stringstream ss("Test 1 2 3 4.5");
	string test;
	ss >> test;
	cout << test<<endl;
	int a;
	ss >> a;
	int b;
	ss >> b;
	cout<< a-b;
}