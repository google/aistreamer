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

"""Library for extracting frames."""
import os
import subprocess

def extract_video_frame(video_path, output_dir, fps=None):
  """ Extract frames from video file."""
  if not os.path.exists(video_path):
    raise Exception("video_path: {} is not found.".format(video_path))

  frame_info_command = ('ffprobe -v quiet -f lavfi -i '
                        '"movie={input_video}[out0]" -show_frames -show_entries'
                        ' frame=pkt_pts_time,pkt_pos -of csv=p=0').format(
                            input_video=video_path)
  frame_info_output = subprocess.check_output(frame_info_command, shell=True)
  frame_info_list = filter(lambda x: x.find(',') != -1,
                           frame_info_output.split('\n'))
  frame_info_dict = dict(
      frame_line.split(',')[::-1] for frame_line in frame_info_list)

  frame_output_dir = os.path.join(
      output_dir, os.path.basename(video_path.replace('.mp4', '')))
  if not os.path.exists(frame_output_dir):
    os.makedirs(frame_output_dir)

  if fps is not None:
    command = (
        'ffmpeg -hide_banner -i {input_video} -filter:v fps={fps},showinfo '
        '-vsync vfr {output_dir}/%013d.jpg').format(
            input_video=video_path, fps=fps, output_dir=frame_output_dir)
  else:
    command = ('ffmpeg -hide_banner -i {input_video} -filter:v showinfo -vsync '
               'vfr {output_dir}/%013d.jpg').format(
                   input_video=video_path, output_dir=frame_output_dir)

  direct_output = subprocess.check_output(
      command, shell=True, stderr=subprocess.STDOUT)

  f_list = direct_output.split('\n')
  ts_list = []
  for f in f_list:
    if f.find('Parsed_showinfo') != -1 and f.find('pos:') != -1:
      pos_start = f.find('pos:')
      pos_end = f.find('fmt:')
      pos = f[pos_start + 4:pos_end].replace(' ', '')
      ts = frame_info_dict[pos]
      ts_list.append(ts)

  image_file_list = sorted(os.listdir(frame_output_dir))
  if len(image_file_list) != len(ts_list):
    raise Exception(
        '{} contains files other than extraced frames. Please '
        'double check the dir and rerun the script.'.format(frame_output_dir))

  for ts, image_name in zip(ts_list, image_file_list):
    new_image_name = '%013d.jpg' % (float(ts) * 1e6)
    os.rename(
        os.path.join(frame_output_dir, image_name),
        os.path.join(frame_output_dir, new_image_name))
