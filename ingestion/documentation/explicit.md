AIStreamer Ingestion Library - Live Explicit Content Detection
===================================

[Google Cloud Video Intelligence Streaming API](https://cloud.google.com/video-intelligence/alpha/docs/streaming) supports
live explicit content detection with streaming input of media data and streaming output of analysis results.

# Feature enum

[SteamingFeature](../proto/video_intelligence_streaming.proto) defines feature enum for live explicit content detection.

```c++
// Explicit content detection.
STREAMING_EXPLICIT_CONTENT_DETECTION = 3;
```

# Input config

[StreamingExplicitContentDetectionConfig](../proto/video_intelligence_streaming.proto) defines input configuration for live explicit content detection.

```c++
// Config for EXPLICIT_CONTENT_DETECTION in streaming mode.
message StreamingExplicitContentDetectionConfig {
  // No customized config support.
}
```

# Output analysis results

[ExplicitContentAnnotation](../proto/video_intelligence_streaming.proto) defines output analysis results for live explicit content detection.

```c++
message StreamingVideoAnnotationResults {
  // Explicit detection results.
  ExplicitContentAnnotation explicit_annotation = 3;
}

// Bucketized representation of likelihood.
enum Likelihood {
  // Unspecified likelihood.
  LIKELIHOOD_UNSPECIFIED = 0;
  // Very unlikely.
  VERY_UNLIKELY = 1;
  // Unlikely.
  UNLIKELY = 2;
  // Possible.
  POSSIBLE = 3;
  // Likely.
  LIKELY = 4;
  // Very likely.
  VERY_LIKELY = 5;
}

// Video frame level annotation results for explicit content.
message ExplicitContentFrame {
  // Time-offset, relative to the beginning of the video, corresponding to the
  // video frame for this location.
  google.protobuf.Duration time_offset = 1;

  // Likelihood of the pornography content..
  Likelihood pornography_likelihood = 2;
}

// Explicit content annotation (based on per-frame visual signals only).
// If no explicit content has been detected in a frame, no annotations are
// present for that frame.
message ExplicitContentAnnotation {
  // All video frames where explicit content was detected.
  repeated ExplicitContentFrame frames = 1;
}
```

# Example output

```
Reading response.
Time: 0.168794s
      pornography: VERY_UNLIKELY
Time: 1.069627s
      pornography: VERY_UNLIKELY
Time: 2.116857s
      pornography: VERY_UNLIKELY
Time: 3.082869s
```
