AIStreamer Ingestion Library - Live Shot Change Detection
===================================

[Google Cloud Video Intelligence Streaming API](https://cloud.google.com/video-intelligence/alpha/docs/streaming) supports
live shot change detection with streaming input of media data and streaming output of analysis results.

# Feature enum

[SteamingFeature](../proto/video_intelligence_streaming.proto) defines feature enum for live shot change detection.

```c++
// Shot change detection.
STREAMING_SHOT_CHANGE_DETECTION = 2;
```

# Input config

[StreamingShotChangeDetectionConfig](../proto/video_intelligence_streaming.proto) defines input configuration for live shot change detection.

```c++
// Config for SHOT_CHANGE_DETECTION in streaming mode.
message StreamingShotChangeDetectionConfig {
  // No customized config support.
}
```

# Output analysis results

[VideoSegment](../proto/video_intelligence_streaming.proto) defines output analysis results for live shot change detection.

```c++
message StreamingVideoAnnotationResults {
  // Shot annotation results. Each shot is represented as a video segment.
  repeated VideoSegment shot_annotations = 1;
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
```

# Example output

```
Reading response.
Shot: 0.0s to 4.170837s
Shot: 4.204204s to 7.107107s
Shot: 7.140473s to 11.778445s
Shot: 11.811811s to 17.28395s
Shot: 17.317317s to 20.82082s
Shot: 20.854187s to 24.591257s
Shot: 24.624624s to 30.063396s
```
