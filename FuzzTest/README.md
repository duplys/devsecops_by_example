# FuzzTest

[Google's FuzzTest](https://github.com/google/fuzztest) is a C++ testing framework for writing and executing so-called *fuzz tests*, property-based tests executed using [coverage-guided fuzzing](https://en.wikipedia.org/wiki/Fuzzing) under the hood. 

Google describes fuzz tests being like regular unit tests, but more generic and more powerful. While unit tests codify a "for this specific input, we expect this specific output" behavior, fuzz tests implement a "for these types of input, we expect this generic property to be true" behavior. As an example:

```C++
void MyApiAlwaysSucceedsOnPositiveIntegers(int i) {
    bool success = MyApi(i);
    EXPECT_TRUE(success);
}

FUZZ_TEST(MyApiTest, MyApiAlwaysSucceedsOnPositiveIntegers).WithDomains(/*i:*/fuzztest::Positive<int>());
```

Fuzz testing is a proven testing technique that has found [tens of thousands of bugs](https://github.com/google/oss-fuzz#trophies). Since fuzz tests are used to test for generic properties, they are much more powerful than regular unit tests. More precisely, fuzz tests automatically uncover edge cases that trigger dormant bugs and security vulnerabilities.

## Installation
Probably the fastest way to try out this example is to build a Docker image using `Dockerfile` in `Docker` directory:
```console
$ cd Docker
$ docker build . -t fuzztest
```

Alternatively, if you want to install [Bazel](https://bazel.build/install/ubuntu) and [Clang](https://clang.llvm.org) natively on your machine:
```console
$ apt install apt-transport-https curl gnupg -y
$ curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor >bazel-archive-keyring.gpg
$ sudo mv bazel-archive-keyring.gpg /usr/share/keyrings
$ echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
$ sudo apt update && sudo apt install bazel -y
$ sudo apt install clang -y
```

## Vulnerable application example
The directory `vuln-app` containing the vulnerable source code is prepared according to the [FuzzTest repository instructions](https://github.com/google/fuzztest/blob/main/doc/quickstart-bazel.md). Hence, it already includes the files:
* `WORKSPACE` to define a Bazel workspace,
* Bazel configuration file `.bazelrc`, and
* `BUILD` file to build and run the fuzz tests.

The vulnerable C++ source code is in file1.cpp:

```C++
#include "fuzztest/fuzztest.h"
#include "gtest/gtest.h"

TEST(MyTestSuite, OnePlustTwoIsTwoPlusOne) {
  EXPECT_EQ(1 + 2, 2 + 1);
}

void IntegerAdditionCommutes(int a, int b) {
  EXPECT_EQ(a + b, b + a);
}

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
```

## Running FuzzTest
If you're using the above Docker image, first do:
```console
$ cd vuln-app
$ vuln-app % docker container run -v </absolute/path/to/vuln-app/directory/on/your/host/machine>:/opt/fuzztest --rm -it fuzztest
root@19d16692a403:/opt/fuzztest# 
```

To run FuzzTest, we can follow [Google's instructions](https://github.com/google/fuzztest/blob/main/doc/quickstart-bazel.md):
```console
root@19d16692a403:/opt/fuzztest# bazel run @com_google_fuzztest//bazel:setup_configs > fuzztest.bazelrc
Extracting Bazel installation...
Starting local Bazel server and connecting to it...

-- snip --

INFO: Build completed successfully, 4 total actions
INFO: Running command line: bazel-bin/external/com_google_fuzztest/bazel/setup_configs @com_google_fuzztest
```

With the Bazel workspace set up, we can now start using FuzzTest. There are two ways to run the tests:
* *unit test mode*: runs any unit tests and fuzz tests for a short time without sanitizer and coverage instrumentation
* *fuzzing mode*: runs a single specified fuzz test, with sanitizer and coverage instrumentation, using different input values until it finds a crash or it is explicitly terminated by the user

Unit test mode:
```console
root@19d16692a403:/opt/fuzztest# bazel test :first_fuzz_test
```

Fuzzing mode:
```console
root@19d16692a403:/opt/fuzztest# bash bazel run --config=fuzztest :first_fuzz_test -- --fuzz=MyTestSuite.MyArrayIsAlwaysWrittenSuccessfully
```


## Example terminal output
If your setup is correct, you should see something like this for the unit test mode:
```console
================================================================
root@19d16692a403:/opt/fuzztest# bazel test :first_fuzz_test
WARNING: Streamed test output requested. All tests will be run locally, without sharding, one at a time
INFO: Build options --copt, --dynamic_mode, --linkopt, and 1 more have changed, discarding analysis cache.
INFO: Analyzed target //:first_fuzz_test (0 packages loaded, 919 targets configured).
INFO: Found 1 test target...
[16 / 103] 6 actions running
    Compiling fuzztest/fuzztest.cc; 3s processwrapper-sandbox
    Compiling googletest/src/gtest-matchers.cc; 1s processwrapper-sandbox
    Compiling googletest/src/gtest-port.cc; 0s processwrapper-sandbox
    Compiling googletest/src/gtest-death-test.cc; 0s processwrapper-sandbox
    Compiling googletest/src/gtest-typed-test.cc; 0s processwr

-- snip --

=================================================================
=== BUG FOUND!

file1.cpp:27: Counterexample found for MyTestSuite.MyArrayIsAlwaysWrittenSuccessfully.
The test fails with input:
argument 0: 2147483647

=================================================================
=== Reproducer test

TEST(MyTestSuite, MyArrayIsAlwaysWrittenSuccessfullyRegression) {
  MyArrayIsAlwaysWrittenSuccessfully(
    2147483647
  );
}

=================================================================

-- snip --
```


You should see something like this for the fuzzing mode:
```console
root@19d16692a403:/opt/fuzztest# bash bazel run --config=fuzztest :first_fuzz_test -- --fuzz=MyTestSuite.MyArrayIsAlwaysWrittenSuccessfully
INFO: Build options --copt, --dynamic_mode, --linkopt, and 1 more have changed, discarding analysis cache.
INFO: Analyzed target //:first_fuzz_test (0 packages loaded, 919 targets configured).
INFO: Found 1 target...
[218 / 282] 6 actions, 5 running

-- snip --

INFO: Build completed successfully, 3 total actions
INFO: Running command line: external/bazel_tools/tools/test/test-setup.sh ./first_fuzz_test '--fuzz=MyTestSuite.MyArrayIsAlwaysWrittenSuccessfully'
exec ${PAGER:-/usr/bin/less} "$0" || exit 1
Executing tests from //:first_fuzz_test
-----------------------------------------------------------------------------
[.] Sanitizer coverage enabled. Counter map size: 21182, Cmp map size: 262144
Note: Google Test filter = MyTestSuite.MyArrayIsAlwaysWrittenSuccessfully
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from MyTestSuite
[ RUN      ] MyTestSuite.MyArrayIsAlwaysWrittenSuccessfully
[*] Corpus size:     1 | Edges covered:    139 | Fuzzing time:    803.495us | Total runs:  1.00e+00 | Runs/secs:     1

=================================================================
=== Fuzzing stats

Elapsed seconds (ns): 1142403
Total runs: 2
Edges covered: 139
Total edges: 21182
Corpus size: 1

=================================================================
=== BUG FOUND!

file1.cpp:27: Counterexample found for MyTestSuite.MyArrayIsAlwaysWrittenSuccessfully.
The test fails with input:
argument 0: 375135700

=================================================================
=== Reproducer test

TEST(MyTestSuite, MyArrayIsAlwaysWrittenSuccessfullyRegression) {
  MyArrayIsAlwaysWrittenSuccessfully(
    375135700
  );
}

-- snip --
```