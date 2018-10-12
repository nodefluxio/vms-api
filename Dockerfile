FROM ubuntu:16.04

WORKDIR /app

RUN apt-get update && apt-get install -y build-essential cmake curl libboost-all-dev && rm -rf /var/lib/apt/lists/*

EXPOSE 8000

COPY external /app/external
RUN cp -r /app/external/eSDK_IVS_API/lib/* /usr/local/lib \
    && cp -r /app/external/eSDK_IVS_API/config /usr/local/config

ENV IVS_SDK_PATH=/app/external/eSDK_IVS_API

COPY CMakeLists.txt /app/CMakeLists.txt
COPY modules /app/modules

RUN mkdir /app/build \
    && cd /app/build \
    && cmake .. \
    && cmake --build /app/build --target all --config Release -- -j 10 \
    && mv /app/build/bin/* /usr/local/bin/ \
    && mv /app/build/lib/* /usr/local/lib \
    && ldconfig \
    && rm -r /app

WORKDIR /

CMD ["vmsapi"]
