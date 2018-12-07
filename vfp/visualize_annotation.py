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

""" Visualize Annotations. """
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import csv
import collections
import argparse
import subprocess
import bisect
import shutil
import multiprocessing
import functools

import cv2
import pandas as pd

import frame_lib

# Cache for video sequence timestamp.
video_time_sequence = {}


def read_csv(file_path):
  file_list = []
  with open(file_path, "r") as f:
    file_reader = csv.reader(f, delimiter=",")
    for row in file_reader:
      file_list.append(row)
  return file_list


def group_box(file_name):
  csv_file = pd.read_csv(file_name, header=None)
  merged_dict = collections.defaultdict(dict)
  video_name_group = csv_file.groupby(0)
  for video_group_name, video_group_value in video_name_group:
    merged_dict[video_group_name] = {}
    timestamp_group = video_group_value.groupby(1)
    for (timestamp_group_name, timestamp_group_value) in timestamp_group:
      merged_dict[video_group_name][
          timestamp_group_name] = timestamp_group_value.values
  return merged_dict


def get_time_sequence_path(video_name, visualize_output_dir):
  return os.path.join(visualize_output_dir, "tmp_all_frames",
                      os.path.basename(video_name).replace(".mp4", ""))


def get_timestamp_sequence(video_name, visualize_output_dir):
  global video_time_sequence
  if video_name in video_time_sequence:
    return video_time_sequence[video_name]

  output_dir = get_time_sequence_path(video_name, visualize_output_dir)
  frame_list = sorted(os.listdir(output_dir))
  video_time_sequence[video_name] = [
      int(f.replace(".jpg", "")) for f in frame_list
  ]

  return video_time_sequence[video_name]


def extract_frames(video_name, timestamp, ts_group_array, video_dir,
                   visualize_output_dir, is_millisecond):
  bboxes = ts_group_array[:, 3:7]

  video_path = os.path.join(video_dir, video_name)
  timestamp_sequence = get_timestamp_sequence(video_path, visualize_output_dir)

  if is_millisecond:
    timestamp *= 1e3

  index = bisect.bisect(timestamp_sequence, timestamp)
  if index == 0:
    timestamp = timestamp_sequence[0]
  elif index == len(timestamp_sequence):
    timestamp = timestamp_sequence[-1]
  else:
    if timestamp - timestamp_sequence[
        index - 1] < timestamp_sequence[index] - timestamp:
      timestamp = timestamp_sequence[index - 1]
    else:
      timestamp = timestamp_sequence[index]

  output_dir = os.path.join(visualize_output_dir, video_name.replace(
      ".mp4", ""))
  if not os.path.exists(output_dir):
    os.makedirs(output_dir)

  output_file_name = os.path.join(output_dir, "{:013d}".format(
      int(timestamp))) + ".jpg"

  time_sequence_path = get_time_sequence_path(video_name, visualize_output_dir)
  input_file_name = os.path.join(time_sequence_path,
                                 "{:013d}.jpg".format(timestamp))

  if not os.path.exists(input_file_name):
    print("{} Cannot be generated.".format(input_file_name))
    return

  image = cv2.imread(input_file_name)
  image_height, image_width, _ = image.shape
  for bbox in bboxes:
    bbox = [
        bbox[0] * image_width, bbox[1] * image_height, bbox[2] * image_width,
        bbox[3] * image_height
    ]
    bbox = [int(b) for b in bbox]
    image = cv2.rectangle(
        image, (bbox[0], bbox[1]), (bbox[2], bbox[3]), (0, 255, 0), thickness=2)
    cv2.imwrite(output_file_name, image)


def extract_all_frames(video_dir, video_name, visualize_output_dir):
  output_dir = os.path.join(visualize_output_dir, "tmp_all_frames")
  if not os.path.exists(output_dir):
    os.makedirs(output_dir)

  video_path = os.path.join(video_dir, video_name)
  frame_lib.extract_video_frame(video_path, output_dir)


def visualize_annotation_per_video(video_name, merged_dict,
                                   visualize_output_dir, is_millisecond):
  print('Processing {} ... '.format(video_name))
  extract_all_frames(args.video_dir, video_name, args.visualize_output_dir)
  for timestamp in merged_dict[video_name]:
    extract_frames(video_name, timestamp, merged_dict[video_name][timestamp],
                   args.video_dir, args.visualize_output_dir,
                   args.is_millisecond)
  time_sequence_path = get_time_sequence_path(video_name,
                                              args.visualize_output_dir)
  shutil.rmtree(time_sequence_path)
  print('Finished processing {}.'.format(video_name))


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("-f", "--csv_file_path", type=str, required=True)
  parser.add_argument("-v", "--video_dir", type=str, required=True)
  parser.add_argument("-o", "--visualize_output_dir", type=str, required=True)
  parser.add_argument("-m", "--is_millisecond", action="store_true")
  parser.add_argument("-n", "--num_threads", type=int, default=5)
  parser.add_argument(
      "--force",
      action="store_true",
      help="force removing the existing visualize_output_dir")
  args = parser.parse_args()

  if not os.path.exists(args.csv_file_path):
    raise Exception("csv_file_path {} doesn't exist".format(args.csv_file_path))

  if not os.path.exists(args.video_dir):
    raise Exception("video_dir {} doesn't exist".format(args.video_dir))

  if args.force and os.path.exists(args.visualize_output_dir):
    shutil.rmtree(args.visualize_output_dir)

  if not os.path.exists(args.visualize_output_dir):
    os.makedirs(args.visualize_output_dir)

  merged_dict = group_box(args.csv_file_path)

  single_thread_fn = functools.partial(
      visualize_annotation_per_video,
      merged_dict=merged_dict,
      visualize_output_dir=args.visualize_output_dir,
      is_millisecond=args.is_millisecond)

  if args.num_threads == 1:
    for video_name in merged_dict:
      single_thread_fn(video_name)
  else:
    p = multiprocessing.Pool(args.num_threads)
    p.map(single_thread_fn, merged_dict.keys())

  # Remove a tmp path.
  tmp_frame_path = os.path.join(args.visualize_output_dir, "tmp_all_frames")
  if os.path.exists(tmp_frame_path):
    shutil.rmtree(tmp_frame_path)
