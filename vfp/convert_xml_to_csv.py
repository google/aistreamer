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

""" Convert xml file to csv file. """
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import argparse
import os
import subprocess
import xml.etree.ElementTree


def get_video_frame_info_list(video_path):
  """Get video frame info list. """

  if not os.path.exists(video_path):
    raise Exception("video_path: {} doesn't exists.".format(args.csv_file_path))

  frame_info_command = ('ffprobe -v quiet -f lavfi -i '
                        '"movie={input_video}[out0]" -show_frames -show_entries'
                        ' frame=pkt_pts_time -of csv=p=0').format(
                            input_video=video_path)

  frame_info_output = subprocess.check_output(frame_info_command, shell=True)
  frame_info_list = frame_info_output.split('\n')
  if frame_info_list[-1] == '':
    del frame_info_list[-1]

  frame_info_list = [
      '{:013d}'.format(int(float(frame) * 1e6)) for frame in frame_info_list
  ]

  return frame_info_list


def parse_xml(xml_file_path, video_base_name):
  """Parsing the xml file. """

  root = xml.etree.ElementTree.parse(xml_file_path).getroot()
  image_height = int(
      root.find('meta').find('task').find('original_size').find('height').text)
  image_width = int(
      root.find('meta').find('task').find('original_size').find('width').text)
  annotation_list = []
  for track in root.iter('track'):
    for box in track.iter('box'):
      label = track.get('label')
      if label.find(',') != -1:
        print(
            'label: {} contains comma, which is not allowed. The comma will be'
            ' removed in the csv file.'.format(label))
        label = label.replace(',', '')

      line = [
          video_base_name,
          int(box.get('frame')), label,
          float(box.get('xtl')) / image_width,
          float(box.get('ytl')) / image_height,
          float(box.get('xbr')) / image_width,
          float(box.get('ybr')) / image_height
      ]
      annotation_list.append(line)
  return annotation_list


if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument(
      '-v',
      '--video_path',
      type=str,
      required=True,
      help='video path. It is required to find the timestamp.')
  parser.add_argument(
      '-f',
      '--xml_file_path',
      type=str,
      required=True,
      help='xml file generated from annotation software.')
  parser.add_argument(
      '-o',
      '--output_file_path',
      type=str,
      required=True,
      help='output path of converted csv file.')

  args = parser.parse_args()

  frame_info_list = get_video_frame_info_list(args.video_path)
  annotation_list = parse_xml(args.xml_file_path,
                              os.path.basename(args.video_path))

  for annotation in annotation_list:
    annotation[1] = frame_info_list[annotation[1]]

  annotation_string = '\n'.join(
      [','.join(map(str, annotation)) for annotation in annotation_list])

  with open(args.output_file_path, 'w') as f:
    f.write(annotation_string)
