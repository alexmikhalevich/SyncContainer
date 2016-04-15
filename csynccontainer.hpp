#include <thread>
#include <mutex>
#include <exception>
#include <condition_variable>
#include <iostream>

class CException
{
	public:
		virtual void PrintError() const = 0;
};

class CExEmptyContainer : public virtual CException
{
	public:
		virtual void PrintError() const {
			std::cerr << "Empty container" << std::endl;
		}
};

template<class Container>
auto Top(Container& container, int trash) -> decltype(container.top(), typename Container::value_type()) {
	return container.top();
}
template<class Container>
auto Top(Container& container, long trash) -> decltype(container.pop_back(), typename Container::value_type()) {
	return container.back();
}
template<class Container>
auto Top(Container& container, long long trash) -> decltype(container.pop(), typename Container::value_type()) {
	return container.front();
}
template<class Container>
auto Pop(Container& container, int trash) -> decltype(container.pop(), typename Container::value_type()) {
	typename Container::value_type res = Top(container, 0);
	container.pop();
	return res;
}
template<class Container>
auto Pop(Container& container, long trash) -> decltype(container.pop_back(), typename Container::value_type()) {
	typename Container::value_type res = Top(container, 0);
	container.pop_back();
	return res;
}
template<class Container>
auto Push(Container& container, typename Container::value_type value, long trash) -> decltype(container.pop_back(), void()) {
	container.push_back(value);
}
template<class Container>
auto Push(Container& container, typename Container::value_type value, int trash) -> decltype(container.pop(), void()) {
	container.push(value);
}

template<class Container>
class CSyncContainer {
	private:
		typedef typename Container::value_type T;
		Container  m_container;
		mutable std::mutex m_mutex;
		std::condition_variable m_empty;
	public:
		CSyncContainer() {}
		CSyncContainer(Container& container) : m_container(container) {}
		CSyncContainer(const CSyncContainer& other) {
			std::unique_lock<std::mutex> lock(other.m_mutex);
			m_container = other.m_container;
		}
		CSyncContainer& operator=(const CSyncContainer&) = delete;

		void push(T value) {
			std::unique_lock<std::mutex> lock(m_mutex);
			Push(m_container, value, 0);
			m_empty.notify_one();
		}
		
		T popNoSleep() {
			std::unique_lock<std::mutex> lock(m_mutex);
			if(m_container.empty()) throw CExEmptyContainer();
			T res = Pop(m_container, 0);
			return res;
		}

		T popOrSleep() {
			std::unique_lock<std::mutex> lock(m_mutex);
			while(m_container.empty()) m_empty.wait(lock);
			T res = Pop(m_container, 0);
			return res;
		}

		bool empty() const {
			std::unique_lock<std::mutex> lock(m_mutex);
			return m_container.empty();
		}
};
