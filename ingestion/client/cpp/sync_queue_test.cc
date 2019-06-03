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

#include "client/cpp/sync_queue.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace api {
namespace video {
namespace {

class SyncQueueTest : public ::testing::Test {
 protected:
  SyncQueueTest()
      : q1_(new SyncQueue<int>()),
        q2_(new SyncQueue<int>(3)),
        q3_(new SyncQueue<std::unique_ptr<int>>(3)),
        q_no_elements_(new SyncQueue<std::unique_ptr<int>>(2)) {
    for (int i = 1; i <= 5; ++i) {
      int a = i;
      q1_->TryPush(a);
      int b = i;
      q2_->TryPush(b);
      std::unique_ptr<int> c(new int(i));
      q3_->TryPush(c);
    }
  }

  std::unique_ptr<SyncQueue<int>> q1_;
  std::unique_ptr<SyncQueue<int>> q2_;
  std::unique_ptr<SyncQueue<std::unique_ptr<int>>> q3_;
  std::unique_ptr<SyncQueue<std::unique_ptr<int>>> q_no_elements_;
};

// Tests synchronous queue without maximum size.
TEST_F(SyncQueueTest, SyncQueueNoMax) {
  EXPECT_EQ(5, q1_->Size());
  EXPECT_EQ(1, q1_->Pop());
  EXPECT_EQ(4, q1_->Size());
  EXPECT_EQ(2, q1_->Pop());
  EXPECT_EQ(3, q1_->Size());
  EXPECT_EQ(3, q1_->Pop());
  EXPECT_EQ(2, q1_->Size());
  int a = 1;
  q1_->Push(a);
  EXPECT_EQ(3, q1_->Size());
  EXPECT_EQ(4, q1_->Pop());
  EXPECT_EQ(2, q1_->Size());
  EXPECT_EQ(5, q1_->Pop());
  EXPECT_EQ(1, q1_->Size());
  EXPECT_EQ(1, q1_->Pop());
  EXPECT_EQ(0, q1_->Size());
}

// Tests synchronous queue with maximum size.
TEST_F(SyncQueueTest, SyncQueueMax) {
  EXPECT_EQ(3, q2_->Size());
  EXPECT_EQ(1, q2_->Pop());
  EXPECT_EQ(2, q2_->Size());
  EXPECT_EQ(2, q2_->Pop());
  EXPECT_EQ(1, q2_->Size());
  EXPECT_EQ(3, q2_->Pop());
  EXPECT_EQ(0, q2_->Size());
  int a = 100;
  q2_->Push(a);
  EXPECT_EQ(1, q2_->Size());
  a = 200;
  q2_->Push(a);
  EXPECT_EQ(2, q2_->Size());
  EXPECT_EQ(100, q2_->Pop());
  EXPECT_EQ(1, q2_->Size());
  EXPECT_EQ(200, q2_->Pop());
  EXPECT_EQ(0, q2_->Size());
}

// Tests synchronous queue with unique_ptr.
TEST_F(SyncQueueTest, SyncQueueUniquePtr) {
  EXPECT_EQ(3, q3_->Size());
  EXPECT_EQ(1, *(q3_->Pop().get()));
  EXPECT_EQ(2, q3_->Size());
  EXPECT_EQ(2, *(q3_->Pop().get()));
  EXPECT_EQ(1, q3_->Size());
  EXPECT_EQ(3, *(q3_->Pop().get()));
  EXPECT_EQ(0, q3_->Size());
  std::unique_ptr<int> a(new int(100));
  q3_->Push(a);
  EXPECT_EQ(1, q3_->Size());
  std::unique_ptr<int> b(new int(200));
  q3_->Push(b);
  EXPECT_EQ(2, q3_->Size());
  EXPECT_EQ(100, *(q3_->Pop().get()));
  EXPECT_EQ(1, q3_->Size());
  EXPECT_EQ(200, *(q3_->Pop().get()));
  EXPECT_EQ(0, q3_->Size());

  std::unique_ptr<int> c(new int(100));
  q3_->Push(c);
  q3_->Clear();
  EXPECT_EQ(0, q3_->Size());
}

// Tests synchronous queue with unique_ptr in multithreading mode.
// Recommend to run this multiple times, via: --runs_per_test 100
TEST_F(SyncQueueTest, SyncQueueUniquePtrPush) {
  ASSERT_EQ(0, q_no_elements_->Size());
  auto make_new_int = [](int val) {
    return std::unique_ptr<int>(new int(val));
  };
  // Insert two elements (so we fill up the queue).
  auto a = make_new_int(1);
  q_no_elements_->Push(a);
  ASSERT_EQ(1, q_no_elements_->Size());
  auto b = make_new_int(2);
  q_no_elements_->Push(b);
  ASSERT_EQ(2, q_no_elements_->Size());

  // Mutexes and condition vars for the thread.
  std::mutex m;
  std::condition_variable cond_var_thread_started;
  bool thread_started = false;

  // We'll do a push in a new thread.
  std::thread new_thread(
      [this, &make_new_int, &m, &cond_var_thread_started, &thread_started] {
        {
          std::unique_lock<std::mutex> lock(m);
          thread_started = true;
          cond_var_thread_started.notify_one();
        }
        auto c = make_new_int(3);
        // This would block.
        q_no_elements_->Push(c);
      });
  // We wait until thread has started.
  {
    std::unique_lock<std::mutex> lock(m);
    cond_var_thread_started.wait(lock,
                                 [&thread_started] { return thread_started; });
  }
  // We will then check the size and pop an element.
  ASSERT_EQ(2, q_no_elements_->Size());
  ASSERT_EQ(1, *(q_no_elements_->Pop().get()));
  // Wait for thread to finish.
  new_thread.join();
  EXPECT_EQ(2, q_no_elements_->Size());
}
}  // namespace
}  // namespace video
}  // namespace api

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
