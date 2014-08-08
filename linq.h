#pragma once
#include <functional>
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <numeric>
#include <exception>
#include <string>
//#include <tuple>
#include "check_type.hpp"
namespace ztl
{
#define COMMON_ITERATOR_DECLARE(SELF_TYPE,ITEATOR_DIFFERENCE_TYPE) \
	using self_type = SELF_TYPE;\
	using value_type		= decltype(*std::declval<self_type>() );\
	using pointer			= value_type*;\
	using reference			= value_type&;\
	using difference_type	=typename std::iterator_traits<ITEATOR_DIFFERENCE_TYPE>::difference_type;\
	using iterator_category = std::forward_iterator_tag;

#define ALIAS_FORWARD_FUNCTION(ALIAS_NAME,REALLY_FUNCTION_NAME) \
	template<typename... args_type>\
	auto ALIAS_NAME(args_type&&... args)\
	{\
	return REALLY_FUNCTION_NAME(std::forward<args_type...>(args)...);\
}

#define STRUCT_NAME(NAME) \
	struct NAME \
	{ \
};
	STRUCT_NAME(iterator_type);
	STRUCT_NAME(key_selector_type);
	STRUCT_NAME(predicate_type);
	STRUCT_NAME(container_type);
	STRUCT_NAME(selector_type);
	STRUCT_NAME(collection_selector_type);
	STRUCT_NAME(result_selector_type);
	STRUCT_NAME(inner_iterator_type);
	STRUCT_NAME(outer_key_selector_type);
	STRUCT_NAME(inner_key_selector_type);
	STRUCT_NAME(outer_iterator_type);
	STRUCT_NAME(value_type);

#undef STRUCT_NAME

#define RETURN_VALUE_TYPE(RETURN_VALUE)\
	typename std::remove_reference<decltype(RETURN_VALUE)>::type

#define PROTECT_PARAMETERS(...) __VA_ARGS__

#define EQUAL_OPERATOR(LEFT,RIGHT)\
	bool operator==(const self_type& right) const\
	{\
	return 	std::tie(LEFT) == std::tie(RIGHT);\
	}\
	bool operator!=(const self_type& right) const\
	{\
	return !(*this == right);\
	}
#define PRODUCT_EQUAL_OPERATOR(A,B) \
	EQUAL_OPERATOR(PROTECT_PARAMETERS##A,PROTECT_PARAMETERS##B);

	class linq_exception
	{
	private:
		std::string message;
	public:
		linq_exception() = delete;
		linq_exception(const char* _message) :message(_message)
		{
		}
		std::string what()const
		{
			return message;
		}
	};

	template<typename UnaryPredicateType>
	struct unary_negate
	{
		UnaryPredicateType pred;
		unary_negate(const UnaryPredicateType& tpred) :pred(tpred)
		{
		}
		unary_negate(UnaryPredicateType&& tpred) :pred(std::move(tpred))
		{
		}
		template<typename Type>
		bool operator()(Type&& arg) const
		{
			return !pred(std::forward<Type>(arg));
		}
	};
	template<typename iterator_type1, typename iterator_type2>
	bool equal(iterator_type1 first1, iterator_type1 last1, iterator_type2 first2, iterator_type2 last2)
	{
		while(first1 != last1 && first2 != last2)
		{
		/*	auto f1 = *first1;
			auto f2 = *first2;
			string a= check_type<decltype(f1)>();
			string b =check_type<decltype(f2)>();
*/
			if(*first1 != *first2)
			{
				return false;
			}
			++first1;
			++first2;
		}
		return first1 == last1 && first2 == last2;
	}

	//always_true

	template<typename value_type>
	struct always_true
	{
		bool operator()(const value_type& value)const
		{
			return true;
		}
	};
	template<typename value_type>
	struct always_false
	{
		bool operator()(const value_type& value)const
		{
			return false;
		}
	};

	template<typename container_type>
	std::true_type check_container(const container_type & t,
		decltype(t.begin())* p1 = 0,
		decltype(t.end())* p2 = 0,
		decltype(t.size())* p3 = 0,
		decltype(t.empty())* p4 = 0
		);
	std::false_type check_container(...);

	template<typename return_type>
	auto container_retun_shared(return_type&& value, std::true_type)
	{
		return from_shared(std::make_shared<typename std::remove_reference<return_type>::type>(value));
	}
	template<typename return_type>
	auto container_retun_shared(return_type&& value, std::false_type)
	{
		return value;
	}
	template<typename T>
	auto return_value(T&& value)
	{
		return	container_retun_shared(std::forward<T>(value), decltype(check_container(std::forward<T>(value)))());
	}

	//iterator

	//skip
	template<typename element_type>
	class empty_iterator
	{
	public:
		using self_type = empty_iterator;
		using value_type = element_type;
		using pointer = element_type*;
		using reference = element_type&;
		using difference_type = size_t;
		using iterator_category = std::forward_iterator_tag;

	public:
		element_type operator*()const
		{
			return element_type();
		}
	public:
		//COMMON_ITERATOR_DECLARE(empty_iterator, );
	public:

		self_type& operator++()
		{
			return *this;
		}

		self_type operator++(int)
		{
			return *this;
		}

	public:
		bool operator==(const self_type& right) const
		{
			return true;
		}
		bool operator!=(const self_type& right) const
		{
			return false;
		}
	};

	//template<typename element_type>
	//class hide_type_iterator
	//{
	//public:
	//	using self_type = hide_type_iterator;
	//	using value_type = element_type;
	//	using pointer = element_type*;
	//	using reference = element_type&;
	//	using difference_type = size_t;
	//	using iterator_category = std::forward_iterator_tag;
	//	hide_type_iterator()
	//	{
	//	}
	//private:
	//	class iterator_interface
	//	{
	//	public:
	//		virtual element_type dref() = 0;
	//		virtual bool equal(const std::shared_ptr<iterator_interface>&) = 0;
	//		virtual void next() = 0;
	//	};
	//	template<typename iterator_type>
	//	class iterator_implement :public iterator_interface
	//	{
	//	private:
	//		iterator_type iterator;
	//	public:
	//		iterator_implement(const iterator_type& _iterator) :iterator(_iterator)
	//		{
	//		}
	//	public:
	//		element_type dref() override
	//		{
	//			return *iterator;
	//		}
	//		bool equal(const std::shared_ptr<iterator_interface>& right)override
	//		{
	//			auto impl = std::dynamic_pointer_cast<iterator_implement<iterator_type>>(right);
	//			//cout << "look" << endl;
	//			//cout << check_type<decltype(impl->iterator)>() << endl;
	//			//cout << check_type<decltype(iterator)>() << endl;

	//			return impl && (iterator == impl->iterator);
	//		}
	//		void next()override
	//		{
	//			++iterator;
	//		}
	//	};
	//private:
	//	std::shared_ptr<iterator_interface> iterator;
	//public:
	//	template<typename iterator_type>
	//	hide_type_iterator(const iterator_type& _iterator)
	//		:iterator(std::shared_ptr<iterator_implement<iterator_type>>(new iterator_implement<iterator_type>(_iterator)))
	//	{
	//	}
	//public:
	//	element_type operator*()const
	//	{
	//		return iterator->dref();
	//	}
	//	self_type& operator++()
	//	{
	//		iterator->next();
	//		return *this;
	//	}
	//	self_type operator++(int)
	//	{
	//		auto temp = *this;
	//		++*this;
	//		return temp;
	//	}
	//	bool operator==(const self_type& right)const
	//	{
	//		return iterator->equal(right.iterator);
	//	}
	//	bool operator!=(const self_type& right)const
	//	{
	//		return !(*this == right);
	//	}
	//};

	//where
	template<typename iterator_type, typename predicate_type>
	class where_iterator
	{
	private:
		iterator_type iterator;
		iterator_type end;
		predicate_type pred;
	public:
		auto operator*()->RETURN_VALUE_TYPE(*iterator) const
		{
			return *iterator;
		}
	public:
		COMMON_ITERATOR_DECLARE(where_iterator, iterator_type);
	public:
		where_iterator() = delete;
		where_iterator(const iterator_type& _iterator, const iterator_type& _end, const predicate_type& _pred) :iterator(_iterator), end(_end), pred(_pred)
		{
			move_iterator(false);
		}
	private:
		//从当前开始,移动到下一个结果为true的迭代器上,如果当前true,不移动
		void move_iterator(bool current)
		{
			if(iterator != end)
			{
				if(current)
				{
					++iterator;
				}
				while(iterator != end && !pred(*iterator))
				{
					++iterator;
				}
			}
		}
	public:

		self_type& operator++()
		{
			move_iterator(true);
			return *this;
		}

		self_type operator++(int)
		{
			self_type temp = *this;
			move_iterator(true);
			return temp;
		}

	public:
		bool operator==(const self_type& right) const
		{
			return iterator == right.iterator;
		}
		bool operator!=(const self_type& right) const
		{
			return !(*this == right);
		}
	};

	//skip
	template<typename iterator_type, typename predicate_type>
	class skip_iterator
	{
	private:
		iterator_type iterator;
		iterator_type end;
		predicate_type pred;
	public:
		auto operator*()->RETURN_VALUE_TYPE(*iterator) const
		{
			return *iterator;
		}
	public:
		COMMON_ITERATOR_DECLARE(skip_iterator, iterator_type);
	public:
		skip_iterator() = delete;
		skip_iterator(const iterator_type& _iterator, const iterator_type& _end, const predicate_type& _pred)
			:end(_end), pred(_pred)
		{
			for(iterator = _iterator; iterator != end && pred(*iterator); ++iterator)
			{
			}
		}
	public:

		self_type& operator++()
		{
			++iterator;
			return *this;
		}

		self_type operator++(int)
		{
			self_type temp = *this;
			++*this;
			return temp;
		}

	public:
		bool operator==(const self_type& right) const
		{
			return iterator == right.iterator;
		}
		bool operator!=(const self_type& right) const
		{
			return !(*this == right);
		}
	};

	//take
	template<typename iterator_type, typename predicate_type>
	class take_iterator
	{
	private:
		iterator_type iterator;
		iterator_type end;
		predicate_type pred;
	public:
		auto operator*()->RETURN_VALUE_TYPE(*iterator) const
		{
			return *iterator;
		}
	public:
		COMMON_ITERATOR_DECLARE(take_iterator, iterator_type);
	public:
		take_iterator() = delete;
		take_iterator(const iterator_type& _iterator, const iterator_type& _end, const predicate_type& _pred)
			:pred(_pred)
		{
			if(_iterator != _end)
			{
				//end迭代器
				for(iterator = _iterator; iterator != _end && pred(*iterator); ++iterator)
				{
				}
				end = iterator;
			}
			else
			{
				iterator = _iterator;
				end = _end;
			}
		}
	public:

		self_type& operator++()
		{
			++iterator;
			return *this;
		}

		self_type operator++(int)
		{
			self_type temp = *this;
			++*this;
			return temp;
		}

	public:
		bool operator==(const self_type& right) const
		{
			return iterator == right.iterator;
		}
		bool operator!=(const self_type& right) const
		{
			return !(*this == right);
		}
	};

	//select

	template<typename iterator_type, typename selector_type>
	class select_iterator
	{
	private:
		iterator_type iterator;
		selector_type selector;
	public:

		auto operator*()->
			RETURN_VALUE_TYPE(selector(*iterator)) const
			//RETURN_VALUE_TYPE(return_value(selector(*iterator))) const
		{
				return selector(*iterator);
		}
		COMMON_ITERATOR_DECLARE(select_iterator, iterator_type);
	public:
		select_iterator() = delete;
		select_iterator(const iterator_type& _iterator, const selector_type& _selector) :iterator(_iterator), selector(_selector)
		{
		}

	public:

		self_type& operator++()
		{
			++iterator;
			return *this;
		}

		self_type operator++(int)
		{
			self_type temp = *this;
			++*this;
			return temp;
		}

	public:
		bool operator==(const self_type& right)const
		{
			return iterator == right.iterator;
		}
		bool operator!=(const self_type& right)const
		{
			return !(operator==(right));
		}
	};

	template<typename iterator_type, typename collection_selector_type, typename result_selector_type>
	class select_many_iterator
	{
	private:
		iterator_type					iterator;
		iterator_type					end;
		collection_selector_type		collection_selector;
		result_selector_type			result_selector;
		int								subindex;
		int								current_size;

	public:

		auto operator*()
			->RETURN_VALUE_TYPE(result_selector(*iterator,
			*(collection_selector(*iterator).begin()))) const
		{
				return result_selector(*iterator, *std::next(collection_selector(*iterator).begin(), subindex));
		}
		//这个方法好啊!!!泪奔┭┮﹏┭┮;
		COMMON_ITERATOR_DECLARE(select_many_iterator, iterator_type);
	public:
		select_many_iterator() = delete;
		select_many_iterator(const iterator_type& _iterator, const iterator_type& _end,
			const collection_selector_type& _collection_selector, const result_selector_type& _result_selector)
			: iterator(_iterator), end(_end),
			collection_selector(_collection_selector), result_selector(_result_selector)
		{
			move_iterator();
		}
	public:
		self_type& operator++()
		{
			if(iterator != end)
			{
				if(subindex < current_size)
				{
					++subindex;
				}
				if(subindex >= current_size)
				{
					++iterator;
					move_iterator();
				}
			}
			return *this;
		}

		self_type operator++(int)
		{
			self_type temp = *this;
			++*this;
			return temp;
		}

	public:
		bool operator==(const self_type& right) const
		{
			return iterator == right.iterator;
		}
		bool operator!=(const self_type& right) const
		{
			return !(operator==(right));
		}
	private:
		void move_iterator()
		{
			if(iterator != end)
			{
				while(iterator != end && (collection_selector(*iterator)).size() == 0)
				{
					++iterator;
				}
				current_size = (collection_selector(*iterator)).size();
				subindex = 0;
			}
		}
	};

	//zip
	template<typename iterator_type1, typename iterator_type2, typename result_selector_type>
	class zip_iterator
	{
	private:
		iterator_type1			first1;
		iterator_type1			last1;
		iterator_type2			first2;
		iterator_type2			last2;
		result_selector_type	result_selector;
	public:
		auto operator*()
			->RETURN_VALUE_TYPE(result_selector(*first1, *first2)) const
		{
				return result_selector(*first1, *first2);
		}

		COMMON_ITERATOR_DECLARE(zip_iterator,
			iterator_type1);
	public:
		zip_iterator(const iterator_type1& _first1, const iterator_type1& _last1,
			const iterator_type2& _first2, const iterator_type2& _last2,
			const result_selector_type& _result_selector)
			:first1(_first1), last1(_last1), first2(_first2), last2(_last2), result_selector(_result_selector)
		{
		}
	public:

		self_type& operator++()
		{
			if(first1 != last1 && first2 != last2)
			{
				++first1;
				++first2;
			}
			return *this;
		}

		self_type operator++(int)
		{
			self_type temp = *this;
			++*this;
			return temp;
		}

	public:
		bool operator==(const self_type& right)const
		{
			return first1 == right.first1
				&& last1 == right.last1;
		}
		bool operator!=(const self_type& right)const
		{
			return !(operator==(right));
		}
	};
	//group_jion_iter
	template<typename outer_iterator_type, typename inner_iterator_type, typename outer_key_selector_type,
		typename inner_key_selector_type, typename result_selector_type>
	class group_jion_iterator
	{
	private:
		outer_iterator_type		first1;
		outer_iterator_type		last1;
		inner_iterator_type		first2;
		inner_iterator_type		last2;
		inner_key_selector_type inner_key_selector;
		outer_key_selector_type outer_key_selector;
		result_selector_type	result_selector;
	public:

		auto operator*()->RETURN_VALUE_TYPE(
			result_selector(*first1, std::declval < std::vector < typename
			std::iterator_traits<inner_iterator_type>::value_type
			>> ())) const
		{
			using inner_value_type = typename std::iterator_traits<inner_iterator_type>::value_type;
			std::vector<inner_value_type> value_list;
			for(auto next2 = first2; next2 != last2; ++next2)
			{
				if(outer_key_selector(*first1) == inner_key_selector(*next2))
				{
					value_list.push_back(*next2);
				}
			}
			return result_selector(*first1, value_list);
		}
	public:
		COMMON_ITERATOR_DECLARE(
			group_jion_iterator,
			outer_iterator_type);
	public:
		group_jion_iterator() = delete;
		group_jion_iterator(
			const outer_iterator_type& first1_, const outer_iterator_type& last1_,
			const inner_iterator_type& first2_, const inner_iterator_type& last2_,
			const outer_key_selector_type& outer_key_selector_,
			const inner_key_selector_type& inner_key_selector_,
			const result_selector_type& result_selector_)
			:
			first1(first1_), last1(last1_), first2(first2_), last2(last2_),
			inner_key_selector(inner_key_selector_), outer_key_selector(outer_key_selector_),
			result_selector(result_selector_)
		{
		}

		self_type& operator++()
		{
			++first1;
			return *this;
		}

		self_type operator++(int)
		{
			self_type temp = *this;
			++(*this);
			return temp;
		}
		bool operator==(const self_type& right)const
		{
			return first1 == right.first1&& last1 == right.last1;
		}
		bool operator!=(const self_type& right)const
		{
			return !(operator==(right));
		}
	};

	//为了解决中间临时变量的生存周期问题
	template<typename iterator_type, typename container_type>
	class storage_iterator
	{
	};
	template<typename iterator_type, typename container_type>
	class storage_iterator<iterator_type, std::shared_ptr<container_type>>
	{
	private:
		iterator_type iterator;
		std::shared_ptr<container_type> shared_container;
	public:
		auto operator*()->RETURN_VALUE_TYPE(*iterator) const
		{
			return *iterator;
		}
	public:
		COMMON_ITERATOR_DECLARE(storage_iterator, iterator_type);
	public:
		storage_iterator() = delete;
		storage_iterator(const iterator_type& _iterator, const std::shared_ptr<container_type>& _container)
			:iterator(_iterator), shared_container(_container)
		{
		}
	public:

		self_type& operator++()
		{
			++iterator;
			return *this;
		}

		self_type operator++(int)
		{
			self_type temp = *this;
			++*this;
			return temp;
		}

	public:
		bool operator==(const self_type& right)const
		{
			return iterator == right.iterator;
		}
		bool operator!=(const self_type& right)const
		{
			return !(operator==(right));
		}
	};

	template<typename iterator_type1, typename iterator_type2>
	class concat_iterator
	{
	private:
		iterator_type1 first1;
		iterator_type1 last1;
		iterator_type2 first2;
		iterator_type2 last2;
	public:
		concat_iterator(const iterator_type1& _first1, const iterator_type1& _last1, const iterator_type2& _first2, const iterator_type2& _last2)
			:first1(_first1), last1(_last1), first2(_first2), last2(_last2)
		{
		}
	public:
		auto operator*()->RETURN_VALUE_TYPE(*first2) const
		{
			if(first1 == last1)
			{
				return *first2;
			}
			else
			{
				return *first1;
			}
		}
	public:
		COMMON_ITERATOR_DECLARE(concat_iterator, iterator_type1);
	public:

		self_type& operator++()
		{
			if(first1 != last1)
			{
				++first1;
			}
			else if(first2 != last2)
			{
				++first2;
			}
			return *this;
		}

		self_type operator++(int)
		{
			self_type temp = *this;
			++*this;
			return temp;
		}

	public:
		bool operator==(const self_type& right)const
		{
			//cout << check_type<decltype(first1)>() << endl;

			//cout << check_type<decltype(first2)>()<<endl;
			if(first1 == last1)
			{
				//cout << *(right.first2) << endl;
				//cout <<*first2 << endl;

				return first2 == right.first2;
			}
			else
			{
				return first1 == right.first1;
			}

			//	return first1 == right.first1 && first2 == right.first2;
		}
		bool operator!=(const self_type& right)const
		{
			return !(operator==(right));
		}
	};

	enum class order :bool
	{
		ascending,
		descending,
	};
	template<typename iterator_type>
	class enumerable;


	/*template<typename element_type>
	class linq : public enumerable<hide_type_iterator<element_type>>
	{
	public:
		template<typename iterator_type>
		linq(const enumerable<iterator_type> container)
			:enumerable<hide_type_iterator<element_type>>(
			hide_type_iterator<element_type>(container.begin()),
			hide_type_iterator<element_type>(container.end()))
		{
		}
	};*/
	template<typename iterator_type>
	class enumerable
	{
	public:
		using self_type = enumerable;
		using value_type = typename std::iterator_traits<iterator_type>::value_type;
		using pointer = value_type*;
		using reference = value_type&;
		using difference_type = typename std::iterator_traits<iterator_type>::difference_type;
		using iterator_category = std::forward_iterator_tag;
	private:
		iterator_type	_begin;
		iterator_type	_end;
	public:
		enumerable()
		{
		}
		enumerable(const iterator_type& t_begin, const iterator_type& t_end) :_begin(t_begin), _end(t_end)
		{
		}
		template<typename container_type>
		enumerable(const container_type& container)
			: _begin(container.begin()), _end(container.end())
		{
		}

		iterator_type begin() const
		{
			return _begin;
		}
		iterator_type  end() const
		{
			return _end;
		}

		template<typename predicate_type>
		decltype(auto) where(const predicate_type& pred)
		{
			return enumerable<where_iterator<iterator_type, predicate_type>>
				(
				where_iterator<iterator_type, predicate_type>(_begin, _end, pred),
				where_iterator<iterator_type, predicate_type>(_end, _end, pred)
				);
		}


		template<typename predicate_type>
		decltype(auto) remove(predicate_type&& pred)
		{
			using negate_predicate = ztl::unary_negate<typename std::remove_reference<predicate_type>::type>;
			return where(negate_predicate(std::forward<negate_predicate>(pred)));
		}

		template<typename selector_type>
		decltype(auto) select(const selector_type&result_selector)
		{
			return enumerable<select_iterator<iterator_type, selector_type>>(
				select_iterator<iterator_type, selector_type>(_begin, result_selector),
				select_iterator<iterator_type, selector_type>
				(_end, result_selector));
		}

		template<typename collection_selector_type>
		decltype(auto) select_many(const collection_selector_type& collection_selector)
		{
			auto result_selector = [](auto&& collection_element, auto&& element)
			{
				return element;
			};
			return select_many(collection_selector, result_selector);
			/*using element_type = std::remove_reference<decltype(
				*collection_selector(
				*_begin)
				.begin())>::type;

				return select(collection_selector).aggregate([](auto&& init, auto&& element)
				{
				return init.concat(ztl::from(element));
				}, from_empty<element_type>());*/
		}

		template<typename collection_selector_type, typename result_selector_type>
		decltype(auto) select_many(const collection_selector_type& collection_selector, const result_selector_type& result_selector)
		{
			using select_many_type = select_many_iterator<typename std::decay<decltype(begin())>::type, collection_selector_type, result_selector_type>;

			return enumerable<select_many_type>(
				select_many_type(begin(), end(), collection_selector, result_selector),
				select_many_type(end(), end(), collection_selector, result_selector));
		}

		template<typename predicate_type>
		decltype(auto) order_by(const predicate_type& key_selector, order cending = order::ascending)
		{
			std::function<bool(const value_type&, const value_type&)>functor;
			auto& result = std::make_shared<std::vector<value_type>>(_begin, _end);
			if(cending == order::ascending)
			{
				functor = [&key_selector](auto&& left, auto&& right)
				{
					return key_selector(left) < key_selector(right);
				};
			}
			else
			{
				functor = [&key_selector](auto&& left, auto&& right)
				{
					return key_selector(left) > key_selector(right);
				};
			}
			std::sort(result->begin(), result->end(), functor);
			return from_shared(result->begin(), result->end(), result);
		}

		////分组外连接和分组内连接搞定

		template<typename inner_iterator_type, typename outer_key_selector_type,
			typename inner_key_selector_type, typename result_selector_type>
			decltype(auto) outer_group_join(
			const inner_iterator_type& inner_begin,
			const inner_iterator_type& inner_end,
			const outer_key_selector_type& out_key_selector,
			const inner_key_selector_type& inner_key_selector,
			const result_selector_type& reuslt_selector)
		{
				using outer_group_join_type = group_jion_iterator<
					iterator_type,
					inner_iterator_type,
					outer_key_selector_type,
					inner_key_selector_type,
					result_selector_type>;
				return enumerable<outer_group_join_type>(
					outer_group_join_type(begin(), end(), inner_begin, inner_end,
					out_key_selector, inner_key_selector, reuslt_selector),
					outer_group_join_type(end(), end(), inner_end, inner_end,
					out_key_selector, inner_key_selector, reuslt_selector));
		}
		template<typename inner_iterator_type, typename outer_key_selector_type,
			typename inner_key_selector_type, typename result_selector_type>
			decltype(auto) inner_group_join(
			const inner_iterator_type& inner_begin,
			const inner_iterator_type& inner_end,
			const outer_key_selector_type& out_key_selector,
			const inner_key_selector_type& inner_key_selector,
			const result_selector_type& reuslt_selector)
		{
				auto& selector = [](auto&& first, auto&& second)
				{
					return std::make_pair(first, second);
				};
				return outer_group_join(inner_begin, inner_end, out_key_selector, inner_key_selector, selector)
					.where([](auto&& pair_element)
				{
					return pair_element.second.size() != 0;
				}).select([&reuslt_selector](auto&& element)
				{
					return reuslt_selector(element.first, element.second);
				});
		}

		template<typename inner_iterator_type, typename outer_key_selector_type,
			typename inner_key_selector_type, typename result_selector_type>
			decltype(auto) inner_join(
			const inner_iterator_type& inner_begin,
			const inner_iterator_type& inner_end,
			const outer_key_selector_type& out_key_selector,
			const inner_key_selector_type& inner_key_selector,
			const result_selector_type& result_selector)
		{
				return	inner_group_join(inner_begin, inner_end, out_key_selector, inner_key_selector, [](auto&& outer_element, auto&& inner_list)
				{
					return std::make_pair(outer_element, inner_list);
				}).select_many([](auto&& pair_element)
				{
					return pair_element.second;
				}, [&result_selector](auto&& pair_element, auto&& element)
				{
					return result_selector(pair_element.first, element);
				});
		}

		//template<typename inner_iterator_type, typename outer_key_selector_type,
		//	typename inner_key_selector_type, typename result_selector_type>
		//	decltype(auto) outer_join(
		//	const inner_iterator_type& inner_begin,
		//	const inner_iterator_type& inner_end,
		//	const outer_key_selector_type& out_key_selector,
		//	const inner_key_selector_type& inner_key_selector,
		//	const result_selector_type& result_selector)
		//{
		//		/*return	outer_group_join(inner_begin, inner_end, out_key_selector, inner_key_selector, [](auto&& outer_element, auto&& inner_list)
		//		{
		//			return std::make_pair(outer_element, inner_list);
		//		}).select_many([&result_selector](auto&& pair_element)
		//		{
		//			return pair_element.second;
		//		}, [&result_selector](auto&& pair_element, auto&& element)
		//		{
		//			return result_selector(pair_element.first, element);
		//		});*/
		//		/*auto query = inner_join(inner_begin, inner_end, out_key_selector, inner_key_selector, [](auto&& outer_element, auto&& inner_list)
		//		{
		//			return std::make_pair(outer_element, inner_list);
		//		});*/

		//}

		template<typename key_selector_type>
		decltype(auto) group_by(const key_selector_type& key_selector)
		{
			using key_type = std::remove_reference<decltype(key_selector(std::declval<value_type>()))>::type;
			using element_type = value_type;
			auto hash_map = std::make_shared<std::unordered_map<key_type, std::vector<element_type>>>();
			for(auto it = _begin; it != _end; ++it)
			{
				auto value = *it;
				auto key = key_selector(value);
				auto& find_iter = hash_map->find(key);
				if(find_iter == hash_map->end())
				{
					hash_map->insert(std::make_pair(key, std::vector<element_type>(1, value)));
				}
				else
				{
					hash_map->operator[](key).emplace_back(value);
				}
			}
			return from_shared(hash_map->begin(), hash_map->end(), hash_map);
		}

		template<typename key_selector_type, typename element_selector_type>
		decltype(auto) group_by(const key_selector_type& key_selector, const element_selector_type& element_selector)
		{
			return group_by(key_selector).select([&element_selector](auto&& enumerable_list)
			{
				using element_type = decltype(element_selector(*std::begin(enumerable_list.second)));
				using key_type = decltype(enumerable_list.first);
				std::vector< element_type> des_list;
				for(auto it = std::begin(enumerable_list.second), it != std::end(enumerable_list.second); ++it)
				{
					des_list.push_back(element_selector(*it));
				}
				return make_pair(enumerable_list.first, std::move(des_list));
			});
		}
		template<typename key_selector_type, typename result_selector_type, typename element_selector_type>
		decltype(auto) group_by(const key_selector_type& key_selector, const element_selector_type& element_selector, const result_selector_type& result_selector)
		{
			return group_by(key_selector, element_selector).select(result_selector);
		}

		template<typename iterator_type1, typename iterator_type2, typename result_selector_type>
		void zip(const iterator_type1& first1, const iterator_type1& last1, const iterator_type2& first2, const iterator_type& last2, const result_selector_type& result_selector)
		{
			using zip_type = zip_iterator<iterator_type1, iterator_type2, result_selector_type>;
			return enumerable<zip_type>
				(
				zip_type(first1, last1, first2, last2, result_selector),
				zip_type(last1, last1, last2, last2));
		}

		template<typename iterator_type>
		auto  equal(const iterator_type& t_begin, const iterator_type& t_end)const
		{
			return ztl::equal(_begin, _end, t_begin, t_end);
		}
		template<typename container_type>
		decltype(auto) equal(container_type&& container)const
		{
			return equal(std::begin(std::forward<container_type>(container)), std::end(std::forward<container_type>(container)));
		}
		template<typename element_type>
		decltype(auto) equal(const std::initializer_list<element_type>& e)const
		{
			return equal(std::begin(e), std::end(e));
		}

		/*

		SequenceEqual<TSource>(IEnumerable<TSource>, IEnumerable<TSource>)
		通过使用相应类型的默认相等比较器对序列的元素进行比较，以确定两个序列是否相等。
		SequenceEqual<TSource>(IEnumerable<TSource>, IEnumerable<TSource>, IEqualityComparer<TSource>)
		使用指定的 IEqualityComparer<T> 对两个序列的元素进行比较，以确定序列是否相等。
		*/
		ALIAS_FORWARD_FUNCTION(sequence_equal, equal);

		void print_pair()
		{
			for(; _begin != _end; ++_begin)
			{
				auto t = *_begin;

				std::cout << t.first << " " << t.second << " " << std::endl;
			}
		}
		void print()
		{
			for(; _begin != _end; ++_begin)
			{
				auto t = *_begin;

				std::cout << t  << " " << std::endl;
			}
			/*std::for_each(_begin, _end, [](auto&& a)
			{
			std::cout << a << " ";
			});
			std::cout << std::endl;*/
		}

		void check_empty()const
		{
			if(empty())
			{
				throw linq_exception("error!container is empty!");
			}
		}
		//下一步要去实现的函数
		/*
		Aggregate<TSource>(IEnumerable<TSource>, Func<TSource, TSource, TSource>)
		对序列应用累加器函数。
		Aggregate<TSource, TAccumulate>(IEnumerable<TSource>, TAccumulate, Func<TAccumulate, TSource, TAccumulate>)
		对序列应用累加器函数。 将指定的种子值用作累加器初始值。
		Aggregate<TSource, TAccumulate, TResult>(IEnumerable<TSource>, TAccumulate, Func<TAccumulate, TSource, TAccumulate>, Func<TAccumulate, TResult>)
		对序列应用累加器函数。 将指定的种子值用作累加器的初始值，并使用指定的函数选择结果值。

		*/
		template<typename accumulator_type, typename init_type>
		init_type aggregate(const iterator_type& first, const iterator_type& last, const accumulator_type& accumulator, const init_type& init)
		{
			init_type result = init;
			for(auto it = first; it != last; ++it)
			{
				result = accumulator(result, *it);
			}
			return result;
		}

		template< typename accumulator_type, typename init_type>
		decltype(auto) aggregate(const accumulator_type& accumulator, const init_type& init)
		{
			return aggregate(_begin, _end, accumulator, init);
		}
		template< typename accumulator_type>
		decltype(auto) aggregate(const accumulator_type& accumulator)
		{
			auto first = _begin;
			auto result = *first;
			++first;
			for(; first != _end;++first)
			{
				result = accumulator(result, *first);
			}
			return result;
		}
		/*template< typename result_selector_type, typename accumulator_type = std::plus<void>, typename init_type = int>
		decltype(auto) aggregate(const result_selector_type& result_selector, const accumulator_type& accumulator = std::plus<void>(), const init_type& init = init_type())
		{
		return select(result_selector).aggregate(accumulator,init);
		}*/
		ALIAS_FORWARD_FUNCTION(accumulate, aggregate);

		/*
		average
		*/
		value_type average()
		{
			auto n = count();
			return sum() / n;
		}
		//Concat<TSource>	连接两个序列。

		template< typename iterator_type2>
		decltype(auto) concat(const iterator_type2& first, const iterator_type2& last)
		{
			using concat_type = concat_iterator<iterator_type, iterator_type2>;
			return enumerable<concat_type>(concat_type(_begin, _end, first, last), concat_type(_end, _end, last, last));
		}
		template< typename container_type>
		decltype(auto) concat(const container_type& container)
		{
			return concat(container.begin(), container.end());
		}
		/*
			Contains<TSource>(IEnumerable<TSource>, TSource)
			通过使用默认的相等比较器确定序列是否包含指定的元素。
			Contains<TSource>(IEnumerable<TSource>, TSource, IEqualityComparer<TSource>)
			通过使用指定的 IEqualityComparer<T> 确定序列是否包含指定的元素。
			*/
		template<typename value_type, typename compare_type = std::equal_to<void>>
		bool contains(const value_type& value, const compare_type& compare = std::equal_to<void>())
		{
			return any([&value, &compare](auto&& element)
			{
				return compare(element, value);
			});
		}

		/*
		Count<TSource>(IEnumerable<TSource>)
		返回序列中的元素数量。
		Count<TSource>(IEnumerable<TSource>, Func<TSource, Boolean>)
		返回一个数字，表示在指定的序列中满足条件的元素数量。
		*/
		template<typename selector_type = ztl::always_true<value_type>>
		decltype(auto) count(const selector_type& selector = ztl::always_true<value_type>())
		{
			return std::count_if(_begin, _end, selector);
		}

		/*
		Distinct<TSource>(IEnumerable<TSource>)
		通过使用默认的相等比较器对值进行比较返回序列中的非重复元素。
		Distinct<TSource>(IEnumerable<TSource>, IEqualityComparer<TSource>)
		通过使用指定的 IEqualityComparer<T> 对值进行比较返回序列中的非重复元素。
		*/
		template< typename compare_type = std::equal_to<void>>
		decltype(auto) distinct(const compare_type& compare = std::equal_to<void>())
		{
			auto temp_set = std::make_shared<std::unordered_set<value_type>>();
			std::for_each(_begin, _end, [&temp_set](auto&& element)
			{
				if(temp_set->find(element) == temp_set->end())
				{
					temp_set->insert(element);
				}
			});
			return from_shared(temp_set);
		}

		/*

		ElementAt<TSource>	返回序列中指定索引处的元素。

		*/
		value_type element_at(const size_t& index)
		{
			return *std::next(_begin, index);
		}

		bool empty()const
		{
			return _begin == _end;
		}
		/*

		Except<TSource>(IEnumerable<TSource>, IEnumerable<TSource>)
		通过使用默认的相等比较器对值进行比较生成两个序列的差集。
		Except<TSource>(IEnumerable<TSource>, IEnumerable<TSource>, IEqualityComparer<TSource>)
		通过使用指定的 IEqualityComparer<T> 对值进行比较产生两个序列的差集。
		*/
		template < typename iterator_type, typename compare_type = std::less< void >>
		decltype(auto) except(const iterator_type& first, const iterator_type& last, const compare_type& compare = std::less<void>())
		{
			auto result = std::make_shared<vector<value_type>>();
			auto result1 = order_by([](auto&& element)
			{
				return element;
			},order::ascending);
			auto result2 = from(first, last).order_by([](auto&& element)
			{
				return element;
			}, order::ascending);
			std::insert_iterator<vector<value_type>> insert_iter(*result.get(), result->begin());
			std::set_difference(result1.begin(), result1.end(), result2.begin(), result2.end(), insert_iter, compare);
			return from_shared(result);
		}
		/*

		Intersect<TSource>(IEnumerable<TSource>, IEnumerable<TSource>)
		通过使用默认的相等比较器对值进行比较生成两个序列的交集。
		Intersect<TSource>(IEnumerable<TSource>, IEnumerable<TSource>, IEqualityComparer<TSource>)
		通过使用指定的 IEqualityComparer<T> 对值进行比较以生成两个序列的交集。
		*/
		template< typename iterator_type, typename compare_type = std::less<void>>
		decltype(auto) intersect(const iterator_type& first, const iterator_type& last, const compare_type& compare = std::less<void>())
		{
			auto result = std::make_shared<vector<value_type>>();
			auto result1 = order_by([](auto&& element)
			{
				return element;
			}, order::ascending);
			auto result2 = from(first, last).order_by([](auto&& element)
			{
				return element;
			}, order::ascending);
			std::insert_iterator<vector<value_type>> insert_iter(*result.get(), result->begin());
			std::set_intersection(result1.begin(), result1.end(), result2.begin(), result2.end(), insert_iter, compare);
			return from_shared(result);
		}

		/*
		First<TSource>(IEnumerable<TSource>)
		返回序列中的第一个元素。
		First<TSource>(IEnumerable<TSource>, Func<TSource, Boolean>)
		返回序列中满足指定条件的第一个元素。
		last<TSource>(IEnumerable<TSource>)
		返回序列中的最后一个元素。
		last<TSource>(IEnumerable<TSource>, Func<TSource, Boolean>)
		返回序列中满足指定条件的最后一个元素。
		*/

		template<typename selector_type = ztl::always_true<value_type>>
		decltype(auto) first(const selector_type& selector=ztl::always_true<value_type>())
		{
			check_empty();
			for(auto it = _begin; it != _end;++it)
			{
				if (selector(*it))
				{
					return *it;
				}
			}
			throw linq_exception("error!none element could pass selector!");
			return *_begin;
		}

		template<typename selector_type = ztl::always_true<value_type>>
		auto last(const selector_type& selector=ztl::always_true<value_type>())
		{
			check_empty();
			auto it = _begin;
			bool is_null = true;
			value_type result = value_type();
			while(++it != _end)
			{
				auto value = *it;
				if(selector(value))
				{
					result = value;
					is_null = false;
				}
			}
			if(is_null == true)
			{
				throw linq_exception("error!none element could pass selector!");
			}
			return result;
		}

		value_type min_value()
		{
			check_empty();
			return this->aggregate([](auto&& a, auto&&b)
			{
				return a < b ? a : b;
			});
		}

		value_type max_value()
		{
			check_empty();
			return this->aggregate( [](auto&& a, auto&&b)
			{
				return a > b ? a : b;
			});
		}
		/*
		Range	生成指定范围内的整数的序列。
		*/
		template<typename value_type = int>
		decltype(auto) range(const value_type& start, const value_type& count = value_type(), const value_type& diff = 1)
		{
			if(!std::is_integral<value_type>::value)
			{
				throw linq_exception("value_type not integral!");
			}
			auto result = std::make_shared<std::vector<value_type>>();
			auto next = start;
			for(auto i = 0; i < count; ++i;)
			{
				result->push_back(next);
				next += diff;
			}
			return from_shared(result);
		}

		/*
		Reverse
		*/
		auto reverse()->enumerable<storage_iterator<decltype(std::vector<value_type>().begin()), std::shared_ptr<std::vector<value_type>>>>
		{
			auto result = std::make_shared<std::vector<value_type>>(_begin, _end);
			std::reverse(result->begin(), result->end());
			return from_shared(result);
		}
		/*
		Skip<TSource>	跳过序列中指定数量的元素，然后返回剩余的元素。
		SkipWhile<TSource>(IEnumerable<TSource>, Func<TSource, Boolean>)
		只要满足指定的条件，就跳过序列中的元素，然后返回剩余元素。
		*/
		enumerable<skip_iterator<iterator_type, std::function<bool(const value_type&)>>>
			skip(const size_t& count)
		{
				size_t increase = 0;
				std::function<bool(const value_type&)> selector = [increase, count](const value_type& element)mutable
				{
					if(increase < count)
					{
						increase++;
						return true;
					}
					else
					{
						increase++;
						return false;
					}
				};
				return skip_while(selector);
		}
		template< typename selector_type >
		decltype(auto) skip_while(const selector_type& selector)
		{
			using skip_type = skip_iterator<iterator_type, selector_type>;
			return enumerable<skip_type>(skip_type(_begin, _end, selector), skip_type(_end, _end, selector));
		}

		/*
		sum
		*/
		value_type sum()
		{
			check_empty();
			return	aggregate([](auto&& a, auto&&b)
			{
				return  a + b;
			});
		}
		/*
		Skip<TSource>	跳过序列中指定数量的元素，然后返回剩余的元素。
		SkipWhile<TSource>(IEnumerable<TSource>, Func<TSource, Boolean>)
		只要满足指定的条件，就跳过序列中的元素，然后返回剩余元素。
		*/
		enumerable<take_iterator<iterator_type, std::function<bool(const value_type&)>>>
			take(const size_t& count)
		{
				size_t increase = 0;
				std::function<bool(const value_type&)> selector = [increase, count](const value_type& element)mutable
				{
					if(increase < count)
					{
						increase++;
						return true;
					}
					else
					{
						increase++;
						return false;
					}
				};
				using take_type = take_iterator<iterator_type, std::function<bool(const value_type&)>>;
				return take_while(selector);
		}
		template<typename selector_type>
		decltype(auto) take_while(const selector_type& selector)
		{
			using take_type = take_iterator<iterator_type, selector_type>;
			return enumerable<take_type>(take_type(_begin, _begin, selector), take_type(_begin, _end, selector));
		}

		/*

		ThenBy<TSource, TKey>(IOrderedEnumerable<TSource>, Func<TSource, TKey>)
		根据某个键按升序对序列中的元素执行后续排序。
		ThenBy<TSource, TKey>(IOrderedEnumerable<TSource>, Func<TSource, TKey>, IComparer<TKey>)
		使用指定的比较器按升序对序列中的元素执行后续排序。
		ThenByDescending<TSource, TKey>(IOrderedEnumerable<TSource>, Func<TSource, TKey>)
		根据某个键按降序对序列中的元素执行后续排序。
		ThenByDescending<TSource, TKey>(IOrderedEnumerable<TSource>, Func<TSource, TKey>, IComparer<TKey>)
		使用指定的比较器按降序对序列中的元素执行后续排序。

		*/
		decltype(auto) first_by()
		{

		}
		decltype(auto) then_by()
		{
		}

		/*

		Union<TSource>(IEnumerable<TSource>, IEnumerable<TSource>)
		通过使用默认的相等比较器生成两个序列的并集。
		Union<TSource>(IEnumerable<TSource>, IEnumerable<TSource>, IEqualityComparer<TSource>)
		通过使用指定的 IEqualityComparer<T> 生成两个序列的并集。
		*/
		template< typename iterator_type, typename compare_type = std::less<void>>
		decltype(auto) union_with(const iterator_type& first, const iterator_type& last, const compare_type& compare = std::less<void>())
		{
			auto result = std::make_shared<vector<value_type>>();
			auto result1 = order_by([](auto&& element)
			{
				return element;
			}, order::ascending);
			auto result2 = from(first, last).order_by([](auto&& element)
			{
				return element;
			}, order::ascending);
			std::insert_iterator<vector<value_type>> insert_iter(*result.get(), result->begin());
			std::set_union(result1.begin(), result1.end(), result2.begin(), result2.end(), insert_iter, compare);
			return from_shared(result);
		}

		template<typename selector_type>
		decltype(auto) any(const selector_type& selector)
		{
			return std::any_of(_begin, _end, selector);
		}
		template<typename selector_type>
		decltype(auto) all(const selector_type& selector)
		{
			return std::all_of(_begin, _end, selector);
		}
		template<typename selector_type>
		decltype(auto) none(const selector_type& selector)
		{
			return std::none_of(_begin, _end, selector);
		}
		value_type single()
		{
			check_empty();
			auto next = _begin;
			if(++next != _end)
			{
				throw linq_exception("container has element more than one!");
			}
			return *_begin;
		}

		//template<typename selector_type>
		//auto to_vector(const selector_type& selector)
		//	->std::vector<decltype(selector(value_type()))> const
		//{
		//	std::vector<decltype(selector(value_type()))> container;
		//	for(auto it = begin(); it != end(); ++it)
		//	{
		//		container.insert(std::end(container), selector(*it));
		//	}
		//	return std::move(container);
		//}
		//auto to_vector()
		//	->std::vector<value_type> const
		//{
		//	std::vector<value_type> container;
		//	for(auto it = begin(); it != end(); ++it)
		//	{
		//		container.insert(container.end(), *it);
		//	}
		//	return std::move(container);
		//}

#define TO_STL_SEQUENCE_CONTAINER_FUNCTION(STL_SEQUENCE_CONTAINER_NAME) \
	template<typename selector_type>\
	auto to_##STL_SEQUENCE_CONTAINER_NAME(const selector_type& selector)\
	->std::STL_SEQUENCE_CONTAINER_NAME<decltype( selector(value_type()) )> const\
		{\
		std::STL_SEQUENCE_CONTAINER_NAME<decltype( selector(value_type()) )> container;\
		for(auto it = begin(); it != end(); ++it)\
		{\
		container.insert(std::end(container), selector(*it));\
		}\
		return std::move(container);\
		}\
		auto to_##STL_SEQUENCE_CONTAINER_NAME()\
		->std::STL_SEQUENCE_CONTAINER_NAME<value_type> const\
		{\
		std::STL_SEQUENCE_CONTAINER_NAME<value_type> container;\
		for(auto it = begin(); it != end(); ++it)\
		{\
		container.insert(std::end(container), *it);\
		}\
		return std::move(container);\
		}

		TO_STL_SEQUENCE_CONTAINER_FUNCTION(vector);
		TO_STL_SEQUENCE_CONTAINER_FUNCTION(unordered_set);
		TO_STL_SEQUENCE_CONTAINER_FUNCTION(unordered_multiset);
#undef TO_STL_SEQUENCE_CONTAINER_FUNCTION

		template<typename key_selector_type, typename value_selector_type>
		auto to_unordered_map(const key_selector_type& key_selector, const value_selector_type& value_selector)
			->std::unordered_map<decltype(key_selector(value_type())), decltype(value_selector(value_type()))>const
		{
			using key_type = decltype(key_selector(value_type()));
			using val_type = decltype(value_selector(value_type()));
			using container_type = std::unordered_map<key_type, val_type>;
			container_type container;
			for(auto it = begin(); it != end(); ++it)
			{
				container.insert(std::make_pair(key_selector(*it), value_selector(*it)));
			}
			return std::move(container);
		}
	};

	template<typename iterator_type>
	decltype(auto) from(const iterator_type& _begin, const iterator_type& _end)
	{
		return enumerable<iterator_type>(_begin, _end);
	}
	template<typename container_type>
	decltype(auto) from(const container_type& container)
	{
		return from(std::begin(container), std::end(container));
	}
	template<typename container_type>
	decltype(auto) from(const std::shared_ptr<container_type>& container)
	{
		return from(container->begin(), container->end());
	}
	template<typename element_type>
	auto from(const std::initializer_list<element_type>& e)
	{
		return from(e.begin(), e.end());
	}
	template<typename iterator_type, typename container_type>
	enumerable <storage_iterator<iterator_type, container_type >>
		from_shared(const iterator_type& _begin, const iterator_type& _end, const container_type& conatiner)
	{
			return enumerable <
				storage_iterator < iterator_type, container_type >>
				(
				storage_iterator<iterator_type, container_type>(_begin, conatiner),
				storage_iterator<iterator_type, container_type>(_end, conatiner)
				);
	}
	template<typename container_type>
	auto from_shared(const std::shared_ptr<container_type>& container)->decltype(enumerable<storage_iterator<decltype(container->begin()), container_type>>)
	{
		return from_shared(container->begin(), container->end(), container);
	}
	//将单一值包装成序列
	template<typename value_type>
	decltype(auto) from_value(const value_type& value)
	{
		return from(std::vector<value_type>(1, value));
	}
	//template<typename element_type>
	//linq<element_type> from_empty()
	//{
	//	using empty_type = empty_iterator<element_type>;
	//	return linq<element_type>(enumerable<empty_type>(empty_type(), empty_type()));
	//}
}
