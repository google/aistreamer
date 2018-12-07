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

#ifndef API_VIDEO_CLIENT_CPP_IO_READER_H_
#define API_VIDEO_CLIENT_CPP_IO_READER_H_

namespace api {
namespace video {

class IOReader {
 public:
  explicit IOReader(const std::string& path) : io_name_(path) {}
  virtual ~IOReader() = default;

  // Disallows copy and assign.
  IOReader(const IOReader&) = delete;
  IOReader& operator=(const IOReader&) = delete;

  // Opens an IO channel.
  virtual bool Open() = 0;

  // Reads bytes from the IO channel.
  virtual size_t ReadBytes(size_t max_bytes_read, char* data) = 0;

  // Closes the IO channel.
  virtual void Close() = 0;

 private:
  // IO name.
  std::string io_name_;
};

}  // namespace video
}  // namespace api

#endif //  API_VIDEO_CLIENT_CPP_IO_READER_H_
