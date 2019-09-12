# Build the app
FROM alpine:latest

# Downoload necessary dependencies to build C++ & CMake
RUN apk update && apk add --no-cache\
    cmake\
    make\
    gcc\
    g++\
    libressl-dev\
    linux-headers

# Build & install Poco
COPY docker/poco /poco
WORKDIR /poco/cmake-build
RUN cmake .. && cmake --build . --target install --parallel 3

# Build the sources
ARG APP_NAME
COPY ${APP_NAME} /src
WORKDIR /src/cmake-build
RUN cmake -DPOCO_PREFIX:STRING="/usr/local" .. && cmake --build . --parallel 3

# Make the app the entrypoint of the container
ENV BUILT_APP_NAME=${APP_NAME}
ENTRYPOINT ./${BUILT_APP_NAME}
