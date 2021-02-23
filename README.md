<!--
 * @Author: your name
 * @Date: 2021-02-23 17:43:02
 * @LastEditTime: 2021-02-23 17:50:44
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \VideoPointerTool\README.md
-->
# 视频选点工具
结合视频播放与业务后台，视频选点

## 编译方法
### Windows
>+ mkdir build
>+ cd build 
>+ cmake .. -G"NMake Makefiles"
>+ nmake

### linux
>+ mkdir  build
>+ cmake ..
>+ make -j4

## 运行
>+ 将根目录下的images文件夹放到运行路径
>+ 将根目录下m_plugins中的debug或者release中的dll放到运行路径(根据编译版本决定，仅限windows)