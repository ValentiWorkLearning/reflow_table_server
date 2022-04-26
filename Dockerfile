FROM gcc:11.3.0

RUN apt-get update; \
    apt-get install -y python3-pip  build-essential libtool autoconf unzip wget

CMD ["mkdir","-p", "/usr/src/drogon_server"]
CMD ["mkdir","-p", "/usr/src/drogon_server/build"]

## Build latest cmake
ARG CMAKE_VERSION=3.22
ARG CMAKE_BUILD_VERSION=2

CMD ["mkdir", "~/temp"]
CMD ["cd", "~/temp"]
RUN wget https://cmake.org/files/v$CMAKE_VERSION/cmake-$CMAKE_VERSION.$CMAKE_BUILD_VERSION.tar.gz
RUN tar -xzvf cmake-$CMAKE_VERSION.$CMAKE_BUILD_VERSION.tar.gz

WORKDIR cmake-$CMAKE_VERSION.$CMAKE_BUILD_VERSION/

RUN ls -la
RUN ./bootstrap
RUN make -j$(nproc)
RUN make install

## Install conan dependencies
RUN pip install conan

COPY . /usr/src/drogon_server
WORKDIR /usr/src/drogon_server/build

RUN cmake -G"Unix Makefiles"  ..
RUN cmake --build .


FROM ubuntu:latest
COPY /usr/src/drogon_server/build/app_test_drogon_server .
COPY app_test_drogon_server/build/config.json .
EXPOSE 8086
ENTRYPOINT ["./app_test_drogon_server"]