FROM emscripten/emsdk:4.0.15 AS builder
WORKDIR /src
COPY src/Model.hpp src/World.hpp src/World.cpp src/
COPY web/ web/
RUN em++ -std=c++20 -O3 --no-entry -Isrc src/World.cpp web/bridge.cpp \
    -sMODULARIZE=1 -sEXPORT_NAME=createInvaders -sENVIRONMENT=web,node \
    -sALLOW_MEMORY_GROWTH=1 -sEXPORTED_RUNTIME_METHODS=HEAPF32 \
    -sEXPORTED_FUNCTIONS=_inv_reset,_inv_step,_inv_score,_inv_lives,_inv_wave,_inv_frame,_inv_frame_data \
    -o web/engine.js
RUN node web/test-engine.cjs

FROM nginx:1.28-alpine
COPY deploy/nginx.conf /etc/nginx/conf.d/default.conf
COPY --from=builder /src/web/index.html /src/web/style.css /src/web/app.js /src/web/render.js /src/web/engine.js /src/web/engine.wasm /usr/share/nginx/html/
