/*
 * @Author: your name
 * @Date: 2021-02-22 19:38:49
 * @LastEditTime: 2021-02-26 18:05:56
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \VideoPlayer\Service\restserviceconcurrent.cpp
 */
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QDebug>
#include "restserviceconcurrent.h"

RestServiceConcurrent::RestServiceConcurrent(QObject *parent):RestServiceI(parent)
{
    sdk_ptr_ = new HikOpenSdk;
}

RestServiceConcurrent::~RestServiceConcurrent()
{
    delete sdk_ptr_;
}

void RestServiceConcurrent::getCameras(const CameraInfoArgs &args)
{
    RestServiceI::CameraInfo *resData = new RestServiceI::CameraInfo;
    QFutureWatcher<QString> *fwatcher = new QFutureWatcher<QString>(this);
    connect(fwatcher, &QFutureWatcher<QString>::finished, this, [=]{
        if(fwatcher->result().isEmpty())
        {
            emit sigCameras(*resData);
        }else{
            emit sigError(fwatcher->result());
        }
        delete resData;
    });
    connect(fwatcher, SIGNAL(finished()), this, SLOT(deleteLater()));
    fwatcher->setFuture(QtConcurrent::run(sdk_ptr_, &HikOpenSdk::QueryCameraList, args.pageNo, args.pageSize, resData));
}

void RestServiceConcurrent::getRtspUrl(const QString &cameraIndexCode)
{
    QString *resData = new QString;
    QFutureWatcher<QString> *fwatcher = new QFutureWatcher<QString>(this);
    connect(fwatcher, &QFutureWatcher<QString>::finished, this, [=]{
        if(fwatcher->result().isEmpty())
        {
            emit sigRtspUrl(*resData);
        }else{
            emit sigError(fwatcher->result());
        }
        delete resData;
    });
    connect(fwatcher, SIGNAL(finished()), this, SLOT(deleteLater()));
    fwatcher->setFuture(QtConcurrent::run(sdk_ptr_, &HikOpenSdk::QueryURL, cameraIndexCode, resData));
}