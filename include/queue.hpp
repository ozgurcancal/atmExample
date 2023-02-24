#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace messaging {
//Base class of our queue entries
struct MessageBase {
  virtual ~MessageBase() {}
};

template <typename Msg>
//Each message type has a specialization
struct WrappedMessage : MessageBase {
  Msg content;
  explicit WrappedMessage(Msg const& msg) : content(msg) {}
};

//Our message queue
class Queue {
 public:
  template <typename T>
  void push(T const& msg) {
    std::lock_guard<std::mutex> lk(m_);
    q_.push(std::make_shared<WrappedMessage<T>>(msg)); //Wrap posted message and store pointer
    c_.notify_all();
  }

  std::shared_ptr<MessageBase> waitAndPop() {
    std::unique_lock<std::mutex> lk(m_);
    c_.wait(lk, [this] { return !q_.empty(); }); //Block until queue isn’t empty
    auto res = q_.front();
    q_.pop();
    return res;
  }

 private:
  std::mutex m_;
  std::condition_variable c_;
  std::queue<std::shared_ptr<MessageBase>> q_; //Actual queue stores pointers to message_base
};
}  // namespace messaging
