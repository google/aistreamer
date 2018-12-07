AIStreamer Ingestion - Live Streaming
===================================

[Google Cloud Video Intelligence Streaming API](https://cloud.google.com/video-intelligence/alpha/docs/streaming) supports
standard live streaming protocols (RTSP and HLS). AIStreamer ingestion pipeline is behaved as a streaming proxy,
converting from live streaming procotols to bidirectional streaming gRPC connection.

To support live streaming protocols, we utilize [gStreamer](https://gstreamer.freedesktop.org) open media framework.

# Step 1: Create a named pipe

A named pipe is created to communicate between gStreamer and AIStreamer ingestion proxy. The two processes are running
inside the same Docker image.

```
$ export PIPE_NAME=/path_to_pipe/pipe_name
$ mkfifo PIPE_NAME
```

# Step 2: Run AIStreamer ingestion proxy

[C++ examples](../client/cpp) are available to our customers. A single binary is implemented to support all features.

Command line to run our example C++ code:

```
$ export GOOGLE_APPLICATION_CREDENTIALS=/path_to_credential/credential_json
$ export FEATURE=1
$ export PIPE_NAME=/path_to_pipe/pipe_name
$ export TIMEOUT=3600
$ ./streaming_client_main --alsologtostderr --endpoint "dns:///alpha-videointelligence.googleapis.com" \
      --video_path $PIPE_NAME --use_pipe=true --feature=$FEATURE --timeout=$TIMEOUT
```

Here, $GOOGLE_APPLICATION_CREDENTIALS specifies where GCP credential json file is located.

$FEATURE is the enum number of [StreamingFeature](../proto/video_intelligence_streaming.proto). For label detection, $FEATURE is 1,
for shot change detection, $FEATURE is 2.

Make sure to set correct timeout flag in the command line. If you need to stream 1 hour of video,
timeout value should be at least 3600 (unit: seconds).

# Step 3: Run gStreamer pipeline

gStreamer supports multiple live streaming protocols including but not limited to:

* HTTP Live Streaming (HLS)
* Real-time Streaming Protocol (RTSP)
* Real-time Protocol (RTP)
* Real-time Messaging Protocol (RTMP)
* WebRTC
* Streaming from Webcam

We use gStreamer pipeline to convert from these live streaming protocols to a decodable video stream, and writes the stream into
the named pipe we create in Step 1.

Here, we only provide examples for HLS, RTSP and RTMP. If you need other protocol support, please contact us.

## HTTP Live Streaming (HLS)

```
$ export PIPE_NAME=/path_to_pipe/pipe_name
$ export HLS_SOURCE=http://abc.def/playlist.m3u8
$ gst-launch-1.0 -v souphttpsrc location=$HLS_SOURCE ! hlsdemux ! filesink location=$PIPE_NAME
```

## Real-time Streaming Protocol (RTSP)

```
$ export PIPE_NAME=/path_to_pipe/pipe_name
$ export RTSP_SOURCE=rtsp://ip_addr:port/stream
$ gst-launch-1.0 -v rtspsrc location=$RTSP_SOURCE ! rtpjitterbuffer ! rtph264depay \
      ! h264parse ! mp4mux ! filesink location=$PIPE_NAME
```

## Real-time Message Protocol (RTMP)
```
$ export PIPE_NAME=/path_to_pipe/pipe_name
$ export RTMP_SOURCE=rtmp://host/app/stream
$ gst-launch-1.0 -v rtmpsrc location=$RTMP_SOURCE ! flvdemux ! flvmux ! filesink location=$PIPE_NAME
```

# Build instruction

We provide a Docker example, and build/deployment dependency rules have been properly configured in [env/Dockerfile](../env/Dockerfile).

Please refer to [Docker documentation](../documentation/kube.md) for detailed instructions.

# Flow control

Google Cloud Video Intelligence Streaming API server has inherent flow control. A StreamingAnnotateVideoRequest message will be rejected, and
gRPC streaming connection will be stopped immediately in the following two cases:

1. when AIStreamer ingestion client is sending requests to Google servers too frequently
2. when AIStreamer ingestion client is sending too much data to Google servers (beyond 20Mbytes per second).

# On-going effort

There is an on-going effort to combine gStreamer and AIStreamer ingestion proxy into a single binary. Expected delivery date will be Q1/2019.
