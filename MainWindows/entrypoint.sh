#!/bin/bash
set -e

# Start the virtual screen
Xvfb :0 -screen 0 1024x768x24 &
sleep 2

# Start the window manager and VNC/Web tools
fluxbox &
x11vnc -display :0 -nopw -listen localhost -xkb -ncache 10 -ncache_cr -forever &
websockify --web /usr/share/novnc/ 8080 localhost:5900 &

# Run the Qt application
/app/build/MainWindows

# Run the Qt application with verbose debugging enabled
export QT_DEBUG_PLUGINS=1
/app/build/MainWindows
