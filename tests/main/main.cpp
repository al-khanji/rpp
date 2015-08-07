#include <rpp.hpp>

#include <iostream>
#include <string>

int main()
{
    rpp::property<int> a, b, c;

    c.onChanged.connect([] (int i) {
        std::cout << "c changed: " << i << std::endl;
    });

    c.bind([](int i, int j, int k) { return i + j + k; }, a, b, 50);

    a = 10;
    b = 20;

    a = 30;
    b = -30;

    rpp::property<std::string> etu, suku, koko;

    koko.onChanged.connect([] (const std::string &s) {
        std::cout << "koko nimi on nyt " << s << std::endl;
    });

    koko.bind([] (const std::string &s1, const std::string &s2) {
        return s1 + " " + s2;
    }, etu, suku);

    etu = "Jonna";
    suku = "Juola";

    suku = "Al-Khanji";
    etu = "Louai";
    etu = "Leevi";
    etu = "Baron";

    return 0;
}
