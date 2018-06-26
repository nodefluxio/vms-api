FROM ubuntu:16.04

WORKDIR /app

RUN apt-get update && apt-get install -y build-essential cmake libboost-all-dev && rm -rf /var/lib/apt/lists/*

ADD . /app

ENV IVS_SDK_PATH=/app/IVS_SDK

RUN mkdir /app/build && cd /app/build && cmake .. && cmake --build /app/build --target all --config Release -- -j 10

EXPOSE 8000

CMD ["build/bin/vmsapi"]
