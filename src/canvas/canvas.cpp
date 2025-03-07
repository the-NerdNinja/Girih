#include "canvas.hpp"
#include "../../lodepng/lodepng.h"
#include <algorithm>
#include <iostream>

using namespace std;

static RGBA blend(const RGBA &dest, const RGBA &src) {
    RGBA out;
    out.r = (src.r * src.a + dest.r * (255 - src.a)) / 255;
    out.g = (src.g * src.a + dest.g * (255 - src.a)) / 255;
    out.b = (src.b * src.a + dest.b * (255 - src.a)) / 255;
    out.a = src.a + (dest.a * (255 - src.a)) / 255;
    return out;
}

canvas::canvas(int s) : SIZE(s),
                        pix(s, vector<RGBA>(s, RGBA())),
                        ovrly(s, vector<RGBA>(s, RGBA(0,0,0,0))) {}

void canvas::draw(vec2 p, const RGBA &color)
{
    float px = SIZE / 2.0f + p.x;
    float py = SIZE / 2.0f - p.y;

    int base_x = floor(px);
    int base_y = floor(py);

    for (int i = base_x - LINE_THICKNESS; i <= base_x + LINE_THICKNESS; ++i)
    {
        for (int j = base_y - LINE_THICKNESS; j <= base_y + LINE_THICKNESS; ++j)
        {
            if (i < 0 || i >= SIZE || j < 0 || j >= SIZE)
                continue;

            float cx = i + 0.5f;
            float cy = j + 0.5f;

            float dx = px - cx;
            float dy = py - cy;
            float dist = sqrt(dx * dx + dy * dy);

            // Define the maximum distance for the kernel influence.
            float maxDist = LINE_THICKNESS + 0.5f;

            // Option 1: Quadratic falloff (uncomment to try)
            // float factor = std::max(0.0f, 1.0f - (dist / maxDist) * (dist / maxDist));

            // Option 2: Gaussian falloff for a smoother effect:
            float sigma = maxDist / 2.0f;
            float factor = exp(-(dist * dist) / (2 * sigma * sigma));
            factor = std::max(0.0f, std::min(1.0f, factor)); // Clamp between 0 and 1

            // Modulate the incoming color's opacity using this factor.
            RGBA modColor = color;
            modColor.a = static_cast<unsigned char>(color.a * factor);

            pix[j][i] = blend(pix[j][i], modColor);
        }
    }
}

void canvas::draw_segment(const segment &seg, const RGBA &color) {
    vec2 start = seg.start.loc();
    vec2 end = seg.end.loc();

    const int steps = 500; //IMPORTANT

    ovrly.assign(SIZE, std::vector<RGBA>(SIZE, RGBA(0, 0, 0, 0)));
    for (int k = 0; k <= steps; k++) {
         float t = static_cast<float>(k) / steps;
         vec2 d = { end.x - start.x, end.y - start.y };
         vec2 p = { start.x + d.x * t, start.y + d.y * t };
         float px = SIZE / 2.0f + p.x;
         float py = SIZE / 2.0f - p.y;
         int base_x = floor(px);
         int base_y = floor(py);
         for (int i = base_x - LINE_THICKNESS; i <= base_x + LINE_THICKNESS; ++i) {
             for (int j = base_y - LINE_THICKNESS; j <= base_y + LINE_THICKNESS; ++j) {
                if (i < 0 || i >= SIZE || j < 0 || j >= SIZE)
                    continue;
                float cx = i + 0.5f;
                float cy = j + 0.5f;
                float dx = px - cx;
                float dy = py - cy;
                float dist = sqrt(dx * dx + dy * dy);
                float maxDist = LINE_THICKNESS + 0.5f;
                float sigma = maxDist / 2.0f;
                float factor = exp(-(dist * dist) / (2 * sigma * sigma));
                factor = max(0.0f, min(1.0f, factor));
                RGBA modColor = color;
                modColor.a = static_cast<unsigned char>(color.a * factor);
                ovrly[j][i] = blend(ovrly[j][i], modColor);
             }
         }
    }
}


void canvas::render(const char* filename){
    std::vector<unsigned char> image(SIZE * SIZE * 4, 0);
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            RGBA combined = blend(pix[i][j], ovrly[i][j]);
            int idx = (i * SIZE + j) * 4;
            image[idx + 0] = combined.r;
            image[idx + 1] = combined.g;
            image[idx + 2] = combined.b;
            image[idx + 3] = combined.a;
        }
    }
    unsigned err = lodepng::encode(filename, image, SIZE, SIZE, LCT_RGBA);
    cout << (err ? "Error" : "Saved:") << filename << endl;
}
