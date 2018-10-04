FROM ubuntu:16.04

WORKDIR /app

RUN apt-get update && apt-get install -y \
    build-essential cmake curl libboost-all-dev \
    libssl-dev libcurl4-openssl-dev \
    && rm -rf /var/lib/apt/lists/*

ADD . /app

ENV IVS_SDK_PATH=/app/external/eSDK_IVS_API

RUN mkdir /app/build \
    && cd /app/build \
    && cmake -DUSE_SYSTEM_CURL=ON .. \
    && cmake --build /app/build --target all --config Release -- -j 10

EXPOSE 8000

CMD ["build/bin/vmsapi"]
