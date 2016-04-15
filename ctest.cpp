#include "ctest.h"

template<class Container>
void pop(CSyncContainer<Container>& container, std::vector<short>& check) {
	int value = 0;
	for(int i = 0; i < ITERATIONS; ++i) {
		value = container.popOrSleep();
		EXPECT_EQ(check[value], 0);
		check[value] = 1;
	}
}

template<class Container>
void push(CSyncContainer<Container>& container, int begin) {
	for(int i = begin; i < ITERATIONS + begin; ++i)
		container.push(i);	
}

typedef ::testing::Types<std::vector<int>, std::deque<int>, std::stack<int>, std::queue<int>, std::list<int>> Types;
TYPED_TEST_CASE(CTest, Types);

TYPED_TEST(CTest, MainTest) {
	std::vector<std::thread> threads;
	for(int i = 0; i < THREADS; ++i)
		threads.emplace_back(push<TypeParam>, std::ref(this->m_container), ITERATIONS * i);
	for(int i = 0; i < THREADS; ++i)
		threads.emplace_back(pop<TypeParam>, std::ref(this->m_container), std::ref(this->m_check));
	for(size_t i = 0; i < threads.size(); ++i)
		threads[i].join();

	for(size_t i = 0; i < this->m_check.size(); ++i)
		EXPECT_EQ(this->m_check.at(i), 1);
	EXPECT_TRUE(this->m_container.empty());
}
