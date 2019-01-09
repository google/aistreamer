#!/usr/bin/env python

# Copyright (c) 2018 Google LLC
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

"""Streaming Storage sample.

This application demonstrates how to enable streaming annotation result storage
in GCS with the Google Cloud Video Intelligence API.

For more information, check out the documentation at
https://cloud.google.com/video-intelligence/docs.

Usage Example:
    $ python streaming_sample.py file_path.mp4 gs://bucket-id/object-id/

Sample Output:
    Reading response.
    Storage uri: gs://foo/bar/project-id/5c1b1ed6-0000-210c-ade5-089e082c1b6c

After processing, annotation results will be available under the provided gcs
bucket.
"""

import argparse

from google.cloud import videointelligence
from google.cloud.videointelligence import enums
from google.cloud.videointelligence import types


def stream(file_object, chunk_size):
  """Reads a file in chunks."""
  while True:
    data = file_object.read(chunk_size)
    if not data:
      break
    yield data


def streaming_annotate(stream_file, output_uri):
  """Annotate a local video file through streaming API."""

  client = videointelligence.StreamingVideoIntelligenceServiceClient()
  # Set the chunk size to 5MB (recommended less than 10MB).
  chunk_size = 5 * 1024 * 1024

  # Open file.
  with open(stream_file) as video_file:
    requests = (
      types.StreamingAnnotateVideoRequest(input_content=chunk)
      for chunk in stream(video_file, chunk_size))

    # Use storage config option in the config request to enable storage.
    storage_config = types.StreamingStorageConfig(
        enable_storage_annotation_result=True,
        annotation_result_storage_directory=output_uri)
    # LABEL_DETECTION feature is used as an example. Storage works for all
    # supported features.
    label_config = types.StreamingLabelDetectionConfig()
    config = types.StreamingVideoConfig(
        feature=enums.StreamingFeature.STREAMING_LABEL_DETECTION,
        label_detection_config=label_config,
        storage_config=storage_config)
    config_request = types.StreamingAnnotateVideoRequest(video_config=config)

    # streaming_annotate_video returns a generator.
    # timeout argument specifies the maximum allowable time duration between
    # the time that the last packet is sent to Google video intelligence API
    # and the time that an annotation result is returned from the API.
    # timeout argument is represented in number of seconds.
    responses = client.streaming_annotate_video(
        config_request, requests, timeout=10800)

    print '\nReading response.'
    # Retrieve results from the response generator.
    for response in responses:
      print 'Storage uri: {}'.format(response.annotation_results_uri)


if __name__ == '__main__':
  parser = argparse.ArgumentParser(
      description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument(
      'file_path', help='Local file location for streaming video annotation.')
  parser.add_argument(
      'output_uri',
      help='Storage uri (gs://bucket-id/object-id) to store annotation results.'
  )
  args = parser.parse_args()

  streaming_annotate(args.file_path, args.output_uri)