#define BOOST_TEST_MODULE Testsuite
#include <boost/test/unit_test.hpp>
#include "Gameboard.cpp"
#define ON 1


BOOST_AUTO_TEST_CASE(simple_test) {
  Gameboard * testgame = new Gameboard();
  testgame->toggleBit(Point(1, 2), ON);
}

