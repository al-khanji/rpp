#include <rpp.hpp>
#include <iostream>
#include <cstdio>
#include <vector>
#include <memory>
#include <tuple>

namespace utils {

template <class Callable, class... Args>
auto apply(Callable c, Args... args)
{
    return std::forward<Callable>(c)(std::forward<Args>(args)...);
}



}

using rpp::utils::List;
using rpp::utils::can_call;
using rpp::utils::accepts_n_of;

template <class>
struct CallableInfo;

//template <class...>
//struct are_arguments_compatible : std::false_type {};

//template <class T>
//struct are_arguments_compatible<T, T> : std::true_type {};

//template <>
//struct are_arguments_compatible<List<>, List<>> : std::true_type {};

//template <class List1>
//struct are_arguments_compatible<List1, List<>> : std::true_type {};

//// XXX CHECK SINGLE ARGUMENTS HERE

//template <class Arg1, class... Tail1, class Arg2, class... Tail2>
//struct are_arguments_compatible<List<Arg1, Tail1...>, List<Arg2, Tail2...>>
//        : bool_constant<are_arguments_compatible<Arg1, Arg2>::value
//                        && are_arguments_compatible<List<Tail1...>, List<Tail2...>>::value>
//{};

//template <class RType, class... FArguments>
//struct CallableInfo<RType (*) (FArguments...)>
//{
//    using ResultType = RType;
//    using ArgumentList = List<FArguments...>;

//    template <class... EmissionArguments>
//    struct can_receive_emission
//            : bool_constant<are_arguments_compatible<ArgumentList,
//                                                     List<EmissionArguments...>>::value>
//    {};
//};

//template <class T> struct remove_const_lvalue_ref           {typedef T type;};
//template <class T> struct remove_const_lvalue_ref<const T&> {typedef T type;};

//template <class T>
//using remove_const_lvalue_ref_t = typename remove_const_lvalue_ref<T>::type;

//template <class A1, class A2>
//struct are_arguments_compatible : std::is_convertible<A1, A2> {};

//template <class A1, class A2>
//struct are_arguments_compatible<A1, A2&> : std::false_type {};

//template <class A>
//struct are_arguments_compatible<A&, A&> : std::true_type {};

//template <class List1, class List2>
//struct can_emit_to : std::false_type {};

//template <>
//struct can_emit_to<List<>, List<>> : std::true_type {};

//template <class List1>
//struct can_emit_to<List1, List<>> : std::true_type {};

//template <class Arg1, class Arg2, class... Tail1, class... Tail2>
//struct can_emit_to<List<Arg1, Tail1...>,
//                   List<Arg2, Tail2...>>
//    : bool_constant< are_arguments_compatible< remove_const_lvalue_ref_t<Arg1>
//                                             , remove_const_lvalue_ref_t<Arg2>
//                                             > ::value
//                   , can_emit_to< List<Tail1...>
//                                , List<Tail2...>
//                                > ::value
//                   > {};


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

//namespace v2 {

//template <class T>
//struct property_impl
//{QStaticSlotObject
//    virtual T read() const = 0;
//    virtual void write(const T &) = 0;
//    virtual void notify() = 0;
//};

//template <class T>
//class property
//{
//public:
//    using value_type = std::decay_t<T>;

//    value_type read() const { return m_impl->read(); }
//    void write(const value_type &t) { m_impl->write(t); }
//    void notify() { m_impl->notify(); }

//protected:
//    std::unique_ptr<property_impl<value_type>> m_impl;
//};

//template <class T>
//struct property_impl_qobject : property_impl<T>
//{
//    T read() const override;
//    void write(const T &) override;
//    void notify() override;
//};

//} // namespace v2

//template <class Outer, class Getter, class Setter, class Notify>
//class property
//{
//    Outer *m_self = nullptr;
//    Getter m_getter = nullptr;
//    Setter m_setter = nullptr;
//    Notify m_notify = nullptr;

//public:
//    property(Outer *self,
//             Getter getter,
//             Setter setter,
//             Notify notify)
//        : m_self(self)
//        , m_getter(getter)
//        , m_setter(setter)
//        , m_notify(notify)
//    {}

//    auto operator()() const {
//        return (*m_self.*m_getter)();
//    }

//    property &operator=(const auto &t)
//    {
//        assert(m_setter);
//        (*m_self.*m_setter)(t);
//        return *this;
//    }
//};

//template <class Outer, class Getter, class Setter = void*, class Notify = void*>
//auto external_make_property(Outer &outer, Getter getter, Setter setter = nullptr, Notify notify = nullptr)
//{
//    using OuterT = std::decay_t<decltype(outer)>;
//    return property<OuterT, Getter, Setter, Notify> {
//        &outer, getter, setter, notify
//    };
//}

//struct S {
//    int m_count = 42;

//    void setCount(int c) {
//        if (c != m_count) {
//            m_count = c;
//            countChanged(m_count);
//        }
//    }

//    int count() {
//        return m_count;
//    }

//    void countChanged(int c) {
//        std::printf("Count Changed: %d\n", c);
//    }

//    template <class Getter, class Setter = void*, class Notify = void*>
//    auto make_property(Getter getter, Setter setter = nullptr, Notify notify = nullptr)
//    {
//        using OuterT = std::decay_t<decltype(*this)>;
//        return property<OuterT, Getter, Setter, Notify> {
//            this, getter, setter, notify
//        };
//    }
//};

//int main()
//{
//    S s;
//    s.setCount(50);

//    auto f = s.make_property(&S::count, &S::setCount, &S::countChanged);
//    std::printf("f(): %d\n", f());
//    f = 42;

//    auto ff = external_make_property(s, &S::count, &S::setCount, &S::countChanged);
//    std::printf("ff(): %d\n", ff());
//    ff = 50;


////    using rpp::property;
////    using rpp::bind;
////    using std::string;

////    property<string> name, email, full;
//////    bind(full, [](const string &name, const string &email) {
//////        if (name.empty() && email.empty()) {
//////            return "unknown";
//////        } else if (name.empty()) {
//////            return "<" + email + ">";
//////        } else if (email.empty()) {
//////            return name;
//////        } else {
//////            return name + " <" + email + ">";
//////        }
//////    }, name, email);

////    auto log = [&] { std::printf("name: '%s', email: '%s', full: '%s'\n", name.c_str(), email.c_str(), full.c_str()); };

////    log();
////    name = "Louai Al-Khanji";
////    log();
////    email = "louai@al-khanji.fi";
////    log();

//    rpp::signal<std::string> nameChanged;
//    nameChanged.connect([](const std::string &s) {
//        std::puts(s.c_str());
//    });

//    nameChanged.fire("LOUAI");

//    std::puts("ALL DONE AND ALL GOOD");
//    return 0;
//}
