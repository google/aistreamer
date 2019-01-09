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

#ifndef API_VIDEO_CLIENT_CPP_STREAMING_CLIENT_H_
#define API_VIDEO_CLIENT_CPP_STREAMING_CLIENT_H_

#include <memory>
#include <string>

#include "glog/logging.h"
#include "grpc++/grpc++.h"
#include "proto/video_intelligence_streaming.grpc.pb.h"

namespace api {
namespace video {

class StreamingClient {
 public:
  StreamingClient() {}
  ~StreamingClient() = default;

  // Disallows copy and assign.
  StreamingClient(const StreamingClient&) = delete;
  StreamingClient& operator=(const StreamingClient&) = delete;

  // Initializes the client with gRPC connection.
  bool Init();

  // Runs the client.
  bool Run();

 private:
  // Reads responses from the stream. NB: It performs a blocking read.
  void ReadResponse();

  // Write streaming config to the stream.
  bool SendConfig();

  // Reads content chunks from video path and writes them to the stream.
  bool SendContent();

  // Unique pointer to a StreamingVideoIntelligenceServive stub.
  std::unique_ptr<google::cloud::videointelligence::v1::
      StreamingVideoIntelligenceService::Stub> stub_;
  // Shared pointer to the communication channel to the backend.
  std::shared_ptr<grpc::Channel> channel_;
  // gRPC client context.
  grpc::ClientContext context_;
  // Shared pointer to gRPC stream.
  std::shared_ptr<grpc::ClientReaderWriter<
      google::cloud::videointelligence::v1::StreamingAnnotateVideoRequest,
      google::cloud::videointelligence::v1::StreamingAnnotateVideoResponse>>
      stream_;
  google::cloud::videointelligence::v1::StreamingFeature feature_;
};

}  // namespace video
}  // namespace api

#endif //  API_VIDEO_CLIENT_CPP_STREAMING_CLIENT_H_
