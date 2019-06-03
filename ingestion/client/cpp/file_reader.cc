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

#include "client/cpp/file_reader.h"

#include "glog/logging.h"

namespace api {
namespace video {

FileReader::FileReader(const std::string& path)
    : IOReader(path), file_name_(path) {}

bool FileReader::Open() {
  file_fd_.reset(new std::ifstream(file_name_, std::ifstream::binary));
  if (!file_fd_->is_open()) {
    LOG(ERROR) << "Failed to open read file " << file_name_;
    return false;
  }
  return true;
}

size_t FileReader::ReadBytes(size_t max_bytes_read, char* data) {
  CHECK(data != nullptr);
  memset(data, 0, max_bytes_read);

  file_fd_->read(data, max_bytes_read);
  return file_fd_->gcount();
}

void FileReader::Close() { file_fd_->close(); }

}  // namespace video
}  // namespace api
