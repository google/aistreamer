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

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "file_reader",
    srcs = [
        "file_reader.cc",
    ],
    hdrs = [
        "file_reader.h",
        "io_reader.h",
    ],
    deps = [
        "//external:glog",
    ],
)

cc_library(
    name = "file_writer",
    srcs = [
        "file_writer.cc",
    ],
    hdrs = [
        "file_writer.h",
        "io_writer.h",
    ],
    deps = [
        "//external:glog",
    ],
)

cc_library(
    name = "io_reader",
    deps = [
        "file_reader",
        "pipe_reader",
        "//external:glog",
    ],
)

cc_library(
    name = "io_writer",
    deps = [
        "file_writer",
        "proto_writer",
        "//external:glog",
    ],
)

cc_library(
    name = "pipe_reader",
    srcs = [
        "pipe_reader.cc",
    ],
    hdrs = [
        "io_reader.h",
        "pipe_reader.h",
    ],
    deps = [
        "//external:glog",
    ],
)

cc_library(
    name = "proto_reader",
    srcs = [
        "proto_reader.cc",
    ],
    hdrs = [
        "io_reader.h",
        "proto_reader.h",
    ],
    deps = [
        "//external:glog",
        "//proto:status_cc_proto",
    ],
)

cc_library(
    name = "proto_processor",
    srcs = [
        "proto_processor.cc",
    ],
    hdrs = [
        "proto_processor.h",
    ],
    deps = [
        "//external:glog",
        "//proto:video_intelligence_streaming_cc_proto",
    ],
)

cc_library(
    name = "proto_writer",
    srcs = [
        "proto_writer.cc",
    ],
    hdrs = [
        "io_writer.h",
        "proto_writer.h",
    ],
    deps = [
        "//external:glog",
        "//proto:status_cc_proto",
    ],
)

cc_test(
    name = "proto_io_test",
    size = "large",
    srcs = [
        "proto_io_test.cc",
    ],
    tags = ["exclusive"],
    deps = [
        ":proto_reader",
        ":proto_writer",
        "//proto:video_intelligence_streaming_cc_proto",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "streaming_client",
    srcs = [
        "streaming_client.cc",
    ],
    hdrs = [
        "file_reader.h",
        "file_writer.h",
        "pipe_reader.h",
        "proto_writer.h",
        "streaming_client.h",
    ],
    deps = [
        ":io_reader",
        ":io_writer",
        ":proto_processor",
        "//external:gflags",
        "//external:glog",
        "//proto:video_intelligence_streaming_cc_proto",
    ],
)

cc_binary(
    name = "streaming_client_main",
    srcs = [
        "streaming_client_main.cc",
    ],
    deps = [
        ":streaming_client",
        "//external:gflags",
        "//external:glog",
    ],
)
