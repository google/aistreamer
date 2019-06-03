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

#include "client/cpp/media_player.h"

#include <algorithm>
#include <cmath>

namespace api {
namespace video {

// Invalid stream index.
constexpr int kInvalidStreamIndex = -1;
// SDL audio buffer (in bytes).
constexpr int kSdlAudioBufferSize = 4096;
// Max audio buffer size (in bytes).
constexpr int kMaxAudioBufferSize = 288000;
// Max encoded stream buffer size (in bytes).
constexpr int kMaxStreamBufferSize = 8192;
// Font size.
constexpr int kFontSize = 20;
// Random video path for piped input.
constexpr char kRandomVideoPath[] = "random";
// Max wait time in milliseconds before clearing outdated rendering text.
constexpr uint32_t kRendereClearThresholdLabel = 1500;
constexpr uint32_t kRendereClearThresholdTracking = 200;

// The below two global variables are nasty, but have to be there.
// They will be used in C-style callback.

// Data stream queue.
SyncQueue<std::string> stream_queue;

namespace {

using ::google::cloud::videointelligence::v1p3beta1::
    StreamingAnnotateVideoResponse;

void audio_callback(void* userdata, uint8_t* stream, int len) {
  SDL_memset(stream, 0, len);
}

int stream_callback(void* userdata, uint8_t* stream, int len) {
  std::string data;
  if (stream_queue.Size() > 0) {
    data = stream_queue.Pop();
  }
  if (data.size() > kMaxStreamBufferSize) {
    LOG(FATAL) << "Input data chunk cannot be larger than "
               << kMaxStreamBufferSize << " bytes!";
  }
  if (data.size() > 0) {
    memcpy(stream, data.c_str(), data.size());
    return data.size();
  }
  return 0;
}

}  // namespace

MediaPlayer::MediaPlayer(const std::string& font,
                         const std::string& video_path) {
  font_ = font;
  video_path_ = video_path;
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
    LOG(FATAL) << "unable to init SDL!";
  }
  TTF_Init();
  av_register_all();
}

MediaPlayer::MediaPlayer(const std::string& font) {
  font_ = font;
  video_path_ = kRandomVideoPath;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
    LOG(FATAL) << "unable to init SDL!";
  }
  TTF_Init();

  av_register_all();

  if ((av_format_ctx_ = avformat_alloc_context()) == nullptr) {
    LOG(FATAL) << "Error avformat_alloc_context!";
  }

  if ((stream_buffer_ = reinterpret_cast<uint8_t*>(
           av_malloc(kMaxStreamBufferSize))) == nullptr) {
    LOG(FATAL) << "Error av_malloc stream buffer!";
  }

  if ((av_io_ctx_ =
           avio_alloc_context(stream_buffer_, kMaxStreamBufferSize, 0, nullptr,
                              stream_callback, nullptr, nullptr)) == nullptr) {
    LOG(FATAL) << "Error avio_alloc_context!";
  }

  av_format_ctx_->pb = av_io_ctx_;
  av_format_ctx_->flags |= AVFMT_FLAG_CUSTOM_IO;
}

MediaPlayer::~MediaPlayer() {
  if (av_io_ctx_ != nullptr) {
    av_free(av_io_ctx_);
  }
  av_free(video_buffer_);
  av_free(frame_rgb_);
  av_free(frame_yuv_);
  avcodec_close(video_codec_ctx_);
  avformat_close_input(&av_format_ctx_);
}

void MediaPlayer::Init() {
  ReadAvCodec();
  InitAvChannel();
  InitSdlDisplay();
  PlayMedia();
}

