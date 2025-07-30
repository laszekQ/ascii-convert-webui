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

            // reverse the index depending on color mode
            if(c_mode == 'd')
                index = grad_len - index - 1;
            
            return gradient[index]; 
        }

    EMSCRIPTEN_KEEPALIVE
    char* imgToAscii(uint8_t* data, uint32_t WIDTH, uint32_t HEIGHT, char* gradient, 
                        int grad_len,  char c_mode, char mode)
    {
        uint32_t height = HEIGHT / (1 + (mode == '2'));
        uint32_t width = WIDTH * (1 + (mode == '3'));
        char_vector t_out(height, std::vector<char>(width));

        // in 2:1 (mode = 2) we need to convert 2 pixels into 1 char,
        // therefore, the iterator must be incremented by 2 instead 1
        int step_y = 1 + (mode == '2');

        auto getPixel = [](uint8_t* data, uint32_t x, uint32_t y, uint32_t WIDTH)
        {
            uint32_t index = y * WIDTH + x; 
            int r = data[index + 0];
            int g = data[index + 1];
            int b = data[index + 2];

            Color c = {r, g, b};
            return c;
        }; 

        for(uint32_t i = 0; i <= HEIGHT - step_y; i += step_y)
        {
            int x = 0;
            for(uint32_t j = 0; j < WIDTH; j++)
            {
                char c = ' ';
                int y = i;
                switch(mode)
                {
                case '1':
                {
                    Color pixel = getPixel(data, j, i, WIDTH);
                    c = pixelsToASCII({pixel}, gradient, grad_len, c_mode);
                    t_out[i][j] = c;
                    break;
                }
                case '2':
                {
                    Color pixel1 = getPixel(data, j, i, HEIGHT);
                    Color pixel2 = getPixel(data, j, i + 1, HEIGHT);
                    c = pixelsToASCII({pixel1, pixel2}, gradient, grad_len, c_mode);
                    y /= 2;
                    t_out[y][j] = c;
                    break;
                }
                case '3':
                {
                    Color pixel = getPixel(data, j, i, HEIGHT); 
                    c = pixelsToASCII({pixel}, gradient, grad_len, c_mode);
                    t_out[i][x] = c;
                    t_out[i][x + 1] = c;
                    x += 2;
                    break;
                }
                default:
                    return nullptr;
                }
            }
        }

        int pix_count = width * height;
        int end_size = pix_count + height;
        
        std::vector<char> out;
        out.reserve(end_size);

        for(auto r : t_out)
        {
            for(auto c : r)
                out.push_back(c);
            out.push_back('\n');
        }
        char* buffer = (char*)malloc(out.size());
        std::memcpy(buffer, out.data(), out.size());
        
        return buffer;
    }
}