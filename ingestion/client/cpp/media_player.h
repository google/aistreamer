// Copyright (c) 2019 Google LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef API_VIDEO_CLIENT_CPP_MEDIA_PLAYER_H_
#define API_VIDEO_CLIENT_CPP_MEDIA_PLAYER_H_

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libpostproc/postprocess.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include <vector>

#include "client/cpp/sync_queue.h"
#include "client/cpp/visualizer_util.h"
#include "glog/logging.h"
#include "proto/video_intelligence_streaming.pb.h"
#include "proto/visualizer.pb.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_audio.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_syswm.h"
#include "SDL2/SDL_thread.h"
#include "SDL2/SDL_ttf.h"

#define SDL_MAIN_HANDLED

namespace api {
namespace video {

typedef struct AudioQueue {
  AVPacketList* first;
  AVPacketList* last;
  int nb_packets;
  int size;
  SDL_mutex* mutex;
  SDL_cond* cond;
} AudioQueue;

class MediaPlayer {
 public:
  // Constructor and destructor.
  MediaPlayer(const std::string& font, const std::string& video_path);
  MediaPlayer(const std::string& font);
  ~MediaPlayer();

  // Inits all necessary components for media player.
  void Init();

  // Reads video and audio codec parameters.
  void ReadAvCodec();

  // Inits video and audio channel.
  void InitAvChannel();

  // Inits SDL player.
  void InitSdlDisplay();

  // Starts playing video.
  void PlayMedia();

  // Inserts stream, label, object bounding box.
  void InsertStreamData(std::string data);

  // Inserts annotation response to queue.
  void InsertAnnotationResponse(
      google::cloud::videointelligence::v1p3beta1::
          StreamingAnnotateVideoResponse annotation_response);

 private:
  // Stream callback function.
  int StreamCallback(void* userdata, uint8_t* stream, int len);

  // Inits video and audio channel.
  void InitVideoChannel();
  void InitAudioChannel();

  // Inits audio queue and player.
  void InitAudioQueue(AudioQueue* q);

  // Video and audio stream index.
  int video_stream_id_;
  int audio_stream_id_;

  // Video and audio format context.
  AVFormatContext* av_format_ctx_ = nullptr;

  // Video and audio IO context.
  AVIOContext* av_io_ctx_ = nullptr;

  // Video and audio codec parameters.
  AVCodecParameters* video_codec_params_ = nullptr;
  AVCodecParameters* audio_codec_params_ = nullptr;

  // Video and audio codec context.
  AVCodecContext* video_codec_ctx_ = nullptr;
  AVCodecContext* audio_codec_ctx_ = nullptr;

  // Video and audio codec.
  AVCodec* video_codec_ = nullptr;
  AVCodec* audio_codec_ = nullptr;

  // Video frame.
  AVFrame* frame_yuv_ = nullptr;
  AVFrame* frame_rgb_ = nullptr;

  // Video and encoded stream buffer.
  uint8_t* video_buffer_ = nullptr;
  uint8_t* stream_buffer_ = nullptr;

  // Audio frame,
  AVFrame frame_audio_;

  // Audio queue.
  AudioQueue audio_queue_;

  // SWS (video) & SWR (audio) context.
  struct SwsContext* sws_ctx_ = nullptr;
  struct SwrContext* swr_ctx_ = nullptr;

  // SDL player window controller.
  SDL_Window* sdl_window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;
  SDL_Texture* sdl_texture_ = nullptr;

  // SDL TTF front.
  std::string font_;
  TTF_Font* font_ptr_ = nullptr;

  // Video path.
  std::string video_path_;

  // Synchronous queue.
  SyncQueue<
      google::cloud::videointelligence::v1p3beta1::
      StreamingAnnotateVideoResponse>
      annotation_response_queue_;
};

}  // namespace video
}  // namespace api

#endif  //  API_VIDEO_CLIENT_CPP_MEDIA_PLAYER_H_