void MediaPlayer::ReadAvCodec() {
  // Seeks stream info.

  if (avformat_open_input(&av_format_ctx_, video_path_.c_str(), nullptr,
                          nullptr) < 0) {
    LOG(FATAL) << "Unable to open input " << video_path_;
  }

  if (avformat_find_stream_info(av_format_ctx_, nullptr) < 0) {
    LOG(FATAL) << "Unable to find stream info!";
  }

  // Finds video and audio streams.

  audio_stream_id_ = kInvalidStreamIndex;
  video_stream_id_ = kInvalidStreamIndex;

  for (uint32_t i = 0; i < av_format_ctx_->nb_streams; ++i) {
    if (av_format_ctx_->streams[i]->codecpar->codec_type ==
        AVMEDIA_TYPE_VIDEO) {
      video_stream_id_ = i;
    }
    if (av_format_ctx_->streams[i]->codecpar->codec_type ==
        AVMEDIA_TYPE_AUDIO) {
      audio_stream_id_ = i;
    }
  }

  // Processes video codec.

  if (video_stream_id_ == kInvalidStreamIndex) {
    LOG(FATAL) << "We cannot support audio-only file.";
  }

  video_codec_params_ = av_format_ctx_->streams[video_stream_id_]->codecpar;
  if (audio_stream_id_ != -1) {
    audio_codec_params_ = av_format_ctx_->streams[audio_stream_id_]->codecpar;
  }

  if ((video_codec_ = avcodec_find_decoder(video_codec_params_->codec_id)) ==
      nullptr) {
    LOG(FATAL) << "Open bad video codec: avcodec_find_decoder!";
  }
  if ((video_codec_ctx_ = avcodec_alloc_context3(video_codec_)) == nullptr) {
    LOG(FATAL) << "Open bad video codec: avcodec_alloc_context3!";
  }
  if (avcodec_parameters_to_context(video_codec_ctx_, video_codec_params_) <
      0) {
    avformat_close_input(&av_format_ctx_);
    avcodec_free_context(&video_codec_ctx_);
    LOG(FATAL) << "Open bad video codec: avcodec_parameters_to_context!";
  }
  if (avcodec_open2(video_codec_ctx_, video_codec_, nullptr) < 0) {
    LOG(FATAL) << "Open bad video codec: avcodec_open2";
  }

  // Processes audio codec.

  if (audio_stream_id_ == kInvalidStreamIndex) {
    return;
  }

  // LOG(INFO) << "This stream contains audio track.";
  if ((audio_codec_ = avcodec_find_decoder(audio_codec_params_->codec_id)) ==
      nullptr) {
    LOG(FATAL) << "Open bad audio codec: avcodec_find_decoder!";
  }
  if ((audio_codec_ctx_ = avcodec_alloc_context3(audio_codec_)) == nullptr) {
    LOG(FATAL) << "Open bad audio codec: avcodec_alloc_context3!";
  }
  if (avcodec_parameters_to_context(audio_codec_ctx_, audio_codec_params_) <
      0) {
    avformat_close_input(&av_format_ctx_);
    avcodec_free_context(&audio_codec_ctx_);
    avcodec_free_context(&video_codec_ctx_);
    LOG(FATAL) << "Open bad audio codec: avcodec_parameters_to_context!";
  }
  if (avcodec_open2(audio_codec_ctx_, audio_codec_, nullptr) < 0) {
    LOG(FATAL) << "Open bad audio codec: avcodec_open2";
  }
}

void MediaPlayer::InitAvChannel() {
  InitVideoChannel();
  if (audio_stream_id_ != kInvalidStreamIndex) {
    InitAudioChannel();
  }
}

void MediaPlayer::InitVideoChannel() {
  if ((frame_yuv_ = av_frame_alloc()) == nullptr) {
    LOG(FATAL) << "Unable to allocate memory for a YUV frame!";
  }
  if ((frame_rgb_ = av_frame_alloc()) == nullptr) {
    LOG(FATAL) << "Unable to allocate memory for a RGB frame!";
  }

  int video_size = av_image_get_buffer_size(
      AV_PIX_FMT_RGB24, video_codec_ctx_->width, video_codec_ctx_->height, 1);
  video_buffer_ = (uint8_t*)av_malloc(video_size * sizeof(uint8_t));

  if ((av_image_fill_arrays(frame_rgb_->data, frame_rgb_->linesize,
                            video_buffer_, AV_PIX_FMT_RGB24,
                            video_codec_ctx_->width, video_codec_ctx_->height,
                            1) < 0)) {
    LOG(FATAL) << "Unable to perform av_image_fill_arrays!";
  }
}

