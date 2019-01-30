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

"""Streaming object tracking annotation sample.

This application demonstrates how to perform streaming object trcaking with the
Google Cloud Video Intelligence API.

For more information, check out the documentation at
https://cloud.google.com/video-intelligence/docs.

Usage Example:
    $ python streaming_object_tracking.py file_path.mp4

Sample Output:
    Reading response.
    Entity description: mirror
    Track Id: 12
    Entity id: /m/054_l
    Confidence: 0.936193287373
    Time: 7.5075s
    Bounding box position:
            left  : 0.0509275756776
            top   : 0.748813211918
            right : 0.218115285039
            bottom: 0.979368686676
"""
from __future__ import print_function

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


def streaming_annotate(stream_file):
  """Annotate a local video file through streaming API."""

  client = videointelligence.StreamingVideoIntelligenceServiceClient()

  # Set the chunk size to 5MB (recommended less than 10MB).
  chunk_size = 5 * 1024 * 1024

  # Open file.
  with open(stream_file) as video_file:
    requests = (
      types.StreamingAnnotateVideoRequest(input_content=chunk)
      for chunk in stream(video_file, chunk_size))

    # Set streaming config.
    config = types.StreamingVideoConfig(
        feature=enums.StreamingFeature.STREAMING_OBJECT_TRACKING)
    config_request = types.StreamingAnnotateVideoRequest(video_config=config)
    # streaming_annotate_video returns a generator.
    # timeout argument specifies the maximum allowable time duration between
    # the time that the last packet is sent to Google video intelligence API
    # and the time that an annotation result is returned from the API.
    # timeout argument is represented in number of seconds.
    responses = client.streaming_annotate_video(
        config_request, requests, timeout=10800)

    print('\nReading response.')
    # Retrieve results from the response generator.
    for response in responses:
      object_annotations = response.annotation_results.object_annotations

      # When object_annotations is empty, no object is found.
      if object_annotations:
        for annotation in object_annotations:
          description = annotation.entity.description
          confidence = annotation.confidence
          track_id = annotation.track_id

          print('Entity description: {}'.format(description))
          print('Track Id: {}'.format(track_id))
          if annotation.entity.entity_id:
            print('Entity id: {}'.format(annotation.entity.entity_id))

          print('Confidence: {}'.format(confidence))

          # In streaming mode, len(annotation.frames) is always 1, and the frames
          # in the same response share the same time_offset.
          frame = annotation.frames[0]
          box = frame.normalized_bounding_box
          print('Time: {}s'.format(
              frame.time_offset.seconds + frame.time_offset.nanos / 1e9))
          print('Bounding box position:')
          print('\tleft  : {}'.format(box.left))
          print('\ttop   : {}'.format(box.top))
          print('\tright : {}'.format(box.right))
          print('\tbottom: {}'.format(box.bottom))
          print('\n')


if __name__ == '__main__':
  parser = argparse.ArgumentParser(
      description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument(
      'file_path', help='Local file location for streaming video annotation.')
  args = parser.parse_args()

  streaming_annotate(args.file_path)
