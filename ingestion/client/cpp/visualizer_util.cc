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

#include "client/cpp/visualizer_util.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>
#include <utility>

namespace api {
namespace video {

namespace {

using ::google::cloud::videointelligence::v1p3beta1::LabelAnnotation;
using ::google::cloud::videointelligence::v1p3beta1::ObjectTrackingAnnotation;
using ::google::cloud::videointelligence::v1p3beta1::
    StreamingAnnotateVideoResponse;
using ::google::cloud::videointelligence::v1p3beta1::
    StreamingVideoAnnotationResults;

// Maximum number of displayed labels.
constexpr int kNumDisplayLabel = 8;
// Max wait time in milliseconds before clearing outdated rendering content.
constexpr uint32_t kRendererClearThresholdLabel = 1500;
constexpr uint32_t kRendererClearThresholdTracking = 200;

bool IsLabelDetectionResponse(const StreamingAnnotateVideoResponse& resp) {
  return (resp.has_annotation_results() &&
          resp.annotation_results().label_annotations_size() > 0);
}

bool IsObjectTrackingResponse(const StreamingAnnotateVideoResponse& resp) {
  return (resp.has_annotation_results() &&
          resp.annotation_results().object_annotations_size() > 0);
}

bool CompareLabel(std::pair<std::string, double> label_1,
                  std::pair<std::string, double> label_2) {
  return (label_1.second > label_2.second);
}

bool IsVisualizableLabel(const std::string& label) {
  for (const char& c : label) {
    if (!std::isalpha(c)) {
      return false;
    }
  }
  return true;
}

void UpdateLabelDetectionSDLRenderer(const StreamingAnnotateVideoResponse& resp,
                                     TTF_Font* font_ptr,
                                     SDL_Renderer* sdl_renderer) {
  std::vector<std::pair<std::string, double>> labels;
  for (const LabelAnnotation& annotation :
       resp.annotation_results().label_annotations()) {
    std::pair<std::string, double> label;
    label.first = annotation.entity().description();
    if (!IsVisualizableLabel(label.first)) {
      continue;
    }
    label.second = annotation.frames(0).confidence();

    labels.push_back(label);
  }

  // Sort labels with confidence score in descending order.
  std::sort(labels.begin(), labels.end(), CompareLabel);

  // Find Top-K labels with highest confidence scores and concatenate them
  std::string display_labels;
  int display_label_count = 0;
  for (const auto& label : labels) {
    if (display_label_count++ == kNumDisplayLabel) {
      break;
    }
    const std::string& label_name = label.first;
    if (display_labels.empty()) {
      display_labels = label_name;
    } else {
      display_labels += ", " + label_name;
    }
  }

  // Updates SDL_Renderer
  SDL_Color txt_color = {255, 165, 0};  // Color is orange.
  SDL_Surface* surface =
      TTF_RenderText_Solid(font_ptr, display_labels.c_str(), txt_color);
  if (surface != nullptr) {
    SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl_renderer, surface);
    if (texture != nullptr) {
      SDL_Rect label_rect = {10, 10, surface->w, surface->h};
      SDL_RenderCopy(sdl_renderer, texture, nullptr, &label_rect);
    }
    SDL_FreeSurface(surface);
  }
}

void UpdateObjectTrackingSDLRenderer(const StreamingAnnotateVideoResponse& resp,
                                     const int video_width,
                                     const int video_height,
                                     TTF_Font* font_ptr,
                                     SDL_Renderer* sdl_renderer) {
  std::vector<SDL_Rect> box_rects, box_text_rects;
  std::vector<SDL_Texture*> textures;
  for (const ObjectTrackingAnnotation& object_annotation :
       resp.annotation_results().object_annotations()) {
    float left = object_annotation.frames(0).normalized_bounding_box().left();
    float right = object_annotation.frames(0).normalized_bounding_box().right();
    float top = object_annotation.frames(0).normalized_bounding_box().top();
    float bottom =
        object_annotation.frames(0).normalized_bounding_box().bottom();

    int box_width = video_width * (right - left);
    int box_height = video_height * (bottom - top);
    SDL_Rect box = {static_cast<int>(left * video_width),
                    static_cast<int>(top * video_height), box_width,
                    box_height};
    box_rects.push_back(box);

    // Creates a "SDL_Texture*" for each detected object's description and
    // push it to `textures` vector.
    SDL_Color txt_color = {255, 165, 0};  // Color is orange.
    SDL_Surface* surface = TTF_RenderText_Solid(
        font_ptr, object_annotation.entity().description().c_str(), txt_color);
    if (surface != nullptr) {
      SDL_Texture* texture =
          SDL_CreateTextureFromSurface(sdl_renderer, surface);
      if (texture != nullptr) {
        textures.push_back(texture);
        // Creates a SDL_Rect instance to store object description's location.
        // The object description will be placed at the top-left corner of its
        // associated bounding box.
        SDL_Rect box_text_rect = {static_cast<int>(left * video_width),
                                  static_cast<int>(top * video_height),
                                  surface->w, surface->h};
        box_text_rects.push_back(box_text_rect);
      }
      SDL_FreeSurface(surface);
    }
  }

  // Convert vector to array.
  SDL_Rect box_rects_array[box_rects.size()];
  std::copy(box_rects.begin(), box_rects.end(), box_rects_array);

  SDL_SetRenderDrawColor(sdl_renderer, 255, 165, 0, 255);  // orange
  // Updates renderer to display bounding box.
  SDL_RenderDrawRects(sdl_renderer, box_rects_array, box_rects.size());
  // Updates renderer to display object description for each bounding box.
  for (uint32_t i = 0; i < textures.size() && i < box_text_rects.size(); i++) {
    SDL_RenderCopy(sdl_renderer, textures[i], nullptr, &box_text_rects[i]);
  }
}

}  // namespace

uint32_t GetAnnotationResponseTimestamp(
    const StreamingAnnotateVideoResponse& resp) {
  if (!resp.has_annotation_results()) {
    LOG(ERROR) << "Failed to get valid response timestamp.";
    return 0;
  }
  StreamingVideoAnnotationResults annotation_results =
      resp.annotation_results();

  uint32_t timestamp;  // in millisecond
  // LABEL_DETECTION feature
  if (IsLabelDetectionResponse(resp) &&
      annotation_results.label_annotations(0).frames_size() > 0) {
    const LabelAnnotation& label_annotation =
        annotation_results.label_annotations(0);
    timestamp = label_annotation.frames(0).time_offset().seconds() * 1000 +
                label_annotation.frames(0).time_offset().nanos() / 1e6;
    return timestamp;
  }

  // OBJECT_TRACKING feature
  if (IsObjectTrackingResponse(resp) &&
      annotation_results.object_annotations(0).frames_size() > 0) {
    const ObjectTrackingAnnotation& object_annotation =
        annotation_results.object_annotations(0);
    timestamp = object_annotation.frames(0).time_offset().seconds() * 1000 +
                object_annotation.frames(0).time_offset().nanos() / 1e6;
    return timestamp;
  }

  return 0;
}

uint32_t GetRendererClearThreshold(const StreamingAnnotateVideoResponse& resp) {
  if (IsLabelDetectionResponse(resp)) {
    return kRendererClearThresholdLabel;
  }

  if (IsObjectTrackingResponse(resp)) {
    return kRendererClearThresholdTracking;
  }
  LOG(ERROR) << "Failed to get valid renderer clear threshold.";
  return 0;
}

void UpdateSDLRendererContent(const StreamingAnnotateVideoResponse& resp,
                              const int video_width, const int video_height,
                              TTF_Font* font_ptr, SDL_Renderer* sdl_renderer) {
  if (sdl_renderer == nullptr) {
    LOG(ERROR) << "Invalid SDL_Renderer instance";
    return;
  }

  if (!resp.has_annotation_results()) {
    LOG(ERROR) << "Empty annotation_results in StreamingAnnotateVideoResponse.";
    return;
  }

  // Updates Label detection renderer.
  if (IsLabelDetectionResponse(resp)) {
    UpdateLabelDetectionSDLRenderer(resp, font_ptr, sdl_renderer);
    return;
  }

  // Updates object tracking renderer.
  if (IsObjectTrackingResponse(resp)) {
    UpdateObjectTrackingSDLRenderer(resp, video_width, video_height, font_ptr,
                                    sdl_renderer);
    return;
  }
}

}  // namespace video
}  // namespace api
