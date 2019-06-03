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

#ifndef API_VIDEO_CLIENT_CPP_PIPE_READER_H_
#define API_VIDEO_CLIENT_CPP_PIPE_READER_H_

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "client/cpp/io_reader.h"
#include "glog/logging.h"

namespace api {
namespace video {

class PipeReader : public IOReader {
 public:
  explicit PipeReader(const std::string& path);
  ~PipeReader() = default;

  // Disallows copy and assign.
  PipeReader(const PipeReader&) = delete;
  PipeReader& operator=(const PipeReader&) = delete;

  // Opens a pipe.
  bool Open();

  // Reads bytes from pipe.
  size_t ReadBytes(size_t max_bytes_read, char* data);

  // Closes a pipe.
  void Close();

 private:
  // Pipe reading thread.
  void ReadPipe();

  // Pipe name.
  std::string pipe_name_;
  // Pipe file descriptor.
  int pipe_fd_;
  // Cached data stream received from pipe.
  std::string data_;
  // Thread specifier.
  std::unique_ptr<std::thread> read_thread_;
  // Mutex for data_.
  std::mutex mtx_;
  // Status of read_thread_;
  std::atomic<bool> status_;
};

}  // namespace video
}  // namespace api

#endif  //  API_VIDEO_CLIENT_CPP_PIPE_READER_H_
