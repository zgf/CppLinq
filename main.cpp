#include "linq.h"
#include <vector>
#include <iostream>
#include <assert.h>
#include <string>
using std::vector;
using std::cout;
using std::endl;
using std::string;

void test_from()
{
	using ztl::from;
	//////////////////////////////////////////////////////////////////
	// from
	//////////////////////////////////////////////////////////////////
	{
		vector<int> xs = {1, 2, 3, 4, 5};
		int sum = 0;
		for(auto x : from(xs.begin(), xs.end()))
		{
			sum += x;
		}
		assert(sum == 15);
	}
	{
		vector<int> xs = {1, 2, 3, 4, 5};
		int sum = 0;
		for(auto x : from(xs))
		{
			sum += x;
		}
		assert(sum == 15);
	}
	{
		int sum = 0;
		for(auto x :ztl::from({1, 2, 3, 4, 5}))
		{
			sum += x;
		}
		assert(sum == 15);
	}
	{
		int sum = 0;
		for(auto x : ztl::from(ztl::from({1, 2, 3, 4, 5})))
		{
			sum += x;
		}
		assert(sum == 15);
	}
	{
		auto xs = std::make_shared<vector<int>>(vector<int>({1, 2, 3, 4, 5}));
		int sum = 0;
		for(auto x : from(from(ztl::from_shared(xs))))
		{
			sum += x;
		}
		assert(sum == 15);
	}
}

void test_select()
{
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
void test_order_by()
{
	//int xs[] = {7, 1, 12, 2, 8, 3, 11, 4, 9, 5, 13, 6, 10};
	//int ys[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
	//int zs[] = {10, 1, 11, 2, 12, 3, 13, 4, 5, 6, 7, 8, 9};

	//assert(ztl::from(xs).order_by([](int x)
	//{
	//	return x;
	//}).equal(ys));
	class Pet
	{
	public:
		string Name;
		int Age;
		Pet(string name, int age)
		{
			Name = name;
			Age = age;
		}
		bool operator ==( const Pet& p )const
		{
			return Name == p.Name && Age == p.Age;
		}
		bool operator!=( const Pet& p )const
		{
			return !(*this==p);
		}
	};
	vector<Pet> pets = {{"李狗1", 8},
		{"李狗2", 4},
		{"王狗",  1}};
	vector<Pet> test = {{"王狗", 1}, {"李狗2", 4}, {"李狗1", 8}};
	assert(ztl::from(pets).order_by([](auto&& pet)
	{
		return pet.Age;
	}).equal(test));

}
void test_select_many()
{
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
	Person magnus =  Person{ "小明"};
	Person terry =  Person{ "小李"};
	Person charlotte =  Person{ "小王"};

	Pet barley =  Pet{ "李狗1",  terry};
	Pet boots =  Pet{ "李狗2",  terry};
	Pet whiskers =  Pet{ "王狗",  charlotte};
	Pet daisy =  Pet{ "明狗",  magnus};

	vector<Person> people = {magnus, terry, charlotte};
	vector<Pet> pets =  {barley, boots, whiskers, daisy};
	auto query =
		ztl::from(people).inner_join(pets.begin(), pets.end(),
		[](auto&& person)
	{
		return person.Name;
	},
		[](auto&& pet)
	{
		return pet.Owner.Name;
	},
		[](auto&&  person, auto&& pet)
	{
		return std::make_pair(person.Name, pet.Name);
	}).print_pair();

}
void test_where()
{
	vector<string> fruits =
		{"apple", "passionfruit", "banana", "mango",
		"orange", "grape", "strawberry"};

	assert(ztl::from(fruits).where([](auto&&fruit)
	{
		return fruit.size() == 5;
	}).equal({"apple",
		"mango",
		"grape"}));
}
void test_outer_group_jion()
{
	
	class Person
	{
	public:
		string Name;
		bool operator==( const Person& right )
		{
			return Name == right.Name;
		}
		bool operator!=( const Person& right )
		{
			return !( *this == right );
		}
	};

	class Pet
	{
	public:
		string Name;
		Person Owner;
	};
	Person magnus = Person{"小明"};
	Person terry = Person{"小李"};
	Person charlotte = Person{"小王"};
	Person tim = Person{"小菜"};

	Pet barley = Pet{"李狗1", terry};
	Pet boots = Pet{"李狗2", terry};
	Pet whiskers = Pet{"王狗", charlotte};
	Pet daisy = Pet{"明狗", magnus};

	vector<Person> people = {magnus, terry, charlotte, tim};
	vector<Pet> pets = {barley, boots, whiskers, daisy};
	//Pet a;
	//const Pet& b =a;
	auto uu = [](auto&&pet)
	{

		return pet.Name;
	};
	auto query = ztl::from(people).outer_group_join(pets.begin(), pets.end(),
		[](auto&& person)
	{
		return person;
	},
		[](auto&& pet)
	{
		return pet.Owner;
	},
		[&uu](auto&& person, auto&& petCollection)
	{
		return std::make_pair(person.Name, petCollection);// ztl::from(petCollection).select(uu).to_vector());
	});
	
	for(auto obj = query.begin(); obj != query.end(); ++obj)
	{
		auto result = *obj;

		//// Output the owner's name.
		cout << result.first << ":" << endl;
		////// Output each of the owner's pet's names.
		////
		for(auto it = result.second.begin();
			it != result.second.end();
			++it)
		{
			cout << (*it).Name<<endl;
		}
		
	}
}
void test_inner_group_jion()
{

	class Person
	{
	public:
		string Name;
		bool operator==( const Person& right )
		{
			return Name == right.Name;
		}
		bool operator!=( const Person& right )
		{
			return !( *this == right );
		}
	};

	class Pet
	{
	public:
		string Name;
		Person Owner;
	};
	Person magnus = Person{"小明"};
	Person terry = Person{"小李"};
	Person charlotte = Person{"小王"};
	Person tim = Person{"小菜"};

	Pet barley = Pet{"李狗1", terry};
	Pet boots = Pet{"李狗2", terry};
	Pet whiskers = Pet{"王狗", charlotte};
	Pet daisy = Pet{"明狗", magnus};

	vector<Person> people = {magnus, terry, charlotte, tim};
	vector<Pet> pets = {barley, boots, whiskers, daisy};
	//Pet a;
	//const Pet& b =a;
	auto uu = [](auto&&pet)
	{

		return pet.Name;
	};
	auto query = ztl::from(people).inner_group_join(pets.begin(), pets.end(),
		[](auto&& person)
	{
		return person;
	},
		[](auto&& pet)
	{
		return pet.Owner;
	},
		[&uu](auto&& person, auto&& petCollection)
	{
		return std::make_pair(person.Name, ztl::from(petCollection).select(uu).to_vector());
	});

	for(auto obj = query.begin(); obj != query.end();)
	{
		auto result = *obj;

		//// Output the owner's name.
		cout << result.first << ":" << endl;
		////// Output each of the owner's pet's names.
		////
		for(auto it = result.second.begin();
			it != result.second.end();
			++it)
		{
			cout << ( *it ) << endl;
		}
		++obj;
	}
}
int main()
{
	test_from();
	test_where();
	test_select();
	test_select_many();
	test_order_by();
	test_inner_jion();
	test_outer_group_jion();
	test_inner_group_jion();

	return 0;
}