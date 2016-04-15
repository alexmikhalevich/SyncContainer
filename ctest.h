#include <vector>
#include <deque>
#include <stack>
#include <queue>
#include <list>
#include <thread>
#include <functional>
#include "csynccontainer.hpp"
#include "gtest/gtest.h"

const int THREADS = std::thread::hardware_concurrency();
const int ITERATIONS = 1000;
		
template<class T>
class CTest : public ::testing::Test {
	public:
		CTest() {
			for(size_t i = 0; i < THREADS * ITERATIONS; ++i)
				m_check.push_back(0);
		}

		std::vector<short> m_check;
		CSyncContainer<T> m_container;
};
