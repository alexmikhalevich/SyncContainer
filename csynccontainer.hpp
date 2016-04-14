#include <thread>
#include <mutex>
#include <exception>
#include <condition_variable>

struct empty_container : std::exception {
	const char* what() const throw();
};

template<class Container>
class CSyncContainer {
	private:
		typedef typename Container::value_type T;
		Container  m_container;
		mutable std::mutex m_mutex;
		std::condition_variable m_empty;

		auto Top(T& value, int trash) -> decltype(m_container.top(), void()) {
			value =  m_container.top();
		}
		auto Top(T& value, long trash) -> decltype(m_container.front(), void()) {
			value = m_container.front();
		}
		auto Pop(T& value, int trash) -> decltype(m_container.pop(), void()) {
			Top(value, 0);
			m_container.pop();
		}
		auto Pop(T& value, long trash) -> decltype(m_container.pop_front(), void()) {
			Top(value, 0);
			m_container.pop_front();
		}
		auto Push(T value, int trash) -> decltype(m_container.push(), void()) {
			m_container.push(value);
		}
		auto Push(T value, long trash) -> decltype(m_container.push_front(), void()) {
			m_container.push_front(value);
		}
	public:
		CSyncContainer() {}
		CSyncContainer(const CSyncContainer& other) {
			std::unique_lock<std::mutex> lock(other.m_mutex);
			m_container = other.m_container;
		}
		CSyncContainer& operator=(const CSyncContainer&) = delete;

		void push(T value) {
			std::unique_lock<std::mutex> lock(m_mutex);
			Push(value, 0);
		}
		
		void popNoSleep(T& value) {
			std::unique_lock<std::mutex> lock(m_mutex);
			if(m_container.empty()) throw empty_container();
			value = m_container.
			Pop(value, 0);
		}

		void popOrSleep(T& value) {
			std::unique_lock<std::mutex> lock(m_mutex);
			if(m_container.empty()) m_empty.wait(lock);
			Pop(value, 0);
		}

		bool empty() const {
			std::unique_lock<std::mutex> lock(m_mutex);
			return m_container.empty();
		}
};
