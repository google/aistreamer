AIStreamer Ingestion - File/Data Streaming
===================================

[Google Cloud Video Intelligence Streaming API](https://cloud.google.com/video-intelligence/alpha/docs/streaming) allows customers
to stream a video file to Google Streaming API server and receives analytics results in real time. It also allows customers to
divide a video file into multiple data chunks, and stream each data chunk individually.

# Supported video codecs

Not all video files can be supported in Google Cloud Video Intellgence Streaming API. Please refer to
[Google Cloud Documentation](https://cloud.google.com/video-intelligence/alpha/docs/streaming) for list of
supported video codecs during file streaming.

You can determine if your video file supports streaming by using a named pipe and the [FFMPEG](https://www.ffmpeg.org) tool.
The following sample commands decode a video into PNG images at a rate of 0.2 fps.
If your file can be successfully decoded using these commands, then you can stream it using the Video Intelligence streaming API.

```
$ export FILE_NAME=/path_to_file/file_name
$ mkfifo /tmp/inpipe
$ ffmpeg -i /tmp/inpipe -r 0.2 output_%04d.png
$ cat $FILE_NAME > /tmp/inpipe
```

# Python client library

[Python examples](../client/python) are available to our customers. We provide one Python file for each feature.

Command line to run our example Python code:

```
$ export GOOGLE_APPLICATION_CREDENTIALS=/path_to_credential/credential_json
$ ./streaming_label_detection.py $FILE_NAME
$ ./streaming_shot_detectin.py $FILE_NAME
$ ./streaming_explicit_content_detection.py $FILE_NAME
$ ./streaming_object_tracking.py $FILE_NAME
```

Here, $GOOGLE_APPLICATION_CREDENTIALS specifies where GCP credential json file is located.

Make sure to set correct timeout value when initiating the gRPC connection. If you need to stream 1 hour of video,
timeout value should be at least 3600 (unit: seconds) in the python source code.

```python
# streaming_annotate_video returns a generator.
# timeout argument specifies the maximum allowable time duration between
# the time that the last packet is sent to Google video intelligence API
# and the time that an annotation result is returned from the API.
# timeout argument is represented in number of seconds.
responses = client.streaming_annotate_video(config_request, requests, timeout=3600)
```

# C++ client library

[C++ examples](../client/cpp) are available to our customers. A single binary example [streaming_client_main](../client/cpp/BUILD)
is provided to support all features. Please follow the build instruction.

Command line to run our example C++ code:

```
$ export GOOGLE_APPLICATION_CREDENTIALS=/path_to_credential/credential_json
$ export CONFIG=/path_to_config/config_json
$ export PIPE_NAME=/path_to_pipe/pipe_name
$ export TIMEOUT=3600
$ ./streaming_client_main --alsologtostderr --endpoint "dns:///alpha-videointelligence.googleapis.com" \
      --video_path=$PIPE_NAME --use_pipe=true --config=$CONFIG --timeout=$TIMEOUT
```

Here, $GOOGLE_APPLICATION_CREDENTIALS specifies where GCP credential json file is located.

Example config files $CONFIG for each feature can be found [here](../client/cpp/config).

Make sure to set correct timeout flag in the command line. If you need to stream 1 hour of video,
timeout value should be at least 3600 (unit: seconds).

# Other languages

Support for other languages will be available by end of Q1/2019. Please contact us if you need immediate support before that time frame.

# Build instruction

We provide a Docker example, and build/deployment dependency rules have been properly configured in [env/Dockerfile](../env/Dockerfile).

Please refer to [Docker documentation](../documentation/docker.md) for detailed instructions.

# Flow control

Google Cloud Video Intelligence Streaming API server has inherent flow control. A StreamingAnnotateVideoRequest message will be rejected, and
gRPC streaming connection will be stopped immediately in the following two cases:

1. when AIStreamer ingestion client is sending requests to Google servers too frequently
2. when AIStreamer ingestion client is sending too much data to Google servers (beyond 20Mbytes per second).
