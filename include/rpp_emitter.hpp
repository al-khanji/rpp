#ifndef RPP_EMITTER_HPP
#define RPP_EMITTER_HPP

#include "rpp_utils.hpp"
#include <vector>
#include <memory>

namespace rpp {

namespace detail {

template <class... EmissionArgs>
struct emission_forwarder
{
    template <class T>
    struct caller
    {
        static auto call(T t, void **args)
        {
            static const int n_args_accepted = utils::accepts_n_of<T, EmissionArgs...>::value;

            return utils::apply_emission(std::forward<T>(t),
                                         args,
                                         utils::List<EmissionArgs...>{},
                                         std::make_index_sequence<n_args_accepted>{});
        }
    };

    template <class Ret, class... Args>
    struct caller<Ret (*) (Args...)>
    {
        typedef Ret (*Function) (Args...);

        static auto call(Function f, void **args)
        {
            return utils::apply_emission(std::forward<Function>(f),
                                         args,
                                         utils::List<EmissionArgs...>{},
                                         std::index_sequence_for<Args...>{});
        }
    };

    template <class T>
    static void call(T t, void **args)
    {
        caller<T>::call(std::forward<T>(t), std::forward<void**>(args));
    }
};

class emitter_base
{
public:
    class receiver_base
    {
    public:
        virtual ~receiver_base() {}
        virtual void call(void **args) = 0;
    };

protected:
    std::vector<std::shared_ptr<receiver_base>> m_connections;
};


} // namespace detail

template <class... EmissionArgs>
class emitter : public detail::emitter_base
{
public:
    using forwarder = detail::emission_forwarder<EmissionArgs...>;

    template <class Callable>
    class receiver : public receiver_base
    {
        Callable m_callable;

    public:
        receiver(Callable c) : m_callable(c) {}

        void call(void **args) override
        {
            forwarder::call(m_callable, args);
        }
    };


    void fire(EmissionArgs... args)
    {
        void *a[] = { const_cast<void *>(reinterpret_cast<const void *>(&args))...
                    , nullptr };
        for (auto &c : m_connections)
            c->call(a);
    }

    template <class Callback>
    void connect(Callback cb)
    {
        auto p = std::make_shared<receiver<Callback>>(cb);
        m_connections.push_back(p);
    }
};

} // namespace rpp

#endif // RPP_EMITTER_HPP

