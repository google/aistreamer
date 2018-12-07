AIStreamer Ingestion Library - Live Label Detection
===================================

[Google Cloud Video Intelligence Streaming API](https://cloud.google.com/video-intelligence/alpha/docs/streaming) supports
live label detection with live/file streaming input of media data, and real-time streaming output of analysis results.

# Feature enum

[SteamingFeature](../proto/video_intelligence_streaming.proto) defines feature enum for live label detection.

```c++
// Label detection. Detect objects, such as dog or flower.
STREAMING_LABEL_DETECTION = 1;
```

# Input config

[StreamingLabelDetectionConfig](../proto/video_intelligence_streaming.proto) defines input configuration for live label detection.

```c++
// Config for LABEL_DETECTION in streaming mode.
message StreamingLabelDetectionConfig {
  // Whether the video has been captured from a stationary (i.e. non-moving)
  // camera. When set to true, might improve detection accuracy for moving
  // objects. Default: false.
  bool stationary_camera = 1;
}
```

By default, stationary_camera option is set to FALSE. If it is set to TRUE, Google Cloud Video Intelligence Streaming API server
will choose a model optimized for images captured by stationary camera.

# Output analysis results

[LabelAnnotation](../proto/video_intelligence_streaming.proto) defines output analysis results for live label detection.

```c++
message StreamingVideoAnnotationResults {
  // Label annotation results.
  repeated LabelAnnotation label_annotations = 2;
}

// Label annotation.
message LabelAnnotation {
  // Detected entity.
  Entity entity = 1;
  // Common categories for the detected entity.
  // E.g. when the label is `Terrier` the category is likely `dog`. And in some
  // cases there might be more than one categories e.g. `Terrier` could also be
  // a `pet`.
  repeated Entity category_entities = 2;
  // All video segments where a label was detected.
  repeated LabelSegment segments = 3;
  // All video frames where a label was detected.
  repeated LabelFrame frames = 4;
}

// Detected entity from video analysis.
message Entity {
  // Opaque entity ID. Some IDs may be available in
  // [Google Knowledge Graph Search
  // API](https://developers.google.com/knowledge-graph/).
  string entity_id = 1;
  // Textual description, e.g. `Fixed-gear bicycle`.
  string description = 2;
  // Language code for `description` in BCP-47 format.
  string language_code = 3;
}

// Video segment level annotation results for label detection.
message LabelSegment {
  // Video segment where a label was detected.
  VideoSegment segment = 1;
  // Confidence that the label is accurate. Range: [0, 1].
  float confidence = 2;
}

// Video frame level annotation results for label detection.
message LabelFrame {
  // Time-offset, relative to the beginning of the video, corresponding to the
  // video frame for this location.
  google.protobuf.Duration time_offset = 1;
  // Confidence that the label is accurate. Range: [0, 1].
  float confidence = 2;
}
```

# Example output

```
Reading response.
0.0s: mustang horse  (0.9374721646308899)
0.0s: herd           (0.9230296611785889)
0.0s: horse          (0.9789802432060242)
```
