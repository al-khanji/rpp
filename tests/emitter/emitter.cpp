#include <rpp_test.hpp>
#include <rpp_emitter.hpp>

using namespace rpp::test;

void fn_v_v()            { test_ok = true; }
void fn_v_i(int)         { test_ok = true; }
void fn_v_s(std::string) { test_ok = true; }

auto l_v_v = []               { test_ok = true; };
auto l_v_i = [] (int)         { test_ok = true; };
auto l_v_s = [] (std::string) { test_ok = true; };

struct functor {
    template <class... Args>
    void operator()(Args...) {
        test_ok = true;
    }
};

int n_calls = 0;
void increment_n_calls()
{
    n_calls++;
}

int main()
{
    start_tests();

    do_test("em_v -> fn_v_v", [] {
        rpp::emitter<> em;
        em.connect(fn_v_v);
        em.fire();
    });

    do_test("em_i -> fn_v_v", [] {
        rpp::emitter<int> em;
        em.connect(fn_v_v);
        em.fire(0);
    });

    do_test("em_i -> fn_v_i", [] {
        rpp::emitter<int> em;
        em.connect(fn_v_i);
        em.fire(0);
    });

    do_test("em_s -> fn_v_v", [] {
        rpp::emitter<std::string> em;
        em.connect(fn_v_v);
        em.fire("Hello World");
    });

    do_test("em_s -> fn_v_s", [] {
        rpp::emitter<std::string> em;
        em.connect(fn_v_s);
        em.fire("Hello World");
    });

    do_test("em_v -> l_v_v", [] {
        rpp::emitter<> em;
        em.connect(l_v_v);
        em.fire();
    });

    do_test("em_i -> l_v_v", [] {
        rpp::emitter<int> em;
        em.connect(l_v_v);
        em.fire(0);
    });

    do_test("em_i -> l_v_i", [] {
        rpp::emitter<int> em;
        em.connect(l_v_i);
        em.fire(0);
    });

    do_test("em_s -> l_v_v", [] {
        rpp::emitter<std::string> em;
        em.connect(l_v_v);
        em.fire("Hello World");
    });

    do_test("em_s -> l_v_s", [] {
        rpp::emitter<std::string> em;
        em.connect(l_v_s);
        em.fire("Hello World");
    });

    do_test("em_v -> functor {}", [] {
        rpp::emitter<> em;
        em.connect(functor {});
        em.fire();
    });

    do_test("em_i -> functor {}", [] {
        rpp::emitter<int> em;
        em.connect(functor {});
        em.fire(0);
    });

    do_test("em_i -> functor {}", [] {
        rpp::emitter<int> em;
        em.connect(functor {});
        em.fire(0);
    });

    do_test("em_s -> functor {}", [] {
        rpp::emitter<std::string> em;
        em.connect(functor {});
        em.fire("Hello World");
    });

    do_test("em_s -> functor {}", [] {
        rpp::emitter<std::string> em;
        em.connect(functor {});
        em.fire("Hello World");
    });

    do_test("em_iis -> fn_v_v", [] {
        rpp::emitter<int, int, std::string> em;
        em.connect(fn_v_v);
        em.fire(0, 1, "Hello World");
    });

    do_test("em_iis -> l_v_i", [] {
        rpp::emitter<int, int, std::string> em;
        em.connect(l_v_i);
        em.fire(0, 1, "Hello World");
    });

    do_test("double stateless lambda connect", [] {
        rpp::emitter<> em;
        auto l = [] { n_calls++; };

        em.connect(l);
        em.connect(l);

        n_calls = 0;
        em.fire();
        test_ok = (n_calls == 1);
    });

    do_test("double stateful lambda connect", [] {
        rpp::emitter<> em;
        auto l = [&em] { n_calls++; };

        em.connect(l);
        em.connect(l);

        n_calls = 0;
        em.fire();
        test_ok = (n_calls == 2);
    });

    do_test("double function connect", [] {
        rpp::emitter<> em;

        em.connect(increment_n_calls);
        em.connect(increment_n_calls);

        n_calls = 0;
        em.fire();
        test_ok = (n_calls == 1);
    });

    do_test("connect/disconnect", [] {
        rpp::emitter<> em;
        auto c = em.connect([] {
            test_ok = false;
        });
        em.disconnect(c);

        test_ok = true;
        em.fire();
    });

    end_tests();
}
