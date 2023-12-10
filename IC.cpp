#include <bits/stdc++.h>

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image_write.h"

/* RGB Color Model */

struct RGB
{
    uint64_t red;
    uint64_t green;
    uint64_t blue;
};

/* YCbCr Color Space */

struct YCbCr
{
    uint64_t Y;
    uint64_t Cb;
    uint64_t Cr;
};

/* Bucket in Brute Force Table */

struct Bucket
{
    RGB rgb;
    YCbCr ycbcr;

    std::string index(void)
    {
        return std::to_string(this->ycbcr.Y) + std::to_string((this->rgb.red + this->rgb.green + this->rgb.blue) / 3) + std::to_string(this->rgb.red + this->rgb.green + this->rgb.blue);
    };

    bool operator==(const Bucket &pixel) const
    {
        if (!(pixel.ycbcr.Y == this->ycbcr.Y)) 
        {
            return false;
        };

        uint64_t alpha = ((pixel.rgb.red + pixel.rgb.green + pixel.rgb.blue) / 3);
        uint64_t beta = ((this->rgb.red + this->rgb.green + this->rgb.blue) / 3);

        uint64_t gamma = (pixel.rgb.red + pixel.rgb.green + pixel.rgb.blue);
        uint64_t delta = (this->rgb.red + this->rgb.green + this->rgb.blue);

        if (!(alpha == beta || (alpha + 1) == (beta + 1) || (alpha - 1) == (beta - 1)))
        {
            return false;
        };

        if (!(gamma == delta || (gamma + 1) == (delta + 1) || (gamma - 1) == (delta - 1)))
        {
            return false;
        };

        return true;
    };
};

/* Describes a Image */

typedef struct Image
{
    uint8_t *pixels;
    int32_t width;
    int32_t height;
    int32_t bpp;
} Image;

/* ITU-R BT.601 Conversion */

YCbCr RGBtoYCbCr(RGB pixel)
{
    YCbCr ycbcr = {};
    ycbcr.Y = round(16 + ((65.738 * pixel.red) / 256) + ((129.057 * pixel.green) / 256) + ((25.064 * pixel.blue) / 256));
    ycbcr.Cb = round(128 - ((37.945 * pixel.red) / 256) - ((74.494 * pixel.green) / 256) + ((112.439 * pixel.blue) / 256));
    ycbcr.Cr = round(128 + ((112.439 * pixel.red) / 256) - ((94.154 * pixel.green) / 256) - ((18.285 * pixel.blue) / 256));

    return ycbcr;
};

int32_t main(int32_t argc, char **argv)
{
    /* Usage */

    if (argc != 2)
    {
        std::cout << "Usage: IC [image]" << std::endl;
        return EXIT_FAILURE;
    };

    /* Image */

    Image grayscale = {};
    grayscale.pixels = stbi_load(argv[1], &grayscale.width, &grayscale.height, &grayscale.bpp, 0);
    if (!grayscale.pixels)
    {
        std::cout << "Cannot Open " << argv[1] << std::endl;
        return EXIT_FAILURE;
    };

    const uint64_t length = grayscale.width * grayscale.height * grayscale.bpp;
    Image color = {};
    color.width = grayscale.width;
    color.height = grayscale.height;
    color.bpp = grayscale.bpp;
    color.pixels = new uint8_t[length];

    /* Brute Force Table */

    std::unordered_map<std::string, std::vector<Bucket>> BFT = {};

    for (uint64_t red = 0; red <= 255; red++)
    {
        for (uint64_t green = 0; green <= 255; green++)
        {
            for (uint64_t blue = 0; blue <= 255; blue++)
            {
                Bucket bucket = {};
                bucket.rgb = {.red = red, .green = green, .blue = blue};
                bucket.ycbcr = RGBtoYCbCr(bucket.rgb);
                BFT[bucket.index()].push_back(bucket);
            };
        };
    };

    /* Brute Force */

    for (uint64_t index = 0; index < length; index += grayscale.bpp)
    {
        Bucket bucket = {};
        bucket.rgb = {.red = grayscale.pixels[index], .green = grayscale.pixels[index + 1], .blue = grayscale.pixels[index + 2]};
        bucket.ycbcr = RGBtoYCbCr(bucket.rgb);

        std::vector<Bucket> pixels = BFT[bucket.index()];
        for (Bucket pixel : pixels)
        {
            if (pixel == bucket)
            {
                color.pixels[index] = pixel.rgb.red;
                color.pixels[index + 1] = pixel.rgb.green;
                color.pixels[index + 2] = pixel.rgb.blue;
            };
        };
    };

    /* Clean Up */

    stbi_write_png("Image.png", color.width, color.height, color.bpp, color.pixels, color.width * color.bpp);
    stbi_image_free(grayscale.pixels);
    delete[] color.pixels;

    return EXIT_SUCCESS;
};
