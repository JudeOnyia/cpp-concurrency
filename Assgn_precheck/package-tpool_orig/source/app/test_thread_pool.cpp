#include <ra/thread_pool.hpp>
#include <type_traits>

void test()
{
	using ra::concurrency::thread_pool;
	static_assert(std::is_same_v<typename thread_pool::size_type,
	  std::size_t>);

	static_assert(std::is_default_constructible_v<thread_pool>);
	static_assert(!std::is_copy_constructible_v<thread_pool>);
	static_assert(!std::is_move_constructible_v<thread_pool>);
	static_assert(!std::is_move_assignable_v<thread_pool>);
	static_assert(!std::is_copy_assignable_v<thread_pool>);

	static_assert(std::is_same_v<decltype(std::declval<thread_pool>().size()),
	  typename thread_pool::size_type>);
	static_assert(std::is_same_v<decltype(
	  std::declval<thread_pool>().schedule(std::function<void()>())), void>);
	static_assert(
	  std::is_same_v<decltype(std::declval<thread_pool>().shutdown()), void>);
	static_assert(std::is_same_v<
	  decltype(std::declval<thread_pool>().is_shutdown()), bool>);

	{
		thread_pool tp;
		thread_pool tp1(0);
		tp.size();
		tp.schedule(std::function<void()>([](){}));
		tp.shutdown();
		tp.is_shutdown();
	}

}

int main()
{
	test();
}
