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

#include <string>

#include "client/cpp/proto_reader.h"
#include "client/cpp/proto_writer.h"
#include "google/protobuf/duration.pb.h"
#include "gtest/gtest.h"
#include "proto/video_intelligence_streaming.grpc.pb.h"

namespace api {
namespace video {
namespace {

using google::cloud::videointelligence::v1p3beta1::
    StreamingVideoAnnotationResults;
using google::cloud::videointelligence::v1p3beta1::VideoSegment;
using google::protobuf::Duration;

TEST(ProtoIo, ProtoReaderWriterTest) {
  const std::string filename = std::string(getenv("TEST_TMPDIR")) + "/proto.io";

  // Creates dummy proto message 1.
  StreamingVideoAnnotationResults res1;
  VideoSegment seg1, seg2;
  Duration dur1, dur2, dur3;
  dur1.set_seconds(0);
  dur1.set_nanos(500000000);
  dur2.set_seconds(1);
  dur2.set_nanos(600000000);
  dur3.set_seconds(2);
  dur3.set_nanos(700000000);
  *(seg1.mutable_start_time_offset()) = dur1;
  *(seg1.mutable_end_time_offset()) = dur2;
  *(seg2.mutable_start_time_offset()) = dur2;
  *(seg2.mutable_end_time_offset()) = dur3;
  *(res1.add_shot_annotations()) = seg1;
  *(res1.add_shot_annotations()) = seg2;

  // Creates dummy proto message 2.
  StreamingVideoAnnotationResults res2;
  dur1.set_seconds(100);
  dur1.set_nanos(500000000);
  dur2.set_seconds(101);
  dur2.set_nanos(600000000);
  dur3.set_seconds(102);
  dur3.set_nanos(700000000);
  *(seg1.mutable_start_time_offset()) = dur1;
  *(seg1.mutable_end_time_offset()) = dur2;
  *(seg2.mutable_start_time_offset()) = dur2;
  *(seg2.mutable_end_time_offset()) = dur3;
  *(res2.add_shot_annotations()) = seg1;
  *(res2.add_shot_annotations()) = seg2;

  // Writes proto messages to tmp file.
  ProtoWriter writer(filename);
  ASSERT_TRUE(writer.Open());
  ASSERT_TRUE(writer.WriteProto(res1));
  ASSERT_TRUE(writer.WriteProto(res2));
  writer.Close();

  // Reads proto messages from tmp file.
  LOG(INFO) << "File name: " << filename;
  ProtoReader reader(filename);
  ASSERT_TRUE(reader.Open());
  StreamingVideoAnnotationResults res3, res4;
  ASSERT_TRUE(reader.ReadProto(&res3));
  ASSERT_TRUE(reader.ReadProto(&res4));
  ASSERT_EQ(res3.ShortDebugString(), res1.ShortDebugString());
  ASSERT_EQ(res4.ShortDebugString(), res2.ShortDebugString());
  reader.Close();
}

}  // namespace
}  // namespace video
}  // namespace api

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
