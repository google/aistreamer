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

#include "client/cpp/proto_writer.h"

#include "glog/logging.h"

namespace api {
namespace video {

ProtoWriter::ProtoWriter(const std::string& path)
    : IOWriter(path), file_name_(path) {}

bool ProtoWriter::Open() {
  file_fd_.reset(new std::ofstream(file_name_, std::ofstream::binary));

  if (!file_fd_->is_open()) {
    LOG(ERROR) << "Failed to open write file " << file_name_;
    return false;
  }
  return true;
}

bool ProtoWriter::WriteBytes(size_t bytes_written, char* data) {
  CHECK(data != nullptr);

  char bytes_size[4];  // size of uint32_t
  bytes_size[0] = bytes_written & 255;
  bytes_size[1] = (bytes_written >> 8) & 255;
  bytes_size[2] = (bytes_written >> 16) & 255;
  bytes_size[3] = (bytes_written >> 24) & 255;
  file_fd_->write(bytes_size, 4);
  file_fd_->write(data, bytes_written);
  return true;
}

bool ProtoWriter::WriteProto(const google::protobuf::MessageLite& message) {
  std::string data;
  if (message.SerializeToString(&data)) {
    return WriteBytes(data.size(), const_cast<char*>(data.c_str()));
  }
  return false;
}

void ProtoWriter::Close() { file_fd_->close(); }

}  // namespace video
}  // namespace api
