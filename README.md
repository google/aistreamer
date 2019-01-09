AIStreamer - A Media Streaming/Processing Library for Google Cloud Video Intelligence
===================================

AIStreamer is an open source media streaming and processing library used in conjunction with [Google Cloud Video Intelligence Solution](https://cloud.google.com/video-intelligence).

[Google Cloud Video Intelligence Solution](https://cloud.google.com/video-intelligence) supports label detection, shot change detection, explicit content detection, object tracking and video transcription.

AIStreamer library is running on premise or in the clouds. It is used for preprocessing and streaming the media content to Google Cloud Video Intelligence servers.

AIStreamer is still an [Alpha feature](https://cloud.google.com/video-intelligence/alpha/docs/streaming). You must be in Google Cloud Video Intelligence Trusted Group to use this Alpha feature.

<table>
  <tr>
    <td><b>Homepage:</b></td>
    <td><a href="https://cloud.google.com/video-intelligence">Google Cloud Video Intelligence (Public Features)</a></td>
  </tr>
  <tr>
    <td><b>Homepage:</b></td>
    <td><a href="https://cloud.google.com/video-intelligence/alpha/docs/streaming">Google Cloud Video Intelligence (Alpha Features, Restricted Access)</a></td>
  </tr>
</table>

[![Join the chat at https://gitter.im/aistreamer/aistreamer](https://badges.gitter.im/aistreamer/aistreamer.svg)](https://gitter.im/aistreamer/aistreamer?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Contact us: [aistreamer@googlegroups.com](mailto:aistreamer@googlegroups.com)

# To start using AIStreamer

For instructions on how to use different media processing and streaming features,
please refer to the documents in individual feature directories.

 * [AIStreamer Ingestion](ingestion): Streaming ingestion pipeline for Google Video Intelligence. It supports both live streaming protocols (HLS/RTSP/RTMP) and file streaming.
 * [Video Frame Processor](vfp): A library for extracting video frames, converting format and visualizing annotations.


# License

All source code in this Github repository follows MIT license.

Copyright (c) 2018 Google LLC

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