void MediaPlayer::InitSdlDisplay() {
  sdl_window_ =
      SDL_CreateWindow("AIStreamer Live Video Player", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, video_codec_ctx_->width,
                       video_codec_ctx_->height, SDL_WINDOW_OPENGL);
  if (!sdl_window_) {
    LOG(FATAL) << "Unable to initialize SDL window!";
  }

  if (!(sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, 0))) {
    LOG(FATAL) << "Unable to create SDL renderer!";
  }

  sdl_texture_ = SDL_CreateTexture(
      sdl_renderer_, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STATIC,
      video_codec_ctx_->width, video_codec_ctx_->height);
  if (!sdl_texture_) {
    LOG(FATAL) << "Unable to create SDL texture!";
  }
}

void MediaPlayer::PlayMedia() {
  AVPacket pkt;

  sws_ctx_ = sws_getContext(video_codec_ctx_->width, video_codec_ctx_->height,
                            video_codec_ctx_->pix_fmt, video_codec_ctx_->width,
                            video_codec_ctx_->height, AV_PIX_FMT_RGB24,
                            SWS_BILINEAR, nullptr, nullptr, nullptr);
  if (sws_ctx_ == nullptr) {
    LOG(ERROR) << "Unable to call sws_getContext!";
  }

  if ((font_ptr_ = TTF_OpenFont(font_.c_str(), kFontSize)) == nullptr) {
    LOG(FATAL) << "Unable to read fonts: " << font_;
  }

  SDL_Event evt;
  // All timestamps are represented in milliseconds.
  uint32_t local_start_time, local_current_time;
  uint32_t video_start_time, video_current_time;

  uint64_t frame_count = 0;
  bool future_read = true;

  StreamingAnnotateVideoResponse cur_resp, future_resp;
  uint32_t last_updated_resp_offset;
  while (av_read_frame(av_format_ctx_, &pkt) >= 0) {
    if (pkt.stream_index == audio_stream_id_) {
      // Does nothing for now. We are not playing audio.
    }
    if (pkt.stream_index == video_stream_id_) {
      if (avcodec_send_packet(video_codec_ctx_, &pkt) < 0) {
        LOG(ERROR) << "Unable to send video packet to decoder!";
        continue;
      }
      if (avcodec_receive_frame(video_codec_ctx_, frame_yuv_) < 0) {
        LOG(ERROR) << "Unable to receive YUV frame!";
        continue;
      }
      AVRational time_base =
          av_format_ctx_->streams[video_stream_id_]->time_base;
      int64_t pts = av_frame_get_best_effort_timestamp(frame_yuv_);
      video_current_time =
          static_cast<uint32_t>(pts * av_q2d(time_base) * 1000);
      if (frame_count == 0) {
        video_start_time = video_current_time;
      }
      uint32_t video_offset = video_current_time - video_start_time;

      // Updates video frames.
      SDL_UpdateYUVTexture(sdl_texture_, nullptr, frame_yuv_->data[0],
                           frame_yuv_->linesize[0], frame_yuv_->data[1],
                           frame_yuv_->linesize[1], frame_yuv_->data[2],
                           frame_yuv_->linesize[2]);
      SDL_RenderCopy(sdl_renderer_, sdl_texture_, nullptr, nullptr);

      // Updates future rendering response.
      if (annotation_response_queue_.Size() > 0) {
        if (future_read) {
          future_resp = annotation_response_queue_.Pop();
          future_read = false;
        }
        if (future_resp.has_annotation_results() &&
            GetAnnotationResponseTimestamp(future_resp) <= video_offset) {
          cur_resp = future_resp;
          future_read = true;
          last_updated_resp_offset = video_offset;
        }
      }
      // If renderer hasn't been updated with new values for too long, clear
      // renderer.
      if (cur_resp.has_annotation_results() &&
          video_offset - last_updated_resp_offset <
              GetRendererClearThreshold(cur_resp)) {
        UpdateSDLRendererContent(cur_resp, video_codec_ctx_->width,
                                 video_codec_ctx_->height, font_ptr_,
                                 sdl_renderer_);
      }

      // Renders video on window.
      if (frame_count == 0) {
        local_start_time = SDL_GetTicks();
      } else {
        local_current_time = SDL_GetTicks();
        int32_t wait_time =
            video_offset - (local_current_time - local_start_time);
        if (wait_time > 0) {
          SDL_Delay(wait_time);
        }
      }

      SDL_RenderPresent(sdl_renderer_);
      SDL_UpdateWindowSurface(sdl_window_);

      ++frame_count;
    }
    SDL_PollEvent(&evt);
  }
}

