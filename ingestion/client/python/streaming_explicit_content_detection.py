#!/usr/bin/env python

# Copyright (c) 2019 Google LLC
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import argparse
import io

from google.cloud import videointelligence_v1p3beta1 as videointelligence

def streaming_annotate(stream_file):
    client = videointelligence.StreamingVideoIntelligenceServiceClient()

    # Set streaming config.
    config = videointelligence.types.StreamingVideoConfig(
        feature=(videointelligence.enums.StreamingFeature.
                 STREAMING_EXPLICIT_CONTENT_DETECTION))

    # config_request should be the first in the stream of requests.
    config_request = videointelligence.types.StreamingAnnotateVideoRequest(
        video_config=config)

    # Set the chunk size to 5MB (recommended less than 10MB).
    chunk_size = 5 * 1024 * 1024

    # Load file content.
    stream = []
    with io.open(stream_file, 'rb') as video_file:
        while True:
            data = video_file.read(chunk_size)
            if not data:
                break
            stream.append(data)

    def stream_generator():
      yield config_request
      for chunk in stream:
          yield videointelligence.types.StreamingAnnotateVideoRequest(
              input_content=chunk)

    requests = stream_generator()

    # streaming_annotate_video returns a generator.
    # timeout argument specifies the maximum allowable time duration between
    # the time that the last packet is sent to Google video intelligence API
    # and the time that an annotation result is returned from the API.
    # timeout argument is represented in number of seconds.
    responses = client.streaming_annotate_video(requests, timeout=3600)

    # Each response corresponds to about 1 second of video.
    for response in responses:
        # Check for errors.
        if response.error.message:
            print(response.error.message)
            break

        for frame in response.annotation_results.explicit_annotation.frames:
            time_offset = (frame.time_offset.seconds +
                           frame.time_offset.nanos / 1e9)
            pornography_likelihood = videointelligence.enums.Likelihood(
                frame.pornography_likelihood)

            print('Time: {}s'.format(time_offset))
            print('\tpornogaphy: {}'.format(pornography_likelihood.name))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument(
        'file_path', help='Local file location for streaming video annotation.')
    args = parser.parse_args()

    streaming_annotate(args.file_path)
