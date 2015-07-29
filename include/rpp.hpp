#ifndef RPP_HPP_INCLUDED
#define RPP_HPP_INCLUDED

#include "rpp_utils.hpp"
#include "rpp_emitter.hpp"

namespace rpp {

//class signal_slot_connection : public std::enable_shared_from_this<signal_slot_connection>
//{
//public:
//    enum class Operation { Destroy, Call, Compare };

//private:
//    typedef void (*impl_fn)(Operation op, signal_slot_connection *this_, void **args, bool *ret);
//    const impl_fn m_impl = nullptr;

//public:
//    signal_slot_connection() = delete;
//    signal_slot_connection(const signal_slot_connection &) = delete;
//    void operator=(const signal_slot_connection &) = delete;

//    signal_slot_connection(impl_fn fn) : m_impl(fn)
//    {
//        assert(fn);
//    }

//    ~signal_slot_connection()
//    {
//        m_impl(Operation::Destroy, this, nullptr, nullptr);
//    }

//    bool compare(void **args)
//    {
//        bool ret = false;
//        m_impl(Operation::Compare, this, args, &ret);
//        return ret;
//    }

//    void call(void **args)
//    {
//        m_impl(Operation::Call, this, args, nullptr);
//    }
//};

//template <class FuncType, class SignalReturnType, class... SignalArgs>
//class function_signal_slot_connection : public signal_slot_connection
//{
//    FuncType m_function;
//    static void impl(Operation op, signal_slot_connection *this_, void **args, bool *ret)
//    {
//        auto *self = static_cast<function_signal_slot_connection *>(this_);

//        switch (op) {
//        case Operation::Call:
//            //util::invoke<SignalArgs..., SignalReturnType>(m_function, args);
//            break;
//        case Operation::Compare:
//            *ret = false; // XXX implement me
//            break;
//        case Operation::Destroy:
//            self->m_function.~FuncType();
//            break;
//        }
//    }

//public:
//    explicit function_signal_slot_connection(FuncType f)
//        : signal_slot_connection(&impl)
//        , m_function(f)
//    {}
//};

//template <class T>
//class signal
//{
//    std::vector<std::shared_ptr<signal_slot_connection>> m_connections;

//public:
//    template <class SlotType>
//    std::weak_ptr<signal_slot_connection> connect(SlotType s);

//    bool disconnect(const std::weak_ptr<signal_slot_connection> &connection);

//    void fire(T t);
//};

//template <class T>
//template <class SlotType>
//std::weak_ptr<signal_slot_connection> signal<T>::connect(SlotType)
//{
//    return std::weak_ptr<signal_slot_connection>();
//}

////template <class T>
////std::weak_ptr<signal_slot_connection> signal<T>::connect(std::function<void(T)> fn)
////{
////    fn(T{});
////    return std::weak_ptr<signal_slot_connection>();
////}

//template <class T>
//void signal<T>::fire(T t)
//{
//    void *args[] = { &t, nullptr };
//    for (auto &signal_slot_connection : m_connections)
//        signal_slot_connection->call(args);
//}

//template <class...>
//using void_t = void;

//struct connection_base;

//template <class... Args>
//struct source;

//template <class... Args>
//struct sink;

//template <class... Args>
//struct source :;

//template <class T>
//using property = T;

//template <class T, class F, class... FArgs>
//void bind(T &t, F&& f, FArgs...)
//{}

//template <class T>
//struct signal
//{
//    void fire(const T& t) {
//        auto ii = _receivers.begin();

//        while (ii != _receivers.end()) {
//            if (*ii) {
//                (*ii)(t);
//                std::next(ii);
//            } else {
//                _receivers.erase(ii);
//            }
//        }

//        for (auto &r : _receivers)
//            r(t);
//    }

//    std::vector<signal_receiver<T>> _receivers;
//};

//template <class T>
//struct changed_signal_arg
//{
//    using U = std::decay_t<T>;
//    using type = std::conditional_t<std::is_scalar<U>
//};

//template <class T>
//struct property
//{
//    vector<> inputs;
//    vector<> outputs;

//    signal<T> changed;

//    void make_input(f, fargs...)
//    {
//        vector<> sinks = { make_sink(fargs)... };
//        auto i = input { f, sinks };
//        i.setReceiver(this);

//        return i;
//    }

//    void bind(f, fargs...)
//    {
//        assert(is_callable(f(fargs...)));

//        input = make_input(f, fargs);
//        inputs.append(input);

//        *this = input();
//    }
//};

} // namespace rpp

#endif // RPP_HPP_INCLUDED
