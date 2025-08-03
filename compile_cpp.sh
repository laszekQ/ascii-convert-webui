emcc cpp/main.cpp -o js/wasm_module.js \
  -std=c++20 \
  -s EXPORTED_FUNCTIONS='["_imgToAscii", "_pixelsToASCII", "_getBrightness", "_malloc", "_free"]' \
  -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "HEAPU8", "UTF8ToString"]' \
  -s MODULARIZE \
  -s EXPORT_NAME="ASCIIModule" \
  -s 'ENVIRONMENT="web"'