void MediaPlayer::InitAudioChannel() {
  if ((swr_ctx_ = swr_alloc()) == nullptr) {
    LOG(FATAL) << "Unable to load audio channel!";
  }

  av_opt_set_channel_layout(swr_ctx_, "in_channel_layout",
                            audio_codec_ctx_->channel_layout, 0);
  av_opt_set_channel_layout(swr_ctx_, "out_channel_layout",
                            audio_codec_ctx_->channel_layout, 0);
  av_opt_set_int(swr_ctx_, "in_sample_rate", audio_codec_ctx_->sample_rate, 0);
  av_opt_set_int(swr_ctx_, "out_sample_rate", audio_codec_ctx_->sample_rate, 0);
  av_opt_set_sample_fmt(swr_ctx_, "in_sample_fmt", audio_codec_ctx_->sample_fmt,
                        0);
  av_opt_set_sample_fmt(swr_ctx_, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);

  if (swr_init(swr_ctx_) != 0) {
    LOG(FATAL) << "Unable to initialize audio channel!";
  }

  SDL_AudioSpec request = {0};
  SDL_AudioSpec obtain = {0};

  memset(&request, 0, sizeof(request));
  request.channels = audio_codec_ctx_->channels;
  request.freq = audio_codec_ctx_->sample_rate;
  request.format = AUDIO_S16SYS;
  request.silence = 0;
  request.samples = kSdlAudioBufferSize;
  request.userdata = audio_codec_ctx_;
  request.callback = audio_callback;

  if (SDL_OpenAudio(&request, &obtain) < 0) {
    LOG(FATAL) << "Unable to open audio!";
  }

  frame_audio_.format = AV_SAMPLE_FMT_S16;
  frame_audio_.sample_rate = obtain.freq;
  frame_audio_.channel_layout = av_get_default_channel_layout(obtain.channels);
  frame_audio_.channels = obtain.channels;

  InitAudioQueue(&audio_queue_);
  SDL_PauseAudio(0);
}

void MediaPlayer::InitAudioQueue(AudioQueue* q) {
  q->last = NULL;
  q->first = NULL;
  q->mutex = SDL_CreateMutex();
  q->cond = SDL_CreateCond();
}

void MediaPlayer::InsertStreamData(std::string data) {
  int num_chunks =
      std::ceil(static_cast<double>(data.size()) / kMaxStreamBufferSize);
  for (int i = 0; i < num_chunks; ++i) {
    int start = i * kMaxStreamBufferSize;
    int rest = data.size() - start;
    int size = (kMaxStreamBufferSize > rest) ? rest : kMaxStreamBufferSize;
    if (size > 0) {
      std::string tmp = data.substr(start, size);
      stream_queue.Push(tmp);
    }
  }
}

void MediaPlayer::InsertAnnotationResponse(
    StreamingAnnotateVideoResponse annotation_response) {
  annotation_response_queue_.Push(annotation_response);
}

}  // namespace video
}  // namespace api
