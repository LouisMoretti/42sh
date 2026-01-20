FROM ubuntu:22.04
WORKDIR /usr/local/app

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    autoconf \
    automake \
    autoconf-archive \
    libcriterion-dev \
    python3-pytest \
    clang-format \
    git \
    && rm -rf /var/lib/apt/lists/*

CMD ["/bin/bash"]
