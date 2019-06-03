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

#ifndef API_VIDEO_CLIENT_CPP_PROTO_PROCESSOR_H_
#define API_VIDEO_CLIENT_CPP_PROTO_PROCESSOR_H_

#include "proto/video_intelligence_streaming.grpc.pb.h"

namespace api {
namespace video {

class ProtoProcessor {
 public:
  static void Process(
      const google::cloud::videointelligence::v1p3beta1::
          StreamingFeature& feature,
      const google::cloud::videointelligence::v1p3beta1::
          StreamingVideoAnnotationResults& res);

 private:
  static void ProcessExplicitContentDetection(
      const google::cloud::videointelligence::v1p3beta1::
          StreamingVideoAnnotationResults& res);

  static void ProcessLabelDetection(
      const google::cloud::videointelligence::v1p3beta1::
          StreamingVideoAnnotationResults& res);

  static void ProcessShotChangeDetection(
      const google::cloud::videointelligence::v1p3beta1::
          StreamingVideoAnnotationResults& res);

  static void ProcessObjectTracking(
      const google::cloud::videointelligence::v1p3beta1::
          StreamingVideoAnnotationResults& res);
};

}  // namespace video
}  // namespace api

#endif  //  API_VIDEO_CLIENT_CPP_PROTO_PROCESSOR_H_
