#include<iostream>
#include<vector>

using namespace std;

int main()
{
	vector<int> vec;
	vec.assign(4, -977);
	for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		cout<< "value : " << *it;
	}
	getchar();
}