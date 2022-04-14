/*
 * @Author: amdone
 * @Date: 2022-04-11 22:56:02
 * @LastEditors: amdone
 * @LastEditTime: 2022-04-11 23:34:38
 * @FilePath: \Test\imageopt.cpp
 * @Description:
 *
 * Copyright (c) 2022 by amdone, All Rights Reserved.
 */
#include "imageopt.h"
#include <string.h>

/**
 * @brief 通过读取文件头获得该文件是否是图片文件，并且返回实际的图片类型
 * @param ifstream& 已经打开的图片文件二进制输入流
 * @return {enum -> imgopt::ImageType}
 * 0 -> NOT_IMAGE,
 * 1 -> JPG,
 * 2 -> PNG,
 * 3 -> BMP
 */
imgopt::ImageType imgopt::ImgOpt::GetImageType(std::ifstream& ifs) {
    ifs.seekg(0);
    unsigned char buffer[12];
    ifs.read((char*)&buffer, sizeof(unsigned char) * 12);
    // jpg header(hex) FF D8 FF
    if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff) {
        return JPG;
    }
    // png header(hex) 89 50 4E 47 0D 0A 1A 0A
    if (buffer[0] == 0x89) {
        unsigned char png_header[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
        if (memcmp(png_header, buffer, 8) == 0) {
            return PNG;
        }
    }
    // webp header(hex) 52 49 46 46 __ __ __ __  57 45 42 50
    if (buffer[0] == 0x52) {
        unsigned char web_header[8] = {0x52, 0x49, 0x46, 0x46, 0x57, 0x45, 0x42, 0x50};
        if (memcmp(web_header, buffer, 4) == 0) {
            if (memcmp(&web_header[4], &buffer[8], 4) == 0) {
                return WEBP;
            }
        }
    }
    // bmp header(hex) 42 4d
    if (buffer[0] == 0x42 && buffer[1] == 0x4d) {
        return BMP;
    }
    // gif header(hex) 47 49 46
    if (buffer[0] == 0x47 && buffer[1] == 0x49 && buffer[2] == 0x46) {
        return GIF;
    }
    // tiff header(hex) 49492A or 4D4D2A
    if (buffer[0] == 0x49 && buffer[1] == 0x49 && buffer[2] == 0x2a && buffer[3] == 0x00) {
        return TIFF_II;
    }
    if (buffer[0] == 0x4D && buffer[1] == 0x4D && buffer[2] == 0x00 && buffer[3] == 0x2a) {
        return TIFF_MM;
    }
    return NOT_IMAGE;
}
/***
 * @brief 仅通过读取文件头部数据来得到图片的宽高
 * @param string imageFilePath 图片文件的实际路径
 * @return imgopt::Size {w,h} 一旦得到 {0,0} 即为失败
 */
imgopt::Size imgopt::ImgOpt::GetImageSize(const std::string imageFilePath) {
#define USE_IFSTREAM 1
#if USE_IFSTREAM == 1
    std::ifstream ifs;
    ifs.open(imageFilePath, std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
        printf("Error in Reading ImageFile");
        return {0, 0};
    } else {
        ImageType type = this->GetImageType(ifs);
        Size ret = {0, 0};
        switch (type) {
            case JPG: return this->GetJpegSize(ifs);
            case PNG: return this->GetPngSize(ifs);
            case BMP: return this->GetBmpSize(ifs);
            case WEBP: return this->GetWebpSize(ifs);
            case GIF: return this->GetGifSize(ifs);
            case TIFF_II: return this->GetTiffIISize(ifs);
            case TIFF_MM: return this->GetTiffMMSize(ifs);
            default: break;
        }
        return ret;
    }
#else
    FILE* fin = fopen(imageFilePath.c_str(), "rb+");
    // Do something here
    fclose(fin);
#endif
    return {0, 0};
}
/***
 * @brief 仅通过读取文件头部数据来得到图片的宽高
 * @param string imageFilePath 图片文件的实际路径
 * @param int& width 保存宽度到这个变量
 * @param int& height 保存高度度到这个变量
 * @return void 虽然没有返回值，但是一旦得到width或height为 0 即为失败
 */
