FROM ubuntu:18.04

RUN apt-get update && apt-get install  --no-install-recommends -y \
    make \
    gcc-avr \
    avr-libc \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

VOLUME /source

WORKDIR /source

CMD bash -c "make clean && make"