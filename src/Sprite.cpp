#include <fstream>

#include "Sprite.h"
#include "BMP_Headers.h"


// Check if the pixel data is stored as BGRA and if the color space type is sRGB
void check_color_header(BMPColorHeader &bmp_color_header) {
    BMPColorHeader expected_color_header;
    if (expected_color_header.red_mask != bmp_color_header.red_mask ||
        expected_color_header.blue_mask != bmp_color_header.blue_mask ||
        expected_color_header.green_mask != bmp_color_header.green_mask ||
        expected_color_header.alpha_mask != bmp_color_header.alpha_mask)
        log_error_and_exit("Unexpected color mask format. Pixel data must be in the BGRA format.");

    if (expected_color_header.color_space_type != bmp_color_header.color_space_type)
        log_error_and_exit("Unexpected color space type (must be sRGB).");
}


// Load sprite from .bmp
Sprite::Sprite(const char *fname) {
    std::ifstream input_file{ fname, std::ios_base::binary };
    if (!input_file)
        log_error_and_exit("Unable to open the input image file.");
    {
        BMPFileHeader file_header;
        BMPInfoHeader bmp_info_header;
        BMPColorHeader bmp_color_header;

        input_file.read((char*)&file_header, sizeof(file_header));
        if(file_header.file_type != 0x4D42)
            log_error_and_exit("Unrecognized file format.");
        
        input_file.read((char*)&bmp_info_header, sizeof(bmp_info_header));

        if(bmp_info_header.bit_count != 32)
            log_error_and_exit("For purposes of this game images must contain alpha channel.");
        
        if(bmp_info_header.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) {
            input_file.read((char*)&bmp_color_header, sizeof(bmp_color_header));
            check_color_header(bmp_color_header);
        } else
            log_error_and_exit("Unrecognized file format. The file does not seem to contain bit mask information.");

        // Jump to the pixel data location
        input_file.seekg(file_header.offset_data, input_file.beg);

        if (bmp_info_header.height < 0)
            log_error_and_exit("BMP images here must be with the origin in the bottom left corner.");

        m_data.resize(bmp_info_header.width * bmp_info_header.height * bmp_info_header.bit_count / 8);

        // Check if we need to take into account row padding
        if (bmp_info_header.width % 4 == 0)
            input_file.read((char*)m_data.data(), m_data.size());
        else {
            uint32_t row_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
            for (int y = 0; y < bmp_info_header.height; ++y) {
                input_file.read((char*)(m_data.data() + row_stride * y), row_stride);
            }
        }

        m_height = bmp_info_header.height;
        m_width = bmp_info_header.width;
    }
}