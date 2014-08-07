#include "linq.h"
#include <iostream>
#include <assert.h>
using std::vector;
using std::cout;
using std::endl;
using std::string;
using ztl::from;
void test_from()
{
	//////////////////////////////////////////////////////////////////
	// from
	//////////////////////////////////////////////////////////////////
	{
		vector<int> xs = { 1, 2, 3, 4, 5 };
		int sum = 0;
		for(auto x : from(xs.begin(), xs.end()))
		{
			sum += x;
		}
		assert(sum == 15);
	}
	{
		vector<int> xs = { 1, 2, 3, 4, 5 };
		int sum = 0;
		for(auto x : from(xs))
		{
			sum += x;
		}
		assert(sum == 15);
	}
	{
		int sum = 0;
		for(auto x : ztl::from({ 1, 2, 3, 4, 5 }))
		{
			sum += x;
		}
		assert(sum == 15);
	}
	{
		int sum = 0;
		for(auto x : ztl::from(ztl::from({ 1, 2, 3, 4, 5 })))
		{
			sum += x;
		}
		assert(sum == 15);
	}
	{
		auto xs = std::make_shared<vector<int>>(vector<int>({ 1, 2, 3, 4, 5 }));
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
		PetOwner() = default;
		PetOwner(string _Name, vector<string> _Pets) :Name(_Name), Pets(_Pets)
		{
		}
	};

	vector<PetOwner> petOwners =
	{ PetOwner { "Higa",
	vector<string>{"Scruffy", "Sam"} },
	PetOwner { "Ashkenazi",
	vector<string>{"Walker", "Sugar"} },
	PetOwner { "Price",
	vector<string>{"Scratches", "Diesel"} },
	PetOwner { "Hines",
	vector<string>{"Dusty"} } };
	assert(ztl::from(petOwners).select([](auto&& a)
	{
		return a.Pets;
	}).equal({ vector<string>{"Scruffy", "Sam"}, vector<string>{"Walker", "Sugar"}, vector<string>{"Scratches", "Diesel"}, vector<string>{"Dusty"} }));
}
//void test_order_by()
//{
//
//	class Pet
//	{
//	public:
//		string Name;
//		int Age;
//		Pet(string name, int age)
//		{
//			Name = name;
//			Age = age;
//		}
//		bool operator ==( const Pet& p )const
//		{
//			return Name == p.Name && Age == p.Age;
//		}
//		bool operator!=( const Pet& p )const
//		{
//			return !(*this==p);
//		}
//	};
//	vector<Pet> pets = {{"李狗1", 8},
//		{"李狗2", 4},
//		{"王狗",  1}};
//	vector<Pet> test = {{"王狗", 1}, {"李狗2", 4}, {"李狗1", 8}};
//	assert(ztl::from(pets).order_by([](auto&& pet)
//	{
//		return pet.Age;
//	}).equal(test));
//
//}
//void test_inner_jion()
//{
//	class Person
//	{
//	public:
//		string Name;
//	};
//
//	class Pet
//	{
//	public:
//		string Name;
//		Person Owner;
//	};
//	Person magnus =  Person{ "小明"};
//	Person terry =  Person{ "小李"};
//	Person charlotte =  Person{ "小王"};
//
//	Pet barley =  Pet{ "李狗1",  terry};
//	Pet boots =  Pet{ "李狗2",  terry};
//	Pet whiskers =  Pet{ "王狗",  charlotte};
//	Pet daisy =  Pet{ "明狗",  magnus};
//
//	vector<Person> people = {magnus, terry, charlotte};
//	vector<Pet> pets =  {barley, boots, whiskers, daisy};
//	auto query =
//		ztl::from(people).inner_join(pets.begin(), pets.end(),
//		[](auto&& person)
//	{
//		return person.Name;
//	},
//		[](auto&& pet)
//	{
//		return pet.Owner.Name;
//	},
//		[](auto&&  person, auto&& pet)
//	{
//		return std::make_pair(person.Name, pet.Name);
//	}).to_vector();
//
//}
//void test_outer_group_jion()
//{
//
//	class Person
//	{
//	public:
//		string Name;
//		bool operator==( const Person& right )
//		{
//			return Name == right.Name;
//		}
//		bool operator!=( const Person& right )
//		{
//			return !( *this == right );
//		}
//	};
//
//	class Pet
//	{
//	public:
//		string Name;
//		Person Owner;
//	};
//	Person magnus = Person{"小明"};
//	Person terry = Person{"小李"};
//	Person charlotte = Person{"小王"};
//	Person tim = Person{"小菜"};
//
//	Pet barley = Pet{"李狗1", terry};
//	Pet boots = Pet{"李狗2", terry};
//	Pet whiskers = Pet{"王狗", charlotte};
//	Pet daisy = Pet{"明狗", magnus};
//
//	vector<Person> people = {magnus, terry, charlotte, tim};
//	vector<Pet> pets = {barley, boots, whiskers, daisy};
//	//Pet a;
//	//const Pet& b =a;
//	auto uu = [](auto&&pet)
//	{
//
//		return pet.Name;
//	};
//	auto query = ztl::from(people).outer_group_join(pets.begin(), pets.end(),
//		[](auto&& person)
//	{
//		return person;
//	},
//		[](auto&& pet)
//	{
//		return pet.Owner;
//	},
//		[&uu](auto&& person, auto&& petCollection)
//	{
//		return std::make_pair(person.Name, petCollection);// ztl::from(petCollection).select(uu).to_vector());
//	});
//
//	for(auto obj = query.begin(); obj != query.end(); ++obj)
//	{
//		auto result = *obj;
//
//		//// Output the owner's name.
//		cout << result.first << ":" << endl;
//		////// Output each of the owner's pet's names.
//		////
//		for(auto it = result.second.begin();
//			it != result.second.end();
//			++it)
//		{
//			cout << (*it).Name<<endl;
//		}
//
//	}
//}
//void test_inner_group_jion()
//{
//
//	//class Person
//	//{
//	//public:
//	//	string Name;
//	//	bool operator==( const Person& right )
//	//	{
//	//		return Name == right.Name;
//	//	}
//	//	bool operator!=( const Person& right )
//	//	{
//	//		return !( *this == right );
//	//	}
//	//};
//
//	//class Pet
//	//{
//	//public:
//	//	string Name;
//	//	Person Owner;
//	//};
//	//Person magnus = Person{"小明"};
//	//Person terry = Person{"小李"};
//	//Person charlotte = Person{"小王"};
//	//Person tim = Person{"小菜"};
//
//	//Pet barley = Pet{"李狗1", terry};
//	//Pet boots = Pet{"李狗2", terry};
//	//Pet whiskers = Pet{"王狗", charlotte};
//	//Pet daisy = Pet{"明狗", magnus};
//
//	//vector<Person> people = {magnus, terry, charlotte, tim};
//	//vector<Pet> pets = {barley, boots, whiskers, daisy};
//	////Pet a;
//	////const Pet& b =a;
//	//auto uu = [](auto&&pet)
//	//{
//
//	//	return pet.Name;
//	//};
//	//auto query = ztl::from(people).inner_group_join(pets.begin(), pets.end(),
//	//	[](auto&& person)
//	//{
//	//	return person;
//	//},
//	//	[](auto&& pet)
//	//{
//	//	return pet.Owner;
//	//},
//	//	[&uu](auto&& person, auto&& petCollection)
//	//{
//	//	return std::make_pair(person.Name, ztl::from(petCollection).select(uu));
//	//});
//
//	//for(auto obj = query.begin(); obj != query.end();)
//	//{
//	//	auto result = *obj;
//
//	//	//// Output the owner's name.
//	//	cout << result.first << ":" << endl;
//	//	////// Output each of the owner's pet's names.
//	//	////
//	//	for(auto it = result.second.begin();
//	//		it != result.second.end();
//	//		++it)
//	//	{
//	//		cout << ( *it ) << endl;
//	//	}
//	//	++obj;
//	//}
//}
////
////
////
////check
//
void test_concat()
{
	vector<int> xs = { 1, 2, 3, 4, 5 };
	vector<int> xy = { 1, 2, 3 };
	assert(ztl::from(xs).concat(ztl::from(xy)).equal({ 1, 2, 3, 4, 5, 1, 2, 3 }));
}

