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

#include "client/cpp/streaming_client.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>
#include <vector>

#include "client/cpp/file_reader.h"
#include "client/cpp/file_writer.h"
#include "client/cpp/pipe_reader.h"
#include "client/cpp/proto_processor.h"
#include "client/cpp/proto_writer.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

namespace api {
namespace video {

namespace {
using google::cloud::videointelligence::v1::STREAMING_LABEL_DETECTION;
using google::cloud::videointelligence::v1::STREAMING_SHOT_CHANGE_DETECTION;
using google::cloud::videointelligence::v1::StreamingAnnotateVideoRequest;
using google::cloud::videointelligence::v1::StreamingAnnotateVideoResponse;
using google::cloud::videointelligence::v1::StreamingFeature;
using google::cloud::videointelligence::v1::StreamingVideoConfig;
using google::cloud::videointelligence::v1::StreamingVideoIntelligenceService;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
}  // namespace

DEFINE_string(endpoint, "dns:///alpha-videointelligence.googleapis.com",
    "API endpoint to connect to.");
DEFINE_int32(feature, STREAMING_SHOT_CHANGE_DETECTION,
    "Streaming Feature. Default: shot change detection.");
DEFINE_string(video_path, "", "Input video path.");
DEFINE_string(enable_cache, "", "Cached video path.");
DEFINE_string(proto_path, "", "Output video path.");
DEFINE_int32(timeout, 10800, "GRPC deadline (default: 3 hours).");
DEFINE_bool(stationary_camera, false,
    "Whether it is stationary camera user case.");
DEFINE_bool(use_pipe, false, "Whether reading from a pipe.");

// Maximum data chunks read: 1 MByte.
constexpr int kDataChunk = 1 * 1024 * 1024;

bool StreamingClient::Init() {
  auto ssl_credentials = grpc::GoogleDefaultCredentials();
  channel_ = grpc::CreateChannel(FLAGS_endpoint, ssl_credentials);

  // Creates a stub call.
  stub_ = StreamingVideoIntelligenceService::NewStub(channel_);

  std::chrono::system_clock::time_point timeout =
      std::chrono::system_clock::now() +
          std::chrono::seconds(FLAGS_timeout);
  context_.set_deadline(timeout);

  // Inits and starts a gRPC client.
  stream_ =
      std::shared_ptr<ClientReaderWriter<StreamingAnnotateVideoRequest,
                                         StreamingAnnotateVideoResponse>>(
          stub_->StreamingAnnotateVideo(&context_));
  grpc_connectivity_state state = channel_->GetState(/*try_to_connect*/ true);
  if (state != GRPC_CHANNEL_READY) {
    LOG(ERROR) << "grpc_connectivity_state error: " << std::to_string(state);
    return false;
  }
  return true;
}

bool StreamingClient::Run() {
  if (!SendConfig()) {
    return false;
  }
  std::thread reader([this] { ReadResponse(); });
  bool status = SendContent();
  reader.join();

  grpc::Status grpc_status = stream_->Finish();
  if (!grpc_status.ok()) {
    LOG(ERROR) << "StreamingAnnotateVideo RPC failed: Code("
               << grpc_status.error_code() << "): "
               << grpc_status.error_message();
    status = false;
  }
  return status;
}

void StreamingClient::ReadResponse() {
  StreamingAnnotateVideoResponse resp;
  int total_responses_received = 0;
  
  bool enable_proto_writer = (FLAGS_proto_path != "");
  std::unique_ptr<ProtoWriter> writer;
  if (enable_proto_writer) {
    writer.reset(new ProtoWriter(FLAGS_proto_path));
    CHECK(writer->Open()) << "Failed to write to " << FLAGS_proto_path;
  }

  while (stream_->Read(&resp)) {
    total_responses_received++;
    ProtoProcessor::Process(static_cast<StreamingFeature>(FLAGS_feature),
                            resp.annotation_results());
    if (resp.has_error()) {
      LOG(ERROR) << "Received an error: " << resp.error().message();
    } else if (enable_proto_writer) {
      writer->WriteProto(resp.annotation_results());
    } 
  }
  LOG(INFO) << "Received " << total_responses_received << " responses.";
  if (enable_proto_writer) {
    writer->Close();
  }
}

bool StreamingClient::SendConfig() {
  // All the config details must be sent in the first request.
  StreamingAnnotateVideoRequest req;
  StreamingVideoConfig* config = req.mutable_video_config();
  config->set_feature(static_cast<StreamingFeature>(FLAGS_feature));
  if (config->feature() == STREAMING_LABEL_DETECTION) {
    config->mutable_label_detection_config()->set_stationary_camera(
        FLAGS_stationary_camera);
  }
  if (!stream_->Write(req)) {
    LOG(ERROR) << "Failed to send config: " << req.ShortDebugString();
    return false;
  }
  return true;
}

bool StreamingClient::SendContent() {
  bool status = true;

  std::unique_ptr<IOReader> reader;
  if (FLAGS_use_pipe) {
    reader.reset(new PipeReader(FLAGS_video_path));
  } else {
    reader.reset(new FileReader(FLAGS_video_path));
  }
  CHECK(reader->Open()) << "Failed to read from " << FLAGS_video_path;

  std::unique_ptr<IOWriter> writer;
  bool enable_cache = (FLAGS_enable_cache != "");
  if (enable_cache) {
    writer.reset(new FileWriter(FLAGS_enable_cache));
    CHECK(writer->Open()) << "Failed to write to " << FLAGS_enable_cache;
  }

  int requests_sent = 0;
  long total_bytes_read = 0;
  std::vector<char> buffer(kDataChunk + 1, 0);

  while (true) {
    size_t num_bytes_read = reader->ReadBytes(kDataChunk, buffer.data());
    if (num_bytes_read == 0) {
      break;
    }
    StreamingAnnotateVideoRequest req;
    req.set_input_content(buffer.data(), num_bytes_read);
    if (!stream_->Write(req)) {
      LOG(ERROR) << "Failed to send content: " << req.ShortDebugString();
      status = false;
      break;
    }
    if (enable_cache) {
      writer->WriteBytes(num_bytes_read, buffer.data());
    }
    total_bytes_read += num_bytes_read;
    requests_sent++;
  }

  if (!stream_->WritesDone()) {
    LOG(ERROR) << "Failed to mark WritesDone in gRPC stream.";
    status = false;
  }

  reader->Close();
  if (enable_cache) {
    writer->Close();
  }

  LOG(INFO) << "Sent " << requests_sent
            << " requests consisting of " << total_bytes_read
            << " bytes of video data in total.";
  return status;
}

}  // namespace video
}  // namespace api
