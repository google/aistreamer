VFP - A Video Frame Processor Client Library
===================================

This library is aiming to help people extracting frames, converting format and
visualizing annotations.

# Install dependency
## Install python dependency:
pip install -r requirements.txt

## Install ffmpeg:
Please follow [official documentation](https://www.ffmpeg.org/) for installation.

## Recommended video annotation tool VATIC:
[VATIC](https://github.com/opencv/cvat) is a free, online, interactive video and image annotation tool for computer vision. Please see the [official documentations](https://github.com/opencv/cvat) for more details.

# Library Functions
If you are using [VATIC](https://github.com/opencv/cvat) for annotation, we provide a script to convert the
annotation xml file to csv file. If you are developing your own annotation tools, we
provide a script to extract the correct timestamp. After you finished the
annotation and generated the csv file, it is highly recommended to use the visualization
script to double check the format.

## Option 1, Using VATIC for annotation.
[VATIC](https://github.com/opencv/cvat) will generate a xml file to store the annotations.
You can convert the generated xml file to csv file with the following command:

Example usage:
```python
./convert_xml_to_csv.py -v video.mp4 -f annotation.xml -o annotation.csv
```

-v: video_path. It is required to find the timestamp.

-f: xml_file_path. it is generated from annotation software.

-o: output path of converted csv file.

## Option 2, Using your own annotation tools.
If you are developing your own annotation tools and don't have a good way to generate timestamp, it is suggested to use the following command. It extracts frames based on the frame rate you specified and name each frame with it's timestamp. It only extracts key frames in the video so the timestamp will be rounding up to the next key frame's timestamp.

Example usage:
```python
./extract_frame.py -f file_list -o /tmp/extracted_frame [-r 2 -n 5]
```

-f: Specify a file_list, which should contains a list of video names. Each line
should be one video file. It should be full path or relative to where you executing the script. i.e.

videos/1532444422444.mp4<br>
videos/1532445683321.mp4

-o: Output dir. The frame will be named with it's timestamp in microsecond.

-r: [Optional] Frame rate. i.e. 2 means 2 frame per second. If not specified, it will extract all the frames.

-n: [Optional] Number of threads. Default is 5.

## Visualize annotation csv file:
If you want to verify if the csv file you generated are in standard format, please use the following command. It will generate a directory which contains the images overlaid with bounding boxes.

Example usage:
```python
./visualize_annotation.py -f annotation.csv -v video_dir -o /tmp/visualize [-m, -n 5 --force]
```

-f: Annotation.csv: Annotation file.

-v: The directory that contains the video files.

-o: The output directory for storing visualization files.

-m: [Optional] Specify the timestamp in csv file is in millisecond. Default is in
microsecond.

-n: [Optional] Number of threads to visualize_annotation. Default is 5.

--force: [Optional] Force recreating the visualization in visualize_output_dir.