void test_where()
{
	vector<string> fruits =
	{ "apple", "passionfruit", "banana", "mango",
	"orange", "grape", "strawberry" };

	assert(ztl::from(fruits).where([](auto&&fruit)
	{
		return fruit.size() == 5;
	}).equal({ "apple",
		"mango",
		"grape" }));
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
		PetOwner { "Higa",
		vector<string>{"Scruffy", "Sam"} },
		PetOwner { "Ashkenazi",
		vector<string>{"Walker", "Sugar"} },
		PetOwner { "Price",
		vector<string>{"Scratches", "Diesel"} },
		PetOwner { "Hines",
		vector<string>{"Dusty"} }
	};

	assert(ztl::from(petOwners).select_many([](auto&& pet_owner)
	{
		return pet_owner.Pets;
	}, [](auto&& pet_owner, auto&& pet)
	{
		return pet;
	}).equal({ "Scruffy", "Sam", "Walker", "Sugar", "Scratches", "Diesel", "Dusty" }));
	assert(ztl::from(petOwners).select_many([](auto&& pet_owner)
	{
		return pet_owner.Pets;
	}).equal({ "Scruffy", "Sam", "Walker", "Sugar", "Scratches", "Diesel", "Dusty" }));
}
void test_count()
{
	struct Person
	{
		int age;
		string name;
		string address;
	};

	vector<Person>v = { { 21, "a", "shanghai" }, { 22, "bb", "wuhan" }, { 21, "a", "zhuhai" } };
	assert(ztl::from(v).where([](const Person& p)
	{
		return p.age > 20;
	}).count() == 3);
}
struct Person
{
	int age;
	string name;
	string address;
	bool operator <(const Person& p)
	{
		return age < p.age;
	}
	friend bool operator==(const Person& l, const Person& p)
	{
		return l.age == p.age;
	}
	friend bool operator!=(const Person& l, const Person& p)
	{
		return !(l == p);
	}
};
void test_groupby()
{
	vector<Person>v = { { 21, "a", "shanghai" }, { 22, "bb", "wuhan" }, { 21, "a", "zhuhai" } };
	vector<Person>w = { { 21, "a", "shanghai" }, { 21, "a", "zhuhai" }, { 22, "bb", "wuhan" } };

	assert(from(v).group_by([](const Person& p)
	{
		return p.age;
	})
		.select_many([](auto&& element)
	{
		return element.second;
	})
	.order_by([](auto&& element)
	{
		return element.age;
	}).equal(w));
	
}
void test_aggregate()
{
	vector<string> strv = { "a", "b", "c" };
	assert(from(strv).aggregate([](const string& str1, const string& str2)
	{
		return str1 + str2;
	}) == string("abc"));
}
void test_distinct()
{
	vector<int>v = { 0, 1, 2, 3, 4, 5, 3, 6 };
	assert(from(v).distinct().order_by([](auto&& element)
	{
		return element;
	}).equal({ 0, 1, 2, 3, 4, 5, 6 }));
	assert(from(v).count() == 8);
	assert(from(v).sum() == 24);
	assert(from(v).average() == 3);
	assert(from(v).min_value() == 0);
	assert(from(v).max_value() == 6);
	assert(from(v).first() == 0);
	assert(from(v).first([](auto&&element)
	{
		return element <= 2;
	}) == 0);
	assert(from(v).last() == 6);
	assert(from(v).last([](auto&&element)
	{
		return element <= 2;
	}) == 2);
	assert(from(v).any([](auto&&element)
	{
		return element <= 2;
	}) == true);
	assert(from(v).all([](auto&&element)
	{
		return element <= 2;
	}) == false);
	assert(from(v).none([](auto&&element)
	{
		return element <= 2;
	}) == false);
}
void test_element_at()
{
	vector<int>v = { 0, 1, 2, 3, 4, 5, 3, 6 };
	assert(from(v).element_at(5) == 5);
	assert(from(v).element_at(6) == 3);
	assert(from(v).element_at(7) == 6);

}
void test_reverse()
{
	vector<int>v = { 0, 1, 2, 3, 4, 5, 3, 6 };
	assert(from(v).distinct().order_by([](auto&& element)
	{
		return element;
	}).reverse().equal({ 6,5,4,3,2,1,0 }));
}
void test_take()
{
	vector<int>v = { 0, 1, 2, 3, 4, 5, 3, 6 };
	assert(from(v).take(3).equal({ 0, 1, 2 }));
	assert(from(v).take_while([](auto&& element)
	{
		return element <= 2;
	}).equal({ 0, 1, 2 }));
}
void test_skip()
{
	vector<int>v = { 0, 1, 2, 3, 4, 5, 3, 6 };
	assert(from(v).skip(3).equal({ 3, 4, 5, 3, 6 }));
	assert(from(v).skip_while([](auto&& element)
	{
		return element <= 2;
	}).equal({ 3, 4, 5, 3, 6 }));
}
void test_remove()
{
	vector<string> fruits =
	{ "apple", "passionfruit", "banana", "mango",
	"orange", "grape", "strawberry" };

	assert(from(fruits).remove([](auto&&fruit)
	{
		return fruit.size() == 5;
	}).equal({ "passionfruit",
		"banana",
		"orange",
		"strawberry" }));
	
		
}
void test_contains()
{
	vector<string> fruits =
	{ "apple", "passionfruit", "banana", "mango",
	"orange", "grape", "strawberry" };
	assert(from(fruits).contains("passionfruit") == true);
	assert(from(fruits).contains("ssionfruit") != true);

}
int main()
{
	test_from();
	test_select();
	test_concat();
	test_where();
	test_select_many();
	test_count();
	test_groupby();
	test_aggregate();
	test_distinct();
	test_element_at();
	test_reverse();
	test_take();
	test_skip();
	test_remove();
	/*

		test_order_by();
		test_inner_jion();
		test_outer_group_jion();
		test_inner_group_jion();*/
	return 0;
}

//void other_select_imp_problem()
//{
//	vector<int> xs = { 1, 2, 3, 4, 5 };
//	vector<vector<int>> a = { { 1, 2, 3 } };
//	//using type = ;
//	//ztl::return_value(xs);
//	auto query = ztl::from(a).select([](auto&& element)
//	{
//		return element;
//	}).aggregate([](auto&& init, auto&& element)
//	{
//		return init.concat(ztl::from(element));
//	}, ztl::from_empty<int>());
//
//	query.begin() == query.end();
//	for(auto it = query.begin(); it != query.end(); ++it)
//	{
//		auto result = *it;
//		cout << result;
//	}
//}