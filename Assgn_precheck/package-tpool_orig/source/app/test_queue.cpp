#include <ra/queue.hpp>
#include <type_traits>
#include <vector>

template <class T>
void test()
{
	using queue_type = ra::concurrency::queue<T>;
	static_assert(std::is_same_v<typename queue_type::value_type, T>);
	static_assert(std::is_same_v<typename queue_type::size_type, std::size_t>);
	static_assert(std::is_enum_v<typename queue_type::status>);
	static_assert(static_cast<int>(queue_type::status::success) == 0);
	static_assert(static_cast<int>(queue_type::status::empty) == 1);
	static_assert(static_cast<int>(queue_type::status::full) == 2);
	static_assert(static_cast<int>(queue_type::status::closed) == 3);
	static_assert(!std::is_default_constructible_v<queue_type>);
	static_assert(!std::is_copy_constructible_v<queue_type>);
	static_assert(!std::is_move_constructible_v<queue_type>);
	static_assert(!std::is_move_assignable_v<queue_type>);
	static_assert(!std::is_copy_assignable_v<queue_type>);

	static_assert(std::is_same_v<decltype(
	  std::declval<queue_type>().push(std::declval<T>())),
	  typename queue_type::status>);
	static_assert(std::is_same_v<decltype(std::declval<queue_type>().pop(
	  *static_cast<T*>(nullptr))), typename queue_type::status>);
	static_assert(std::is_same_v<decltype(std::declval<queue_type>().close()),
	  void>);
	static_assert(std::is_same_v<decltype(std::declval<queue_type>().clear()),
	  void>);
	static_assert(std::is_same_v<decltype(std::declval<queue_type>().is_full()),
	  bool>);
	static_assert(std::is_same_v<decltype(std::declval<queue_type>().is_empty()),
	  bool>);
	static_assert(std::is_same_v<decltype(std::declval<queue_type>().is_empty()),
	  bool>);
	static_assert(std::is_same_v<
	  decltype(std::declval<queue_type>().max_size()),
	  typename queue_type::size_type>);

	{
		T e;
		queue_type q(10);
		q.push(std::move(e));
		q.pop(e);
		q.close();
		q.clear();
		q.is_full();
		q.is_empty();
		q.is_closed();
		q.max_size();
		typename queue_type::status stat = queue_type::status::success;
		stat = queue_type::status::closed;
	}

	{
    	namespace rc = ra::concurrency;
	    rc::queue<int>::status s;
	}

}

int main()
{
	test<int>();
	test<std::vector<double>>();
//	auto f= [](){};
//	test<decltype(f)>();
}
