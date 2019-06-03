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

#ifndef API_VIDEO_CLIENT_CPP_VISUALIZER_UTIL_H_
#define API_VIDEO_CLIENT_CPP_VISUALIZER_UTIL_H_

#include "glog/logging.h"
#include "proto/video_intelligence_streaming.pb.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_syswm.h"
#include "SDL2/SDL_thread.h"
#include "SDL2/SDL_ttf.h"

namespace api {
namespace video {

// Utility function to update contents (bounding box, text, etc) that will be
// rendered on a video. Rendering contents will be extracted from `resp` and
// `sdl_renderer` will be updated.
void UpdateSDLRendererContent(
    const google::cloud::videointelligence::v1p3beta1::
        StreamingAnnotateVideoResponse& resp,
    const int video_width, const int video_height, TTF_Font* font_ptr,
    SDL_Renderer* sdl_renderer);

// Gets timestamp of an annotation response.
uint32_t GetAnnotationResponseTimestamp(
    const google::cloud::videointelligence::v1p3beta1::
        StreamingAnnotateVideoResponse& resp);

// Gets SDL_Renderer clear threshold depedning on feature type.
uint32_t GetRendererClearThreshold(
    const google::cloud::videointelligence::v1p3beta1::
        StreamingAnnotateVideoResponse& resp);

}  // namespace video
}  // namespace api

#endif  //  API_VIDEO_CLIENT_CPP_VISUALIZER_UTIL_H_
