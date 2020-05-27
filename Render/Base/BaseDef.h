#pragma once

//跨平台标准库
#include <algorithm> //算法
#include <vector> //数组
#include <map>    //字典
#include <cstdio> //读写
#include <ctime>  //时间
#include <cmath>  //数学
#include <cfloat> //浮点
using namespace std;

//系统基础库
#ifdef _MSC_VER
#include <Windows.h>
#endif

//基础类型定义
typedef unsigned char uchar;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;