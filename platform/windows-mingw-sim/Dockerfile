FROM debian:bookworm

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        mingw-w64 \
        pkg-config \
        build-essential \
        ca-certificates \
        git \
        autoconf automake libtool \
        wget \
        && \
    rm -rf /var/lib/apt/lists/*

COPY install-cairo.sh install-freetype.sh install-pixman.sh install-libpng.sh install-zlib.sh /scripts/
RUN chmod +x /scripts/*.sh

RUN bash /scripts/install-zlib.sh && \
    bash /scripts/install-pixman.sh && \
    bash /scripts/install-libpng.sh && \
    bash /scripts/install-freetype.sh && \
    bash /scripts/install-cairo.sh

WORKDIR /build

CMD ["/bin/bash"]
