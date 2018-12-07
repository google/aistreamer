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

#include "client/cpp/proto_processor.h"

#include "glog/logging.h"

namespace api {
namespace video {

namespace {
using google::cloud::videointelligence::v1::STREAMING_LABEL_DETECTION;
using google::cloud::videointelligence::v1::STREAMING_SHOT_CHANGE_DETECTION;
using google::cloud::videointelligence::v1::StreamingFeature;
using google::cloud::videointelligence::v1::StreamingVideoAnnotationResults;
}  // namespace

void ProtoProcessor::Process(const StreamingFeature& feature,
                             const StreamingVideoAnnotationResults& res) {
  switch (feature) {
    case STREAMING_LABEL_DETECTION:
      ProcessLabelDetection(res);
      break;
    case STREAMING_SHOT_CHANGE_DETECTION:
      ProcessShotChangeDetection(res);
      break;
    default:
      LOG(ERROR) << "Unsupported inference feature: " << feature;
      break;
  }
}

void ProtoProcessor::ProcessLabelDetection(
    const StreamingVideoAnnotationResults& res) {
  LOG(INFO) << "Time Offset\tDescription\tConfidence";
  for (auto& annotation : res.label_annotations()) {
    std::string description = annotation.entity().description();
    double time_offset = annotation.frames(0).time_offset().seconds() +
                         annotation.frames(0).time_offset().nanos() / 1e9;
    float confidence = annotation.frames(0).confidence();
    LOG(INFO) << time_offset << "s:\t" << description << "\t" << confidence;
  }
}

void ProtoProcessor::ProcessShotChangeDetection(
    const StreamingVideoAnnotationResults& res) {
  for (auto& annotation : res.shot_annotations()) {
    double start_time = annotation.start_time_offset().seconds() +
                        annotation.start_time_offset().nanos() / 1e9;
    double end_time = annotation.end_time_offset().seconds() +
                      annotation.end_time_offset().nanos() / 1e9;
    LOG(INFO) << "Shot: " << start_time << "s\t" << end_time << "s";
  }
}

}  // namespace video
}  // namespace api
