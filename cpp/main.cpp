#include <string>
#include <vector>
#include <initializer_list>
#include <emscripten.h>

extern "C" 
{
    using char_vector = std::vector< std::vector<char> >;
    const int COLOR_RANGE = 255;

    struct Color
    {
        int r, g, b;
    };

    EMSCRIPTEN_KEEPALIVE
    int getBrightness(const Color &pixel)
    {
        int r = pixel.r;
        int g = pixel.g;
        int b = pixel.b;
        int bright = (r + g + b) / 3;

        return bright;
    }

    EMSCRIPTEN_KEEPALIVE
    char pixelsToASCII(std::initializer_list<Color> pixels, 
                       char* gradient, int grad_len, const char c_mode)
        {
            float bright = 0;
            for(const auto& pixel : pixels)
                bright += getBrightness(pixel);
            bright /= pixels.size();

            float step = COLOR_RANGE / (grad_len - 1);
            int index = bright / step;

            if(c_mode == 'd')
                index = grad_len - index - 1;
            
            return gradient[index]; 
        }

    EMSCRIPTEN_KEEPALIVE
    char* imgToAscii(uint8_t* data, uint32_t WIDTH, uint32_t HEIGHT, char* gradient, 
                        int grad_len,  char c_mode, char mode)
    {
        int width = WIDTH * (1 + (mode == '2'));
        int height = HEIGHT / (1 + (mode == '3'));

        std::vector<char> out;
        uint32_t out_size = width * height + height;
        out.reserve(out_size);

        auto getPixel = [](uint8_t* data, int index)
        {
            int r = data[index];
            int g = data[index + 1];
            int b = data[index + 2];
            Color pixel = {r, g, b};
            return pixel;
        };

        int k = 0;
        for(int i = 0; i < WIDTH * HEIGHT; i++)
        {
            int index = i * 4;
            Color pixel = getPixel(data, index);
            char c = pixelsToASCII({pixel}, gradient, grad_len, c_mode);
            out.push_back(c);

            if(mode == '2') // 1:2
                out.push_back(c);

            k++;
            if(k % WIDTH == 0)
                out.push_back('\n');
        }

        printf("Number of pixels: %i\n", k);

        char* buffer = (char*)malloc(out.size() + 1);
        std::memcpy(buffer, out.data(), out.size());
        buffer[out.size()] = '\0';
        
        return buffer;
    }
}