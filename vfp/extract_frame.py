#!/usr/bin/python
#
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

""" Extract frames from video file."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import argparse
import multiprocessing
import functools

import frame_lib


if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument(
      '-f',
      '--video_file_list_name',
      type=str,
      required=True,
      help='Specify a file_list, which should contains a list of video names. '
      'Each line should be one video file.')
  parser.add_argument('-r', '--fps', type=float, help='frame rate.')
  parser.add_argument(
      '-o',
      '--output_dir',
      type=str,
      required=True,
      help='output of the extracted frames.')
  parser.add_argument('-n', '--num_threads', type=int, default=5)

  args = parser.parse_args()

  if not os.path.exists(args.video_file_list_name):
    raise Exception("video_file_list: {} doesn't exist.".format(
        args.video_file_list_name))

  if os.path.exists(args.output_dir):
    raise Exception(
        'Output dir: {} already exists. Please specify another dir to avoid'
        ' overwritten.'.format(args.output_dir))

  with open(args.video_file_list_name, 'r') as f:
    file_string = f.read()
    file_list = file_string.split('\n')
    file_list = [f.replace('\r', '') for f in file_list if f.endswith('.mp4')]

  single_thread_fn = functools.partial(
      frame_lib.extract_video_frame, output_dir=args.output_dir, fps=args.fps)
  if args.num_threads == 1:
    for video_file in file_list:
      single_thread_fn(video_file)
  else:
    p = multiprocessing.Pool(args.num_threads)
    p.map(single_thread_fn, file_list)
