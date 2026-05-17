FROM emscripten/emsdk:latest AS builder

WORKDIR /src
COPY cpp/ cpp/
COPY compile_cpp.sh .
RUN mkdir -p js && bash compile_cpp.sh

# Stage 2: serve the app
FROM node:24-alpine

ENV NODE_ENV=production
WORKDIR /usr/src/app

RUN npm install -g http-server

COPY . .

COPY --from=builder /src/js/wasm_module.js js/wasm_module.js
COPY --from=builder /src/js/wasm_module.wasm js/wasm_module.wasm

EXPOSE 8080
CMD ["http-server", ".", "-p", "8080"]
