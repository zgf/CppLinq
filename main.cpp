#include "linq.h"
#include <vector>
#include <iostream>
#include <assert.h>
#include <string>
using std::vector;
using std::cout;
using std::endl;
using std::string;
class PetOwner
{
public:
	string Name;
	vector<string> Pets;
public:
	PetOwner(string _Name, vector<string> _Pets) :Name(_Name), Pets(_Pets)
	{

	}
};

class Person
{
public:
	string Name;
};

class Pet
{
public:
	string Name;
	Person Owner;
};

void test_select()
{
	vector<PetOwner> petOwners =
	{PetOwner{"Higa",
	vector<string>{"Scruffy", "Sam"}},
	PetOwner{"Ashkenazi",
	vector<string>{"Walker", "Sugar"}},
	PetOwner{"Price",
	vector<string>{"Scratches", "Diesel"}},
	PetOwner{"Hines",
	vector<string>{"Dusty"}}};
	assert(ztl::from(petOwners).select([](auto&& a)
	{
		return a.Pets;
	}).equal({vector<string>{"Scruffy", "Sam"}, vector<string>{"Walker", "Sugar"}, vector<string>{"Scratches", "Diesel"}, vector<string>{"Dusty"}}));
}
void test_select_many()
{
	vector<PetOwner> petOwners =
	{
		PetOwner{"Higa",
			vector<string>{"Scruffy", "Sam"}},
		PetOwner{"Ashkenazi",
			vector<string>{"Walker", "Sugar"}},
		PetOwner{"Price",
			vector<string>{"Scratches", "Diesel"}},
		PetOwner{"Hines",
			vector<string>{"Dusty"}}
	};

	assert(ztl::from(petOwners).select_many([](auto&& pet_owner)
	{
		return pet_owner.Pets;
	}, [](auto&& pet_owner,auto&& pet)
	{
		return pet;
	}).equal({"Scruffy", "Sam","Walker", "Sugar", "Scratches", "Diesel", "Dusty"}));
}
void test_inner_jion()
{
	Person magnus =  Person{ "Hedlund, Magnus"};
	Person terry =  Person{ "Adams, Terry"};
	Person charlotte =  Person{ "Weiss, Charlotte"};

	Pet barley =  Pet{ "Barley",  terry};
	Pet boots =  Pet{ "Boots",  terry};
	Pet whiskers =  Pet{ "Whiskers",  charlotte};
	Pet daisy =  Pet{ "Daisy",  magnus};

	vector<Person> people = {magnus, terry, charlotte};
	vector<Pet> pets =  {barley, boots, whiskers, daisy};
	std::function< const std::string (const Person&)> person_functor = [](const Person& person)
	{
		return person.Name;
	};
	std::function< const std::string ( const Pet& )> pet_functor = [](const Pet&pet)
	{
		return pet.Owner.Name;
	};
	std::function<std::pair<const std::string, const std::string>(const Person&, const Pet&)> result_functor = [](const Person& person, const Pet&pet)
	{
		return std::make_pair(person.Name, pet.Name);
	};
	//vector<std::pair<std::string, std::string>> result_list = {};
	auto query =
		ztl::from(people).inner_join(pets.begin(), pets.end(),
		person_functor,
		pet_functor,
		result_functor
		).print();

}
int main()
{
	test_select();
	test_select_many();
	test_inner_jion();
	/*ztl::from(petOwners).select_many([](auto&& a)
	{
		return a.Pets;
	}, [](auto&& a)
	{
		return a;
	}).print();*/

	vector<int> a = { 1, 2, 3, 4, 5 };
	vector<int> b = {10,6,2 };
	
	//ztl::from(a).
	//remove([](auto&& a){return a % 2 == 0;}).
	//select([](auto&& a)
	//{
	//	return a * 2;
	//}).group_by([]
	//	(const int& a)
	//{
	//	return a > 4;
	//}).print_pair();/*order_by([](auto&& a)
	//{
	//	return a;
	//},ztl::order::descending).*/
	//;

	return 0;
}