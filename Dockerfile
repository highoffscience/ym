# docker build -t cpp-dev .
# docker run --rm -it cpp-dev

# Base image
FROM ubuntu:26.04

# Avoid prompts during package install
ENV DEBIAN_FRONTEND=noninteractive

ARG WORK_DIR=/workspace

# Set default working directory
WORKDIR ${WORK_DIR}

# Update system and install core build tools + Python
RUN apt-get update && apt-get install -y \
   build-essential \
   wget \
   curl \
   git \
   ca-certificates \
   ninja-build \
   python3 \
   python3-venv \
   python3-dev \
   python3-pip \
   python-is-python3 \
   libssl-dev \
   gcc \
   g++ \
   clang \
   cmake \
   vim \
   graphviz

# Get repo with error handling
RUN git clone https://github.com/highoffscience/ym.git

WORKDIR ${WORK_DIR}/ym
RUN git switch working
RUN git submodule update --init extlibs/fmt

WORKDIR ${WORK_DIR}/ym/extlibs
RUN cmake -S . --preset default -DYM_BUILD_LLVM=OFF
RUN cmake --build build

WORKDIR ${WORK_DIR}/ym/unittests
RUN python -m venv venv
RUN ./venv/bin/python -m pip install -r requirements.txt
RUN cmake -S . --preset test

# Default shell
CMD ["/bin/bash"]
