#pragma once
#include <functional>
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "check_type.hpp"
namespace ztl
{
#define COMMON_ITERATOR_DECLARE(SELF_TYPE,ITEATOR_DIFFERENCE_TYPE) \
	using self_type = SELF_TYPE;\
	using value_type		= decltype( *std::declval<self_type>() );\
	using pointer			= value_type*;\
	using reference			= value_type&;\
	using difference_type	=typename std::iterator_traits<ITEATOR_DIFFERENCE_TYPE>::difference_type;\
	using iterator_category = std::forward_iterator_tag;
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

#undef STRUCT_NAME
	
#define RETURN_VALUE_TYPE(RETURN_VALUE)\
	typename std::remove_reference<decltype(RETURN_VALUE)>::type


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
			auto f1 = *first1;
			auto f2 = *first2;
			if(*first1 != *first2)
			{
				return false;
			}
			++first1;
			++first2;
		}
		return first1 == last1 && first2 == last2;
	}

	//iterator

	//where
	template<typename iterator_type, typename predicate_type>
	class where_iterator
	{
	private:
		iterator_type iterator;
		iterator_type end;
		predicate_type pred;
	public:
		auto operator*( )->RETURN_VALUE_TYPE(*iterator) const
		{
			return *iterator;
		}
	public:
		COMMON_ITERATOR_DECLARE(where_iterator,iterator_type);
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
		
		self_type& operator++( )
		{
			move_iterator(true);
			return *this;
		}

		self_type operator++( int )
		{
			self_type temp = *this;
			move_iterator(true);
			return temp;
		}

	public:
		bool operator==( const self_type& right ) const
		{
			return iterator == right.iterator;
		}
		bool operator!=( const self_type& right ) const
		{
			return !( *this== right  );
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
		auto operator*( )->
			RETURN_VALUE_TYPE(selector(*iterator)) const
		{
			return selector(*iterator);
		}
		COMMON_ITERATOR_DECLARE(select_iterator,iterator_type);
	public:
		select_iterator() = delete;
		select_iterator(const iterator_type& _iterator, const selector_type& _selector) :iterator(_iterator), selector(_selector)
		{

		}

	public:
		
		self_type& operator++( )
		{
			++iterator;
			return *this;
		}

		self_type operator++( int )
		{
			self_type temp = *this;
			++iterator;
			return temp;
		}

	public:
		bool operator==( const self_type& right )const
		{
			return iterator == right.iterator;
		}
		bool operator!=( const self_type& right )const
		{
			return !( operator==( right ) );
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
		
		auto operator*( )
			->RETURN_VALUE_TYPE(result_selector(*iterator, 
			*std::next(collection_selector(*iterator).begin(), subindex))) const
		{
			return result_selector(*iterator, *std::next(collection_selector(*iterator).begin(), subindex));
		}
		//这个方法好啊!!!泪奔┭┮﹏┭┮;
		COMMON_ITERATOR_DECLARE(select_many_iterator,iterator_type);
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
		self_type& operator++( )
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

		self_type operator++( int )
		{
			self_type temp = *this;
			++iterator;
			return temp;
		}

	public:
		bool operator==( const self_type& right ) const
		{
			return iterator == right.iterator;
		}
		bool operator!=( const self_type& right ) const
		{
			return !( operator==( right ) );
		}
	private:
		void move_iterator()
		{
			if(iterator != end)
			{
				while(iterator != end && ( collection_selector(*iterator) ).size() == 0)
				{
					++iterator;
				}
				current_size = ( collection_selector(*iterator) ).size();
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
		auto operator*( )
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
		

		self_type& operator++( )
		{
			if(first1 != last1 && first2 != last2)
			{
				++first1;
				++first2;
			}
			return *this;
		}

		self_type operator++( int )
		{
			self_type temp = *this;
			++iterator;
			return temp;
		}

	public:
		bool operator==( const self_type& right )const
		{
			return first1 == right.first1
				&& last1 == right.last1;
		}
		bool operator!=( const self_type& right )const
		{
			return !( operator==( right ) );
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
		
		auto operator*( )->RETURN_VALUE_TYPE(
			result_selector(
			*first1, std::declval<std::vector<typename 
			std::iterator_traits<inner_iterator_type>::value_type
			>>()) ) const
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


		self_type& operator++( )
		{
			++first1;
			return *this;
		}

		self_type operator++( int )
		{
			self_type temp = *this;
			++(*this);
			return temp;
		}
		bool operator==( const self_type& right )const
		{
			return first1 == right.first1&& last1 == right.last1;
		}
		bool operator!=( const self_type& right )const
		{
			return !( operator==( right ) );
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
		auto operator*( )->RETURN_VALUE_TYPE(*iterator) const
		{
			return *iterator;
		}
	public:
		COMMON_ITERATOR_DECLARE(storage_iterator,iterator_type);
	public:
		storage_iterator() = delete;
		storage_iterator(const iterator_type& _iterator, const std::shared_ptr<container_type>& _container)
			:iterator(_iterator), shared_container(_container)
		{
		}
	public:
		
		self_type& operator++( )
		{
			++iterator;
			return *this;
		}

		self_type operator++( int )
		{
			self_type temp = *this;
			++iterator;
			return temp;
		}

	public:
		bool operator==( const self_type& right )const
		{
			return iterator == right.iterator;
		}
		bool operator!=( const self_type& right )const
		{
			return !( operator==( right ) );
		}
	};
	
	enum class order :bool
	{
		ascending,
		descending,
	};
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
		decltype( auto ) where(const predicate_type& pred)
		{
			return enumerable<where_iterator<iterator_type, predicate_type>>
				(
				where_iterator<iterator_type, predicate_type>(_begin, _end, pred),
				where_iterator<iterator_type, predicate_type>(_end, _end, pred)
				);
		}

		template<typename predicate_type>
		decltype( auto ) copy(predicate_type&& pred)
		{
			return where(std::forward<predicate_type>(pred));
		}

		template<typename predicate_type>
		decltype( auto ) remove(predicate_type&& pred)
		{
			using negate_predicate = ztl::unary_negate<typename std::remove_reference<predicate_type>::type>;
			return where(negate_predicate(std::forward<negate_predicate>(pred)));
		}

		template<typename selector_type>
		decltype( auto ) select(const selector_type&result_selector)
		{
			return enumerable<select_iterator<iterator_type, selector_type>>(
				select_iterator<iterator_type, selector_type>(_begin, result_selector),
				select_iterator<iterator_type, selector_type>
				(_end, result_selector));
		}

		template<typename collection_selector_type, typename result_selector_type>
		decltype( auto ) select_many(const collection_selector_type& collection_selector, const result_selector_type& result_selector)
		{
			using select_many_type = select_many_iterator<typename std::decay<decltype( begin() )>::type, collection_selector_type, result_selector_type>;

			return enumerable<select_many_type>(
				select_many_type(begin(), end(), collection_selector, result_selector),
				select_many_type(end(), end(), collection_selector, result_selector));
		}

		template<typename predicate_type>
		decltype( auto ) order_by(const predicate_type& key_selector, order cending=order::ascending)
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
			decltype( auto ) outer_group_join(
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
			decltype( auto ) inner_group_join(
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
				return outer_group_join(inner_begin,inner_end,out_key_selector,inner_key_selector,selector)
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
			decltype( auto ) inner_join(
			const inner_iterator_type& inner_begin,
			const inner_iterator_type& inner_end,
			const outer_key_selector_type& out_key_selector,
			const inner_key_selector_type& inner_key_selector,
			const result_selector_type& result_selector)
		{
				return	inner_group_join( inner_begin, inner_end, out_key_selector, inner_key_selector, [](auto&& outer_element, auto&& inner_list)
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

		template<typename inner_iterator_type, typename outer_key_selector_type,
			typename inner_key_selector_type, typename result_selector_type>
			decltype( auto ) outer_join(
			const inner_iterator_type& inner_begin,
			const inner_iterator_type& inner_end,
			const outer_key_selector_type& out_key_selector,
			const inner_key_selector_type& inner_key_selector,
			const result_selector_type& reuslt_selector)
		{
			return	outer_group_join(inner_begin, inner_end, out_key_selector, inner_key_selector, [](auto&& outer_element,auto&& inner_list)
				{
					return std::make_pair(outer_element, inner_list);
				}).select_many([&result_selector](auto&& pair_element)
				{
					return pair_element.second;
				}, [&result_selector](auto&& pair_element,auto&& element)
				{
					return result_selector(pair_element.first, element);
				});
		}

		template<typename key_selector_type>
		decltype( auto ) group_by(const key_selector_type& key_selector)
		{
			using key_type = std::remove_reference<decltype( key_selector(value_type()) )>::type;
			using element_type = value_type;
			auto hash_map = std::make_shared<std::unordered_map<key_type, std::vector<element_type>>>();
			for(auto it = _begin; it != _end; ++it)
			{
				auto value = *_begin;
				auto key = key_selector(value);
				auto& find_iter = hash_map->find(key);
				if(find_iter == hash_map->end())
				{
					hash_map->insert(std::make_pair(key, std::vector<element_type>(1, value)));
				}
				else
				{
					hash_map[key].emplace_back(value);
				}
			}
			return from_shared(hash_map->begin(), hash_map->end(), hash_map);
		}

		template<typename key_selector_type, typename element_selector_type>
		decltype( auto ) group_by(const key_selector_type& key_selector, const element_selector_type& element_selector)
		{
			
			return group_by(key_selector).select([&element_selector](auto&& enumerable_list)
			{
				using element_type = decltype( element_selector(*std::begin(enumerable_list.second)));
				using key_type = decltype( enumerable_list.first );
				std::vector< element_type> des_list;
				for(auto it = std::begin(enumerable_list.second), it != std::end(enumerable_list.second); ++it)
				{
					des_list.push_back(element_selector(*it));
				}
				return make_pair(enumerable_list.first, std::move(des_list));
			});
		}
		template<typename key_selector_type, typename result_selector_type,typename element_selector_type>
		decltype( auto ) group_by(const key_selector_type& key_selector, const element_selector_type& element_selector,const result_selector_type& result_selector)
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
		decltype( auto ) equal(const iterator_type& t_begin, const iterator_type& t_end)const
		{
			return ztl::equal(_begin, _end, t_begin, t_end);
		}
		template<typename container_type>
		decltype( auto ) equal(container_type&& container)const
		{
			return equal(std::begin(std::forward<container_type>(container)), std::end(std::forward<container_type>(container)));
		}
		template<typename element_type>
		decltype( auto ) equal(const std::initializer_list<element_type>& e)const
		{
			return equal(std::begin(e), std::end(e));
		}
		void print_pair()
		{
			for(; _begin != _end; ++_begin)
			{
				auto t = *_begin;

				std::cout << t.first << " " << t.second<<" " << std::endl;
			}
		}
		void print()
		{
			for(; _begin != _end; ++_begin)
			{
				auto t = *_begin;
				
				std::cout << t<<" "<< " "<<std::endl;
			}
			/*std::for_each(_begin, _end, [](auto&& a)
			{
			std::cout << a << " ";
			});
			std::cout << std::endl;*/
		}

		//模板的模板参数
		template<typename container_type>
		container_type transform() const
		{
			container_type container;
			for(auto it = begin(); it != end();++it)
			{
				container.insert(std::end(container), *it);
			}
			return std::move(container);
		}
		//decltype( auto ) to_vector()
		//{
		//	return transform<std::vector<value_type>>();
		//}
		/*std::vector<value_type> to_vector()
		{
		return transform<std::vector<value_type>>();
		}*/

#define TO_CONTAINER_FUNCTION(STL_CONTAINER_NAME) \
		std::STL_CONTAINER_NAME<value_type> to_##STL_CONTAINER_NAME()\
		{\
		return transform<std::STL_CONTAINER_NAME<value_type>>();\
		}

		TO_CONTAINER_FUNCTION(vector);
		/*TO_CONTAINER_FUNCTION(unordered_map);
		TO_CONTAINER_FUNCTION(unordered_multimap);
		TO_CONTAINER_FUNCTION(unordered_set);
		TO_CONTAINER_FUNCTION(unordered_multiset);*/

#undef TO_CONTAINER_FUNCTION
	};

	template<typename iterator_type>
	decltype( auto ) from(const iterator_type& _begin, const iterator_type& _end)
	{
		return enumerable<iterator_type>(_begin, _end);
	}
	template<typename container_type>
	decltype( auto ) from(const container_type& container)
	{
		return from(std::begin(container), std::end(container));
	}
	template<typename container_type>
	decltype( auto ) from(const std::shared_ptr<container_type>& container)
	{
		return from(container->begin(), container->end());
	}
	template<typename element_type>
	auto from(const std::initializer_list<element_type>& e)
	{
		return from(e.begin(), e.end());
	}
	template<typename iterator_type, typename container_type>
	decltype( auto ) from_shared(const iterator_type& _begin, const iterator_type& _end, const container_type& conatiner)
	{
		return enumerable <
			storage_iterator < iterator_type, container_type >>
			(
			storage_iterator<iterator_type, container_type>(_begin, conatiner),
			storage_iterator<iterator_type, container_type>(_end, conatiner)
			);
	}
	template<typename container_type>
	decltype( auto ) from_shared(const std::shared_ptr<container_type>& container)
	{
		return from_shared(container->begin(), container->end(), container);
	}
}
