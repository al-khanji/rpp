#ifndef RPP_TEST
#define RPP_TEST

#include <cstdio>
#include <string>

namespace rpp {

namespace test {

static bool test_ok;
static int n_failed = 0;

void start_tests() {
    n_failed = 0;
}

template <class T>
void do_test (std::string test_name, T test) {
    test_ok = false;
    test();

    if (!test_ok) {
        n_failed++;
        std::fprintf(stderr, "%s :: FAILURE\n", test_name.c_str());
    }
}

void end_tests() {
    if (n_failed > 0)
        std::fprintf(stderr, "%d TEST FAILURES\n", n_failed);
    std::exit(n_failed);
}

} // namespace test

} // namespace rpp



#endif // RPP_TEST

