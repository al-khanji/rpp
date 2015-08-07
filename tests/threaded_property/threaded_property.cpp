#include <rpp.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <iostream>

template <class T, class Property>
struct owning_access_policy
{
    const T &read() const {
        return m_value;
    }

    void write(const T &t) {
        m_value = t;
        static_cast<Property *>(this)->notify(t);
    }

protected:
    T m_value {};
};

template <class T, class Property>
struct threaded_notify_policy
{
    ~threaded_notify_policy() {
        lock_t lock(m_mutex);
        m_quitting = true;
        lock.unlock();

        m_cv.notify_one();

        if (m_scheduler.joinable()) {
            puts("JOINING");
            m_scheduler.join();
        }
    }

    void notify(const T &t) {
        ensureScheduler();

        lock_t lock(m_mutex);
        m_scheduledValues.push_back(t);
        lock.unlock();

        m_cv.notify_one();
    }

    template <class Callback>
    auto onChanged(Callback cb) {
        return m_emitter.connect(cb);
    }

protected:
    using lock_t = std::unique_lock<std::mutex>;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    std::vector<T> m_scheduledValues;

    bool m_quitting = false;
    std::thread m_scheduler;

    rpp::emitter<T> m_emitter;

    void ensureScheduler() {
        if (m_scheduler.joinable())
            return;

        m_scheduler = std::thread([&] {
            while (true) {
                lock_t lock(m_mutex);
                m_cv.wait(lock, [this] { return !m_scheduledValues.empty(); });

                auto values = std::move(m_scheduledValues);
                printf("Moved %lu values\n", values.size());
                lock.unlock();

                for (const auto &v : values)
                    m_emitter.fire(v);

                lock.lock();
                if (m_quitting)
                    return;
            }
        });
    }
};

template < class T
         , template <class...> class AccessPolicy = owning_access_policy
         , template <class...> class NotifyPolicy = threaded_notify_policy >
class property
    : public AccessPolicy<T, property<T, AccessPolicy, NotifyPolicy>>
    , public NotifyPolicy<T, property<T, AccessPolicy, NotifyPolicy>>
{};

int main()
{
    property<int> prop;

    prop.onChanged([] (int i) {
        std::printf("int changed: %d - SLEEPING\n", i);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::printf("int changed: %d - SLEPT\n", i);
    });

    prop.write(50);
    prop.write(5550);

    std::this_thread::yield();

    prop.write(1337);
    prop.write(666);

    std::puts("MAIN THREAD SLEEPING FOR 3 SEC");
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::puts("MAIN THREAD AWAKE");

    prop.write(150);
    prop.write(250);
    prop.write(500);

    return 0;
}
