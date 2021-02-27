/*
 * @Author: your name
 * @Date: 2021-02-22 19:38:50
 * @LastEditTime: 2021-02-26 22:10:05
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \VideoPlayer\main.cpp
 */
#include <QApplication>
#ifdef WIN32
#include <Windows.h>
#endif
#include "Service/servicefacetory.h"
#include "mainwindow.h"

extern "C" {
//确保连接了nvidia的显示器
//    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//确保连接了amd的显示器
//    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x00000001;
}

#ifdef WIN32
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif

#pragma execution_character_set("utf-8")
int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication a(argc, argv);
    a.setApplicationName(QObject::tr("铁塔视频选点"));
    ServiceFactoryI *facetoryI = new ServiceFactory;
    a.setProperty(FACETORY_KEY, reinterpret_cast<unsigned long long>(facetoryI));

    MainWindow w;
    w.resize(1395, 956);
    // w.setWindowFlag(Qt::FramelessWindowHint);
    w.show();

    QObject::connect(qApp, &QApplication::aboutToQuit, [&]{
        delete facetoryI;
    });
    return a.exec();
}
