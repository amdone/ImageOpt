<!--
 * @Author: amdone
 * @Date: 2022-04-12 11:57:24
 * @LastEditors: amdone
 * @LastEditTime: 2022-04-14 14:42:57
 * @FilePath: \ImageOpt\README.md
 * @Description: 
 * 
 * Copyright (c) 2022 by amdone, All Rights Reserved. 
-->
# 图片处理
（仅某些C++库没有的函数）

## 目前拥有的功能

>- 获得图片的尺寸（不加载整张图片，仅读取文件头部信息）
>- ...

## 获取图片的尺寸（宽高）仅读取图片文件头部信息
该函数只读取文件头部数据，不会加载图片文件全部数据至内存

> 支持的图片格式：jpg, png, webp, bmp, tiff, gif
```cpp
    imgopt::ImgOpt opt;
    imgopt::Size s = opt.GetImageSize("./1.jpg");
    std::cout << s.w << "x" << s.h << std::endl;
```


