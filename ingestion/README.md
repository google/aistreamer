AIStreamer Ingestion Library
===================================

[Google Cloud Video Intelligence Streaming API](https://cloud.google.com/video-intelligence/alpha/docs/streaming) enables real-time streaming analysis for live media and archived data. Supported features include:

1. Live Label Detection

2. Live Shot Change Detection

3. Live Explicit Content Detection

4. Live Object Tracking

AIStreamer ingestion library provides a set of open source interface and example code to connect to Google Cloud Video Intelligence Streaming API. The library supports:

1. File Streaming

2. HTTP Live Streaming (HLS): a HTTP based media streaming and communication protoocol.

3. Real Time Streaming Protocol (RTSP): a network control protocol for streaming media servers. It is used in conjunction with Real Time Protocol (RTP) and Real Time Control Protocol (RTCP).

4. Real Time Messaging Protocol (RTMP): a protocol for streaming audio, video and data over the Internet.

# To start using AIStreamer

AIStreamer ingestion library provides a Docker example. Please refer to individual documentation:

* [Live Streaming](documentation/live.md): Instruction for supporting live streaming protocols (HLS, RTSP and RTMP) in Google Cloud Video Intelligence.
* [File/Data Streaming](documentation/file.md): Instruction for support file/data streaming in Google Cloud Video Intelligence.
* [Docker & K8s](documentation/kube.md): Instruction of using our docker example and kubernetes deployment.
* [Live Label Detection](documentation/label.md): Instruction for streaming label detection.
* [Live Shot Change Detection](documentation/shot.md): Instruction for streaming shot change detection.
* [Live Explicit Content Detection](documentation/explicit.md): Instruction for streaming explicit content detection.
* [Live Object Tracking](documentation/object.md): Instruction for streaming object tracking.

# Code architecture

AIStreamer ingestion library includes the following three directories:

* [client](client): Python & C++ client libraries for connecting to Cloud Video Intelligence.
* [env](env): Docker example for AIStreamer ingestion.
* [proto](proto): Proto definitions and gRPC interface for Cloud Video Intelligence.

# Third-party dependency

The open source AIStreamer ingestion library is based on the following Google-owned and third-party open source libraries.

* [Bazel](https://bazel.build): A build and test tool with multi-language support.
* [gRPC](https://grpc.io): A high performance, open-source universal RPC framework.
* [Protobuf](https://developers.google.com/protocol-buffers): Google's language-neutral, platform-neutral, extensible mechanism for serializing structured data.
* [rules_protobuf](https://github.com/pubref/rules_protobuf): Bazel rules for building protocol buffers and gRPC services.
* [glog](https://github.com/google/glog): C++ implementation of the Google logging module.
* [gflags](https://github.com/gflags/gflags): C++ library that implements commandline flags processing.
* [ffmpeg](https://www.ffmpeg.org): A complete, cross-platform solution to record, convert and stream audio and video.
* [gStreamer](https://gstreamer.freedesktop.org): Another cross-platform multimedia processing and streaming framework.

# Protobuf definition

Google Cloud Video Intelligence Streaming API supports the following features in
[video_intelligence_streaming.proto](../proto/video_intelligence_streaming.proto).

```c++
// Streaming video annotation feature.
enum StreamingFeature {
  // Unspecified.
  STREAMING_FEATURE_UNSPECIFIED = 0;
  // Label detection. Detect objects, such as dog or flower.
  STREAMING_LABEL_DETECTION = 1;
  // Shot change detection.
  STREAMING_SHOT_CHANGE_DETECTION = 2;
  // Explicit content detection.
  STREAMING_EXPLICIT_CONTENT_DETECTION = 3;
  // Object tracking.
  STREAMING_OBJECT_TRACKING = 4;
}
```


AIStreamer ingestion client sends [StreamingAnnotateVideoRequest](../proto/video_intelligence_streaming.proto) to Google Cloud Video Intelligence Streaming API servers.
The first StreamingAnnotateVideoRequest message must only contain StreamingVideoConfig, and cannot include input_content. There is an option to store live annotation
results to customer specified GCS bucket. By default, this storage option is disabled.

```c++
// The top-level message sent by the client for the `StreamingAnnotateVideo`
// method. Multiple `StreamingAnnotateVideoRequest` messages are sent.
// The first message must only contain a `StreamingVideoConfig` message.
// All subsequent messages must only contain `input_content` data.
message StreamingAnnotateVideoRequest {
  // *Required* The streaming request, which is either a streaming config or
  // video content.
  oneof streaming_request {
    // Provides information to the annotator, specifing how to process the
    // request. The first `AnnotateStreamingVideoRequest` message must only
    // contain a `video_config` message.
    StreamingVideoConfig video_config = 1;

    // The video data to be annotated. Chunks of video data are sequentially
    // sent in `StreamingAnnotateVideoRequest` messages. Except the initial
    // `StreamingAnnotateVideoRequest` message containing only
    // `video_config`, all subsequent `AnnotateStreamingVideoRequest`
    // messages must only contain `input_content` field.
    bytes input_content = 2;
  }
}

// Provides information to the annotator that specifies how to process the
// request.
message StreamingVideoConfig {
  // Requested annotation feature.
  StreamingFeature feature = 1;

  // Config for requested annotation feature.
  oneof streaming_config {
    // Config for SHOT_CHANGE_DETECTION.
    StreamingShotChangeDetectionConfig shot_change_detection_config = 2;

    // Config for LABEL_DETECTION.
    StreamingLabelDetectionConfig label_detection_config = 3;

    // Config for STREAMING_EXPLICIT_CONTENT_DETECTION.
    StreamingExplicitContentDetectionConfig explicit_content_detection_config =
      4;

    // Config for STREAMING_OBJECT_TRACKING.
    StreamingObjectTrackingConfig object_tracking_config = 5;
  }

  // Streaming storage option. By default: storage is disabled.
  StreamingStorageConfig storage_config = 30;
}

// Config for streaming storage option.
message StreamingStorageConfig {
  // Enable streaming storage. Default: false.
  bool enable_storage_annotation_result = 1;

  // GCS URI to store all annotation results for one client. Client should
  // specify this field as the top-level storage directory. Annotation results
  // of different sessions will be put into different sub-directories denoted
  // by project_name and session_id. All sub-directories will be auto generated
  // by program and will be made accessible to client in response proto.
  // URIs must be specified in the following format: `gs://bucket-id/object-id`
  // `bucket-id` should be a valid GCS bucket created by client and bucket
  // permission shall also be configured properly. `object-id` can be arbitrary
  // string that make sense to client. Other URI formats will return error and
  // cause GCS write failure.
  string annotation_result_storage_directory = 3;
}
```

AIStreamer ingestion client receives [StreamingAnnotateVideoResponse](../proto/video_intelligence_streaming.proto) from Google Cloud Video Intelligence Streaming API servers.

```c++
// `StreamingAnnotateVideoResponse` is the only message returned to the client
// by `StreamingAnnotateVideo`. A series of zero or more
// `StreamingAnnotateVideoResponse` messages are streamed back to the client.
message StreamingAnnotateVideoResponse {
  // If set, returns a [google.rpc.Status][] message that
  // specifies the error for the operation.
  api.video.Status error = 1;

  // Streaming annotation results.
  StreamingVideoAnnotationResults annotation_results = 2;
}

// Streaming annotation results corresponding to a portion of the video
// that is currently being processed.
message StreamingVideoAnnotationResults {
  // Shot annotation results. Each shot is represented as a video segment.
  repeated VideoSegment shot_annotations = 1;

  // Label annotation results.
  repeated LabelAnnotation label_annotations = 2;

  // Explicit content detection results.
  ExplicitContentAnnotation explicit_annotation = 3;

  // Object tracking results.
  repeated ObjectTrackingAnnotation object_annotations = 4;
}
```

# Bidirectional streaming gRPC interface

AIStreamer ingestion client uses bidirectional streaming gRPC interface to talk to Google Cloud Video Intelligence Streaming API servers.
The bidrectional gRPC streaming interface is defined as [StreamingVideoIntelligenceService](../proto/video_intelligence_streaming.proto).

```c++
// Service that implements streaming Google Cloud Video Intelligence API.
service StreamingVideoIntelligenceService {
  // Performs video annotation with bidirectional streaming: emitting results
  // while sending video/audio bytes.
  // This method is only available via the gRPC API (not REST).
  rpc StreamingAnnotateVideo(stream StreamingAnnotateVideoRequest)
    returns (stream StreamingAnnotateVideoResponse);
}
```

AIStreamer ingestion client must use two threads (sender thread and receiver thread) to support bidirectional streaming gRPC interface.
To see Python and C++ examples related to AIStreamer, go to [client](client) directory.
To understand the basic gRPC concept and how it works, go to [gRPC documentation](https://grpc.io/docs/guides/concepts.html#bidirectional-streaming-rpc).

# Notes

1. Users can take advantage of Video API for free during the Alpha phase, however you will still incur charges for your GCS usage (not applicable for streaming API).

2. This is an Alpha release of Google Cloud Video Intelligence. As a result, it might be changed in backward-incompatible ways and it is not recommended for production use. It is not subject to any SLA or deprecation policy.
