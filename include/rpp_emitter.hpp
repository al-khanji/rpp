#ifndef RPP_EMITTER_HPP
#define RPP_EMITTER_HPP

#include "rpp_utils.hpp"
#include "rpp_concepts.hpp"

#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>

namespace rpp {

template <class F, class... Args>
struct n_args_accepted : std::conditional_t < MODELS_CONCEPT(concepts::Callable(F, Args...))
                                            , utils::int_constant<sizeof...(Args)>
                                            , n_args_accepted<F, utils::left_n_t<sizeof...(Args)-1, Args...>> >
{};

template <class F>
struct n_args_accepted<F> : std::conditional_t < MODELS_CONCEPT(concepts::Callable(F))
                                               , utils::int_constant<  0 >
                                               , utils::int_constant< -1 > >
{};

template <class F, class... Args>
struct n_args_accepted<F, utils::typelist<Args...>> : n_args_accepted<F, Args...>
{};

template <class F, std::size_t... Is, class... Args>
auto emit_to(F &&f, void **args, std::index_sequence<Is...>, utils::typelist<Args...>)
{
    // Avoid warnings about unused variables in the case of functions that take no arguments.
    (void) args;

    return f( *reinterpret_cast<utils::select_nth_t<Is, Args...> *>(args[Is])... );
}

namespace detail {

struct emitter_base
{
protected:
    class receiver_base
    {
    public:
        virtual ~receiver_base() {}
        virtual void call(void **args) = 0;
        virtual bool compare(const void *arg) const = 0;

        template <class T, class = decltype(std::declval<T>() == std::declval<T>())>
        static bool compare_impl(const void *left, const void *right)
        {
            return *reinterpret_cast<const T *>(left) == *reinterpret_cast<const T *>(right);
        }

        template <class>
        static bool compare_impl(...) { return false; }
    };

    std::vector<std::shared_ptr<receiver_base>> m_connections;

public:
    class connection
    {
    public:
        connection() = default;

        connection(const connection &) = default;
        connection(connection &&) = default;

        connection &operator=(const connection &) = default;
        connection &operator=(connection &&) = default;

        connection(emitter_base *emitter, std::shared_ptr<receiver_base> receiver)
            : m_emitter(emitter)
            , m_receiver(receiver)
        {
            assert(m_emitter);
        }

        ~connection()
        {}

        bool disconnect()
        {
            if (!m_receiver.expired()) {
                m_emitter->disconnect(m_receiver);
                return true;
            }

            return false;
        }

        bool expired() const noexcept {
            return m_receiver.expired();
        }

        void detach() noexcept {
            m_receiver.reset();
        }

    private:
        emitter_base *m_emitter = nullptr;
        std::weak_ptr<receiver_base> m_receiver;
    };

    static void disconnect(connection &c)
    {
        c.disconnect();
    }

    bool disconnect(const std::weak_ptr<receiver_base> &weak)
    {
        const auto strong = weak.lock();
        if (!strong)
            return false;

        auto ii = std::find(m_connections.cbegin(), m_connections.cend(), strong);
        if (ii != m_connections.cend()) {
            m_connections.erase(ii);
            return true;
        }

        return false;
    }

    void disconnect_all()
    {
        m_connections.clear();
    }
};


} // namespace detail


template <class... EmissionArgs>
class emitter : public detail::emitter_base
{
    using dereffed_args = utils::typelist< std::add_const_t<EmissionArgs>... >;

    template <class Callable>
    struct receiver : receiver_base
    {
        Callable m_callable;
        static const int n_accepted = n_args_accepted<Callable, dereffed_args>();

        receiver(Callable c)
            : m_callable(std::forward<Callable>(c))
        {
            static_assert(n_accepted >= 0, "");
        }

        void call(void **args) override
        {
            emit_to(m_callable,
                    args,
                    std::make_index_sequence<n_accepted> {},
                    utils::typelist_left_t<n_accepted, dereffed_args> {});
        }

        bool compare(const void *arg) const override
        {
            return compare_impl<Callable>(arg, reinterpret_cast<const void *>(std::addressof(m_callable)));
        }
    };

public:
    void fire(EmissionArgs... args)
    {
        void *a[] = { const_cast<void *>(reinterpret_cast<const void *>(&args))... , nullptr };

        for (auto &c : m_connections)
            c->call(a);
    }

    template <class Callback>
    connection connect(Callback cb)
    {
        using receiver_t = receiver<Callback>;
        std::shared_ptr<receiver_t> receiver;

        auto cb_ptr = reinterpret_cast<const void *>(std::addressof(cb));
        for (const auto &r : m_connections) {
            if (r->compare(cb_ptr)) {
                receiver = std::static_pointer_cast<receiver_t>(r);
            }
        }

        if (!receiver) {
            receiver = std::make_shared<receiver_t>(std::forward<Callback>(cb));
            m_connections.push_back(receiver);
        }

        return connection(this, receiver);
    }
};

} // namespace rpp

#endif // RPP_EMITTER_HPP

