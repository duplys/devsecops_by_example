#include "fuzztest/fuzztest.h"
#include "gtest/gtest.h"

TEST(MyTestSuite, OnePlustTwoIsTwoPlusOne) {
  EXPECT_EQ(1 + 2, 2 + 1);
}

void IntegerAdditionCommutes(int a, int b) {
  EXPECT_EQ(a + b, b + a);
}
//FUZZ_TEST(MyTestSuite, IntegerAdditionCommutes);

int WriteAnArray(int i) {
    char a[10];
    a[i] = 0;
    if (a[i] == 0) {
        return true;
    }
    return false;
}

void MyArrayIsAlwaysWrittenSuccessfully(int i) {
    bool success = WriteAnArray(i);
    EXPECT_TRUE(success);
}

FUZZ_TEST(MyTestSuite, MyArrayIsAlwaysWrittenSuccessfully).WithDomains(/*i:*/fuzztest::Positive<int>());