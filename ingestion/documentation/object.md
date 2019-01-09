AIStreamer Ingestion Library - Live Object Detection and Tracking
===================================

[Google Cloud Video Intelligence Streaming API](https://cloud.google.com/video-intelligence/alpha/docs/streaming) supports
live object detection and tracking with streaming input of media data and streaming output of analysis results.

# Feature enum

[SteamingFeature](../proto/video_intelligence_streaming.proto) defines feature enum for live object detection and tracking.

```c++
// Object detection and tracking.
STREAMING_OBJECT_TRACKING = 4;
```

# Input config

[StreamingObjectTrackingConfig](../proto/video_intelligence_streaming.proto) defines input configuration for live object detection and tracking.

```c++
// Config for STREAMING_OBJECT_TRACKING.
message StreamingObjectTrackingConfig {
  // No customized config support.
}
```

# Output analysis results

[ObjectTrackingAnnotation](../proto/video_intelligence_streaming.proto) defines output analysis results for live object detection and tracking.

```c++
message StreamingVideoAnnotationResults {
  // Object tracking results.
  repeated ObjectTrackingAnnotation object_annotations = 4;
}

// Normalized bounding box.
// The normalized vertex coordinates are relative to the original image.
// Range: [0, 1].
message NormalizedBoundingBox {
  // Left X coordinate.
  float left = 1;
  // Top Y coordinate.
  float top = 2;
  // Right X coordinate.
  float right = 3;
  // Bottom Y coordinate.
  float bottom = 4;
}

// Video frame level annotations for object detection and tracking. This field
// stores per frame location, time offset, and confidence.
message ObjectTrackingFrame {
  // The normalized bounding box location of this object track for the frame.
  NormalizedBoundingBox normalized_bounding_box = 1;

  // The timestamp of the frame in microseconds.
  google.protobuf.Duration time_offset = 2;
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

// Video segment.
message VideoSegment {
  // Time-offset, relative to the beginning of the video,
  // corresponding to the start of the segment (inclusive).
  google.protobuf.Duration start_time_offset = 1;
  // Time-offset, relative to the beginning of the video,
  // corresponding to the end of the segment (inclusive).
  google.protobuf.Duration end_time_offset = 2;
}

// Annotations corresponding to one tracked object.
message ObjectTrackingAnnotation {
  // Entity to specify the object category that this track is labeled as.
  Entity entity = 1;

  // Object category's labeling confidence of this track.
  float confidence = 4;

  // Information corresponding to all frames where this object track appears.
  // Non-streaming batch mode: it may be one or multiple ObjectTrackingFrame
  // messages in frames.
  // Streaming mode: it can only be one ObjectTrackingFrame message in frames.
  repeated ObjectTrackingFrame frames = 2;

  // Different representation of tracking info in non-streaming batch
  // and streaming modes.
  oneof track_info {
    // Non-streaming batch mode ONLY.
    // Each object track corresponds to one video segment where it appears.
    VideoSegment segment = 3;
    // Streaming mode ONLY.
    // In streaming mode, we do not know the end time of a tracked object
    // before it is completed. Hence, there is no VideoSegment info returned.
    // Instead, we provide a unique identifiable integer track_id so that
    // the customers can correlate the results of the ongoing
    // ObjectTrackAnnotation of the same track_id over time.
    int64 track_id = 5;
  }
}
```

# Example output

```
Reading response.
Entity description: mirror
Track Id: 12
Entity id: /m/054_l
Confidence: 0.936193287373
Time offset of the first frame: 7.5075s
Bounding box position:
    left  : 0.0509275756776
    top   : 0.748813211918
    right : 0.218115285039
    bottom: 0.979368686676
```
