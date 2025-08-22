# ascii-convert-webui

## What's it
A WebUI version of [ascii-convert](https://github.com/laszekQ/ascii-convert).

## Screenshots
![screenshot 1](assets/Screenshot1.png)

## How to use
Everything is pretty straightforward. Touch anything on the screen and the image will be redrawn.

## Build and Run
### Docker
If you have Docker, simply run:
```
docker compose up -d
```
in the root directory.
<br>
### Node.js and emcc
If you don't have Docker, you can install Node.js's http-server utility:
```
npm install http-server
```
and then simply type:
```
http-server
```
in the root directory to start the server. It should available on port 8080.
<br>
However, you still need to build the wasm_module. To do that, install [Emscripten](https://emscripten.org/). And then run the ```compile_cpp.sh``` file to build the WebAssembly module.