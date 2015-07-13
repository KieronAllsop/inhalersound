#define BOOST_TEST_MODULE Subtract
#include <boost/test/included/unit_test.hpp>

int subtract(int num1, int num2)
{
    return num1 - num2;
}

BOOST_AUTO_TEST_CASE(subtractTest)
{
    BOOST_CHECK(subtract(3,3) == 0);
}