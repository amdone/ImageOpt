/*
 * @Author: amdone
 * @Date: 2022-04-11 20:07:54
 * @LastEditors: amdone
 * @LastEditTime: 2022-04-11 23:50:08
 * @FilePath: \Test\imageopt.h
 * @Description:
 *
 * Copyright (c) 2022 by amdone, All Rights Reserved.
 */
#include <fstream>

/***
 * @brief imgopt命名空间
 */
namespace imgopt {

/***
 * @brief 图片类型的枚举值，0表示不是一个图片
 */
enum ImageType { NOT_IMAGE, JPG, PNG, BMP };

/***
 * @brief This is a struct --> { w, h }
 */
struct Size {
    int w, h;
};

/***
 * @brief 这个类包含着对图片的一些操作函数
 * @param * 暂时没有参数
 */
class ImgOpt {
  public:
    /**
     * @brief 通过读取文件头获得该文件是否是图片文件，并且返回实际的图片类型
     * @param ifstream& 已经打开的图片文件二进制输入流
     * @return {enum -> imgopt::ImageType}
     * 0 -> NOT_IMAGE,
     * 1 -> JPG,
     * 2 -> PNG,
     * 3 -> BMP
     */
    ImageType GetImageType(std::ifstream& ifs);
    /***
     * @brief 仅通过读取文件头部数据来得到图片的宽高
     * @param string imageFilePath 图片文件的实际路径
     * @return imgopt::Size {w,h} 一旦得到 {0,0} 即为失败
     */
    Size GetImageSize(const std::string imageFilePath);
    /***
     * @brief 仅通过读取文件头部数据来得到图片的宽高
     * @param string imageFilePath 图片文件的实际路径
     * @param int& width 保存宽度到这个变量
     * @param int& height 保存高度度到这个变量
     * @return void 虽然没有返回值，但是一旦得到width或height为 0 即为失败
     */
    void GetImageSize(const std::string imageFilePath, int& width, int& height);
};
}  // namespace imgopt