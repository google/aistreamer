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

#include "client/cpp/pipe_reader.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <algorithm>

#include "glog/logging.h"

namespace api {
namespace video {

// Pipe size. In Linux, by default: 65536 bytes.
constexpr int kPipeSize = 65536;
// Pipe polling interval in milliseconds.
constexpr int kPollingIntervalMs = 1000;

PipeReader::PipeReader(const std::string& path)
    : IOReader(path), pipe_name_(path), pipe_fd_(-1) {}

bool PipeReader::Open() {
  CHECK(pipe_fd_ == -1);
  pipe_fd_ = open(pipe_name_.c_str(), O_RDONLY | O_NONBLOCK);
  if (pipe_fd_ == -1) {
    LOG(ERROR) << "Failed to open pipe " << pipe_name_;
    return false;
  }
  read_thread_.reset(new std::thread([this] { ReadPipe(); }));
  read_thread_->detach();
  return true;
}

size_t PipeReader::ReadBytes(size_t max_bytes_read, char* data) {
  CHECK(data != nullptr);
  memset(data, 0, max_bytes_read);
  while (status_) {
    std::lock_guard<std::mutex> lck(mtx_);
    size_t bytes_read =
        (max_bytes_read > data_.size()) ? data_.size() : max_bytes_read;
    if (bytes_read > 0) {
      memcpy(data, data_.c_str(), bytes_read);
      data_.erase(0, bytes_read);
      return bytes_read;
    }
  }
  size_t bytes_read =
      (max_bytes_read > data_.size()) ? data_.size() : max_bytes_read;
  memcpy(data, data_.c_str(), bytes_read);
  data_.erase(0, bytes_read);
  return bytes_read;
}

void PipeReader::ReadPipe() {
  char data[kPipeSize];
  status_ = true;
  while (true) {
    // Do polling for non-blocking fd read.
    pollfd fds;
    memset(&fds, 0, sizeof(fds));
    fds.fd = pipe_fd_;
    fds.events = POLLIN | POLLHUP | POLLERR;

    int res = poll(&fds, 1, kPollingIntervalMs);
    if (res == 0) {
      continue;
    }
    if (res < 0 && (errno != EINTR) && (errno != EAGAIN)) {
      LOG(ERROR) << pipe_name_ << " pipe failed: " << strerror(errno);
      status_ = false;
      return;
    }
    if (res > 0) {
      if ((fds.revents & POLLIN) != 0) {
        do {
          int new_bytes_read = read(pipe_fd_, data, kPipeSize);
          if (new_bytes_read < 0) {
            if (errno == EINTR) {
              continue;
            } else if (errno == EAGAIN) {
              break;
            } else {
              LOG(ERROR) << pipe_name_ << " pipe failed: " << strerror(errno);
              status_ = false;
              return;
            }
          }
          if (new_bytes_read == 0) {
            break;
          }
          if (new_bytes_read > 0) {
            std::lock_guard<std::mutex> lck(mtx_);
            data_.append(data, new_bytes_read);
          }
        } while (true);
      }
      if ((fds.revents & (POLLHUP | POLLERR)) != 0) {
        LOG(INFO) << "Pipe " << pipe_name_
                  << " has been closed by remote side.";
        status_ = false;
        return;
      }
    }
  }
  status_ = false;
}

void PipeReader::Close() {
  if (pipe_fd_ != -1) {
    close(pipe_fd_);
    pipe_fd_ = -1;
  }
}

}  // namespace video
}  // namespace api
