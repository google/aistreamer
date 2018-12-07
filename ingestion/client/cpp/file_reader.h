// Copyright (c) 2018 Google LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef API_VIDEO_CLIENT_CPP_FILE_READER_H_
#define API_VIDEO_CLIENT_CPP_FILE_READER_H_

#include <fstream>
#include <memory>

#include "client/cpp/io_reader.h"
#include "glog/logging.h"

namespace api {
namespace video {

class FileReader : public IOReader {
 public:
  explicit FileReader(const std::string& path);
  virtual ~FileReader() = default;

  // Disallows copy and assign.
  FileReader(const FileReader&) = delete;
  FileReader& operator=(const FileReader&) = delete;

  // Opens a file.
  bool Open();

  // Reads bytes from file.
  size_t ReadBytes(size_t max_bytes_read, char* data);

  // Closes a file.
  void Close();

 private:
  // File name.
  std::string file_name_;
  // File stream.
  std::unique_ptr<std::ifstream> file_fd_;
};

}  // namespace video
}  // namespace api

#endif //  API_VIDEO_CLIENT_CPP_FILE_READER_H_
