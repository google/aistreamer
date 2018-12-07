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

# To build a new docker image, please run:
# docker build -t streaming-api:v1.0 -f env/Dockerfile .

FROM ubuntu:xenial

LABEL description="Streaming API build environment."

RUN apt-get update && apt-get upgrade -y

RUN apt-get update && apt-get install -y \
    apt-utils \
    autoconf \
    autoconf-archive \
    automake \
    binutils-dev \
    build-essential \
    clang \
    cmake \
    curl \
    g++ \
    gcc \
    git \
    google-perftools \
    libass-dev \
    libboost-all-dev \
    libdouble-conversion-dev \
    libevent-dev \
    libgflags-dev \
    libfreetype6-dev \
    libgoogle-glog-dev \
    libgtest-dev \
    libiberty-dev \
    libjemalloc-dev \
    liblz4-dev \
    liblzma-dev \
    libopenmpi-dev \
    libsdl2-dev \
    libsnappy-dev \
    libssl-dev \
    libtool \
    libva-dev \
    libvdpau-dev \
    libvorbis-dev \
    libxcb1-dev \
    libxcb-shm0-dev \
    libxcb-xfixes0-dev \
    make \
    python-dev \
    python-setuptools \
    wget \
    vim \
    pkg-config \
    zlib1g-dev \
    && apt-get clean

RUN easy_install pip

RUN apt-get update & apt-get install -y software-properties-common
RUN add-apt-repository ppa:jonathonf/ffmpeg-3 -y

RUN apt-get update && apt-get install -y \
    libav-tools \
    libx264-dev \
    libx265-dev \
    libnuma-dev \
    libvpx-dev \
    libfdk-aac-dev \
    libmp3lame-dev \
    libopus-dev \
    ffmpeg \
    && apt-get clean

RUN apt-get install -y \
    libgstreamer1.0-0 \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    gstreamer1.0-doc \
    gstreamer1.0-tools \
    && apt-get clean

ENV CPLUS_INCLUDE_PATH=/usr
ENV LD_LIBRARY_PATH=/usr

#install gRPC 1.12.0
ENV GRPC_RELEASE_TAG v1.12.0
RUN git clone -b ${GRPC_RELEASE_TAG} https://github.com/grpc/grpc /var/local/git/grpc
RUN cd /var/local/git/grpc && \
    git submodule update --init && \
    make && \
    make prefix=/usr install && make clean

#install proto v3.6.0
ENV PROTO_RELEASE_TAG v3.6.0
RUN git clone -b ${PROTO_RELEASE_TAG} https://github.com/google/protobuf.git \
    /var/local/git/protobuf && \
    cd /var/local/git/protobuf && \
    git submodule update --init --recursive && \
    ./autogen.sh && \
    ./configure && make && make install

#install bazel
RUN apt-get -y install openjdk-8-jdk && \
    echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list && \
    curl https://storage.googleapis.com/bazel-apt/doc/apt-key.pub.gpg | apt-key add - && \
    apt-get update && \
    apt-get -y install bazel && \
    apt-get -y upgrade bazel && \
    apt-get clean

#install Google Video Intelligence Streaming API Python Libraries
RUN pip install https://storage.googleapis.com/api-client-staging/videointelligence-python-q3alpha-sep2018.tar.gz

#set up environment for Google Video Intelligence Streaming API
ENV SRC_DIR /googlesrc
ENV BIN_DIR /google

#copy all aistreamer directory to Docker
RUN mkdir -p $SRC_DIR
COPY BUILD *.BUILD *.md *.py LICENSE WORKSPACE $SRC_DIR/
COPY client $SRC_DIR/client
COPY env $SRC_DIR/env
COPY proto $SRC_DIR/proto

#build aistreamer
RUN cd $SRC_DIR/client/cpp && bazel build -c opt streaming_client_main

#copy binaries to BIN_DIR directory
RUN mkdir -p $BIN_DIR
RUN mkdir -p $BIN_DIR/cpp
RUN mkdir -p $BIN_DIR/python
RUN cp $SRC_DIR/bazel-bin/client/cpp/streaming_client_main $BIN_DIR/cpp
RUN cp $SRC_DIR/client/python/*.py $BIN_DIR/python
RUN chmod +x $BIN_DIR/python/*.py

#clean up the build artifacts and source directory.
RUN cd $SRC_DIR  && bazel clean --expunge

CMD ["/bin/bash"]
