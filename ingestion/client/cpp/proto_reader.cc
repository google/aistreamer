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

#include "client/cpp/proto_reader.h"

#include "glog/logging.h"

namespace api {
namespace video {

// Maximum data read: 10 KBytes.
constexpr int kMaxDataRead = 10 * 1024;

ProtoReader::ProtoReader(const std::string& path)
    : IOReader(path), file_name_(path) {}

bool ProtoReader::Open() {
  file_fd_.reset(new std::ifstream(file_name_, std::ifstream::binary));
  if (!file_fd_->is_open()) {
    LOG(ERROR) << "Failed to open read file " << file_name_;
    return false;
  }
  return true;
}

size_t ProtoReader::ReadBytes(size_t max_bytes_read, char* data) {
  CHECK(data != nullptr);
  memset(data, 0, max_bytes_read);

  size_t bytes_read;
  char bytes_size[4];  // size of uint32_t
  file_fd_->read(bytes_size, 4);
  if (file_fd_->gcount() == 4) {
    bytes_read = static_cast<int>(bytes_size[0]) +
                 (static_cast<int>(bytes_size[1]) << 8) +
                 (static_cast<int>(bytes_size[2]) << 16) +
                 (static_cast<int>(bytes_size[3]) << 24);
    // Confirms that max allowable data is larger than bytes_read.
    // If not, exits the process.
    CHECK(bytes_read < max_bytes_read);
    // Reads bytes of exact bytes_read size.
    file_fd_->read(data, bytes_read);
    // Confirms that read bytes is of the size bytes_read.
    // If not, exits the process.
    CHECK(bytes_read == static_cast<size_t>(file_fd_->gcount()));
    return bytes_read;
  }
  return 0;
}

bool ProtoReader::ReadProto(google::protobuf::MessageLite* message) {
  CHECK(message != nullptr);
  char data[kMaxDataRead];
  size_t bytes_read = ReadBytes(kMaxDataRead, data);
  if (bytes_read > 0) {
    return message->ParseFromArray(data, bytes_read);
  }
  return false;
}

void ProtoReader::Close() { file_fd_->close(); }

}  // namespace video
}  // namespace api
