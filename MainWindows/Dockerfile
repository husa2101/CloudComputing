FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV DISPLAY=:0

RUN apt-get update && apt-get install -y \
    build-essential cmake qt6-base-dev qt6-base-dev-tools \
    xvfb x11vnc novnc websockify fluxbox \
    libxcb-xinerama0 libxcb-cursor0 libxkbcommon-x11-0 \
    libfontconfig1 libxrender1 libxcb-icccm4 libxcb-image0 \
    libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 \
    libxcb-shape0 libxcb-sync1 libxcb-xfixes0 libgl1 \
    libxkbcommon-dev \
    && rm -rf /var/lib/apt/lists/*

RUN ln -s /usr/share/novnc/vnc.html /usr/share/novnc/index.html

WORKDIR /app

COPY . /app/

RUN rm -rf build && mkdir build && cd build && \
    cmake .. && \
    make

RUN chmod +x /app/entrypoint.sh

CMD ["/app/entrypoint.sh"]
