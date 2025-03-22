FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive \
    LC_ALL=en_US.UTF8 \
    LANG=en_US.UTF8

RUN apt-get update && apt-get install -y \
    build-essential \
    gdb \
    locales \
    git \
    cmake \
    wget \
    curl \
    autoconf \
    automake

ENV BOOST_VERSION=1.87.0

RUN cd /tmp && \
    BOOST_VERSION_MOD1=$(echo $BOOST_VERSION | tr . _) && \
    wget https://archives.boost.io/release/${BOOST_VERSION}/source/boost_${BOOST_VERSION_MOD1}.tar.bz2 && \
    tar --bzip2 -xf boost_${BOOST_VERSION_MOD1}.tar.bz2 && \
    cd boost_${BOOST_VERSION_MOD1} && \
    ./bootstrap.sh --prefix=/usr/local && \
    ./b2 install && \
    rm -rf /tmp/*

RUN cd /tmp && \
    git clone https://github.com/abseil/abseil-cpp.git && \
    cd abseil-cpp && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local && \
    cmake --build . --target install && \
    rm -rf /tmp/*

RUN dd if=/dev/zero bs=1M count=100 | tr '\000' '\001' > /usr/local/dummyf.bin