#include <rpp.hpp>
#include <iostream>
#include <cstdio>

void ifb_receiver(int i, float f, bool b)
{
    const char * const bb = b ? "true" : "false";
    std::printf("ifb_receiver(%d, %f, %s)\n", i, f, bb);
}

void if_receiver(int i, float f)
{
    std::printf("if_receiver(%d, %f)\n", i, f);
}

void i_receiver(int i)
{
    std::printf("i_receiver(%d)\n", i);
}

void v_receiver(void)
{
    std::printf("v_receiver(void)\n");
}


int main()
{
    using rpp::utils::can_call;
    using rpp::utils::accepts_n_of;

    rpp::emitter<int, float, bool> ifb_emitter;
    ifb_emitter.connect(ifb_receiver);
    ifb_emitter.connect(if_receiver);
    ifb_emitter.connect(i_receiver);
    ifb_emitter.connect(v_receiver);

    auto lambda = [] {
        std::puts("lambda called!");
    };

    auto lambda2 = [] (int i, float f) {
        std::printf("lambda2 called! %d %f \n", i, f);
    };

    using lambda_type = decltype(lambda);

    std::cout << "can call lambda() "    << can_call<lambda_type     >::value << std::endl;
    std::cout << "can call lambda(int) " << can_call<lambda_type, int>::value << std::endl;

    void f(int);
    void n();

    using f_type = decltype(f);
    using n_type = decltype(n);

    std::cout << "can call f() "               << can_call<f_type            >::value << std::endl;
    std::cout << "can call f(int) "            << can_call<f_type, int       >::value << std::endl;
    std::cout << "can call f(int, float) "     << can_call<f_type, int, float>::value << std::endl;

    std::cout << "accepts n of f(int, float) " << accepts_n_of<f_type, int, float>::value << std::endl;

    std::cout << "accepts n of n() "           << accepts_n_of<n_type            >::value << std::endl;
    std::cout << "accepts n of n(int) "        << accepts_n_of<n_type, int       >::value << std::endl;


    ifb_emitter.connect(lambda);
    ifb_emitter.connect(lambda2);

    ifb_emitter.fire(42, 2.5, true);
    return 0;
}