void imgopt::ImgOpt::GetImageSize(const std::string imageFilePath, int& width, int& height) {
    imgopt::Size s = {0, 0};
    s = this->GetImageSize(imageFilePath);
    width = s.w;
    height = s.h;
}

imgopt::Size imgopt::ImgOpt::GetJpegSize(std::ifstream& ifs) {
    Size ret = {0, 0};
    int next_pos = 0x02;
    unsigned char header[9];
    while (1) {
        ifs.seekg(next_pos);
        ifs.read((char*)&header[0], sizeof(char) * 9);
        if (header[0] == 0xff) {
            next_pos = header[2] * 256 + header[3] + next_pos + 2;
        }
        if (header[1] == 0xc0) {
            ret.h = header[5] * 256 + header[6];
            ret.w = header[7] * 256 + header[8];
            ifs.close();
            return ret;
        }
    }
}

imgopt::Size imgopt::ImgOpt::GetPngSize(std::ifstream& ifs) {
    Size ret = {0, 0};
    unsigned char buffer[8];  //前4个字节是宽度，后4个字节是高度
    ifs.seekg(0x10);          //宽高信息相对于文件头偏移16个字节
    ifs.read((char*)&buffer, sizeof(char) * 8);
    ret.w = buffer[0] * 65536 + buffer[1] * 4096 + buffer[2] * 256 + buffer[3];
    ret.h = buffer[4] * 65536 + buffer[5] * 4096 + buffer[6] * 256 + buffer[7];
    ifs.close();
    return ret;
}
imgopt::Size imgopt::ImgOpt::GetBmpSize(std::ifstream& ifs) {
    Size ret = {0, 0};
    unsigned char buffer[8];  //前4个字节是宽度，后4个字节是高度
    ifs.seekg(18);            //宽高信息相对于文件头偏移18个字节
    ifs.read((char*)&buffer, sizeof(char) * 8);
    ret.w = buffer[3] * 65536 + buffer[2] * 4096 + buffer[1] * 256 + buffer[0];
    ret.h = buffer[7] * 65536 + buffer[6] * 4096 + buffer[5] * 256 + buffer[4];
    ifs.close();
    return ret;
}
imgopt::Size imgopt::ImgOpt::GetWebpSize(std::ifstream& ifs) {
    Size ret = {0, 0};
    unsigned char buffer[4];  //前2个字节是宽度，后2个字节是高度
    ifs.seekg(26);            //宽高信息相对于文件头偏移26个字节
    ifs.read((char*)&buffer, sizeof(char) * 4);
    ret.w = buffer[0] + buffer[1] * 256;
    ret.h = buffer[2] + buffer[3] * 256;
    ifs.close();
    return ret;
}
imgopt::Size imgopt::ImgOpt::GetGifSize(std::ifstream& ifs) {
    Size ret = {0, 0};
    unsigned char buffer[4];  //前2个字节是宽度，后2个字节是高度
    ifs.seekg(6);             //宽高信息相对于文件头偏移6个字节
    ifs.read((char*)&buffer, sizeof(char) * 4);
    ret.w = buffer[0] + buffer[1] * 256;
    ret.h = buffer[2] + buffer[3] * 256;
    ifs.close();
    return ret;
}
imgopt::Size imgopt::ImgOpt::GetTiffIISize(std::ifstream& ifs) {
    unsigned char buffer[4];
    ifs.seekg(0x1e);  //宽
    ifs.read((char*)&buffer, sizeof(char) * 4);
    int w = buffer[0] + buffer[1] * 256;
    ifs.seekg(0x2a);  //高
    ifs.read((char*)&buffer, sizeof(char) * 4);
    int h = buffer[0] + buffer[1] * 256;
    ifs.close();
    return {w, h};
}
imgopt::Size imgopt::ImgOpt::GetTiffMMSize(std::ifstream& ifs) {
    unsigned char buffer[4];
    ifs.seekg(0x1e);  //宽
    ifs.read((char*)&buffer, sizeof(char) * 4);
    int w = buffer[0] * 256 + buffer[1];
    ifs.seekg(0x2a);  //高
    ifs.read((char*)&buffer, sizeof(char) * 4);
    int h = buffer[0] * 256 + buffer[1];
    ifs.close();
    return {w, h};
}