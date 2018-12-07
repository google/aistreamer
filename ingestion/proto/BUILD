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

load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cc_proto_library")
load("@org_pubref_rules_protobuf//python:rules.bzl", "py_proto_library")

cc_proto_library(
    name = "video_intelligence_streaming_cc_proto",
    imports = [
        "external/com_google_protobuf/src/",
    ],
    inputs = [
        "@com_google_protobuf//:well_known_protos",
    ],
    proto_deps = [
        ":status_cc_proto",
    ],
    protos = [
        "video_intelligence_streaming.proto",
    ],
    with_grpc = True,
    deps = [
        "@com_google_protobuf//:cc_wkt_protos",
    ],
)

cc_proto_library(
    name = "status_cc_proto",
    imports = [
        "external/com_google_protobuf/src/",
    ],
    inputs = [
        "@com_google_protobuf//:well_known_protos",
    ],
    protos = [
        "status.proto",
    ],
    deps = [
        "@com_google_protobuf//:cc_wkt_protos",
    ],
)
