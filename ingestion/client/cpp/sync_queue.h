// Copyright (c) 2019 Google LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef API_VIDEO_CLIENT_CPP_SYNC_QUEUE_H_
#define API_VIDEO_CLIENT_CPP_SYNC_QUEUE_H_

#include <climits>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace api {
namespace video {

// Implements a thread-safe synchronous queue.
// More sophisticated implementation is 'ConcurrentLinkedQueue'.
// https://stackoverflow.com/questions/1426754/linkedblockingqueue-vs-concurrentlinkedqueue
template <class T>
class SyncQueue {
 public:
  // Constructs an empty thread-safe synchronous queue.
  // Maximum queue size allowed is set in input argument.
  explicit SyncQueue(size_t max_size = INT_MAX) : max_size_(max_size) {}

  // Destructs a synchronous queue.
  ~SyncQueue() = default;

  // Pushes an element into synchronous queue. This method returns only after
  // successfully pushing the element into the queue.
  void Push(T& val) {
    std::unique_lock<std::mutex> lock(m_);
    cond_var_element_popped_.wait(
        lock, [this] { return q_.size() < this->max_size_; });
    q_.push(std::move(val));
    cond_var_element_pushed_.notify_one();
  }

  // Tries to push an element into synchronous queue.
  // It returns true if the element is successfully pushed,
  // otherwise returns false because queue is full.
  bool TryPush(T& val) {
    std::lock_guard<std::mutex> lock(m_);
    if (q_.size() < max_size_) {
      q_.push(std::move(val));
      cond_var_element_pushed_.notify_one();
      return true;
    }
    return false;
  }

  // Pops an element from synchronous queue.
  T Pop() {
    std::unique_lock<std::mutex> lock(m_);
    cond_var_element_pushed_.wait(lock, [this] { return !q_.empty(); });
    T val = std::move(q_.front());
    q_.pop();
    cond_var_element_popped_.notify_one();
    return val;
  }

  void Clear() {
    std::lock_guard<std::mutex> lock(m_);
    while (!q_.empty()) {
      q_.pop();
    }
  }

  // Gets queue size.
  size_t Size() {
    std::lock_guard<std::mutex> lock(m_);
    return q_.size();
  }

 private:
  // Storage queue.
  std::queue<T> q_;
  // Queue max size.
  const int max_size_;
  // Mutex.
  mutable std::mutex m_;
  // Condition variables.
  std::condition_variable cond_var_element_pushed_;
  std::condition_variable cond_var_element_popped_;
};

}  // namespace video
}  // namespace api

#endif  // API_VIDEO_CLIENT_CPP_SYNC_QUEUE_H_
