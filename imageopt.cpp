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
    unsigned char buffer[8];
    ifs.read((char*)&buffer, sizeof(unsigned char) * 8);
    // jpg header(hex) FF D8 FF
    if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff) {
        return JPG;
    }
    // png header(hex) 89 50 4E 47 0D 0A 1A 0A
    if (buffer[0] == 0x89) {
        unsigned char png_header[8] = {0x89, 0x50, 0x4E, 0x47,
                                       0x0D, 0x0A, 0x1A, 0x0A};
        bool is_png = true;
        for (int i = 0; i < 8; ++i) {
            if (buffer[i] != png_header[i]) {
                is_png = false;
                break;
            }
        }
        if (is_png) {
            return PNG;
        }
    }
    // bmp header(hex) 42 4D
    if (buffer[0] == 0x42 && buffer[1] == 0x4d) {
        return BMP;
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
    Size image_size;
    if (!ifs.is_open()) {
        printf("Error in Reading ImageFile");
        return {0, 0};
    } else {
        ImageType type = this->GetImageType(ifs);
        if (JPG == type) {
            unsigned char buffer[4];  //前2个字节是高度，后2个字节是宽度
            ifs.seekg(163);  //宽高信息相对于文件头偏移163个字节
            ifs.read((char*)&buffer[0], sizeof(buffer) * 4);
            image_size.h = buffer[0] * 256 + buffer[1];
            image_size.w = buffer[2] * 256 + buffer[3];
            ifs.close();
            return image_size;
        }
        if (PNG == type) {
            unsigned char buffer[8];  //前4个字节是宽度，后4个字节是高度
            ifs.seekg(16);  //宽高信息相对于文件头偏移16个字节
            ifs.read((char*)&buffer[0], sizeof(buffer) * 8);
            image_size.w = buffer[0] * 65536 + buffer[1] * 4096 +
                           buffer[2] * 256 + buffer[3];
            image_size.h = buffer[4] * 65536 + buffer[5] * 4096 +
                           buffer[6] * 256 + buffer[7];
            ifs.close();
            return image_size;
        }
        if (BMP == type) {
            unsigned char buffer[8];  //前4个字节是宽度，后4个字节是高度
            ifs.seekg(18);  //宽高信息相对于文件头偏移16个字节
            ifs.read((char*)&buffer[0], sizeof(buffer) * 8);
            image_size.w = buffer[3] * 65536 + buffer[2] * 4096 +
                           buffer[1] * 256 + buffer[0];
            image_size.h = buffer[7] * 65536 + buffer[6] * 4096 +
                           buffer[5] * 256 + buffer[4];
            ifs.close();
            return image_size;
        }
    }
#else
    FILE* fin = fopen(imageFilePath.c_str(), "rb+");
    while (!feof(fin)) {
        cout << fgetc(fin) << endl;
    }
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
void imgopt::ImgOpt::GetImageSize(const std::string imageFilePath,
                                  int& width,
                                  int& height) {
    imgopt::Size s = {0, 0};
    s = this->GetImageSize(imageFilePath);
    width = s.w;
    height = s.h;
}