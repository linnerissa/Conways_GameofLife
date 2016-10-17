#define BOOST_TEST_MODULE Testsuite
#define BOOST_AUTO_TEST_MAIN
#include <stdio.h>
#include "/usr/include/gtest/gtest.h"
#include "Gameboard.cpp"
#define ON 1

using namespace testing;

TEST(Init_Board, Multi) {
	std::unordered_set<Point> beginSet;
	beginSet.emplace(Point(1,2));
	beginSet.emplace(Point(2,2));
	beginSet.emplace(Point(3,2));
	Gameboard * testgame = new Gameboard(beginSet);
	testgame->nextState();

	std::unordered_set<Point> currState = testgame->getCurrentlyOn();
	for (const Point& point: beginSet){
		ASSERT_TRUE(currState.find(point) != currState.end());
	}

	ASSERT_TRUE(currState.size() == beginSet.size());
}

TEST(Init_Board, Single) {
	std::unordered_set<Point> beginSet;
	beginSet.emplace(Point(1,2));
	Gameboard * testgame = new Gameboard(beginSet);
	testgame->nextState();

	std::unordered_set<Point> currState = testgame->getCurrentlyOn();
	for (const Point& point: beginSet){
		ASSERT_TRUE(currState.find(point) != currState.end());
	}

	ASSERT_TRUE(currState.size() == beginSet.size());
}

TEST(Second_Stage, Single) {
	std::unordered_set<Point> beginSet;
	beginSet.emplace(Point(1,2));
	Gameboard * testgame = new Gameboard(beginSet);
	testgame->nextState();

	std::unordered_set<Point> currState = testgame->getCurrentlyOn();
	for (const Point& point: beginSet){
		ASSERT_TRUE(currState.find(point) != currState.end());
	}

	ASSERT_TRUE(currState.size() == beginSet.size());

	testgame->nextState();
	ASSERT_TRUE(currState.size() == 0);
}

	
GTEST_API_ int main(int argc, char *argv[]){
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}



