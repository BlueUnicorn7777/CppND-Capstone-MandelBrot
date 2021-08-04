#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <mutex>
#include <deque>
#include <condition_variable>

template <class T>
class MessageQueue
{
public:
        T receive();
        void send(T &&msg);

private:
        std::deque<T> _queue;
        std::condition_variable _condition;
        std::mutex _mutex;
};

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock, [this] { return !_queue.empty(); });
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
     std::lock_guard<std::mutex> uLock(_mutex);
    _queue.clear() ;
    _queue.emplace_back(std::move(msg));
    _condition.notify_one();
}

#endif // MESSAGEQUEUE_H

