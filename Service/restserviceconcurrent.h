/*
 * @Author: your name
 * @Date: 2021-02-22 19:38:49
 * @LastEditTime: 2021-02-23 11:41:14
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \VideoPlayer\Service\restserviceconcurrent.h
 */
#ifndef RESTSERVICECONCURRENT_H
#define RESTSERVICECONCURRENT_H

#include "servicei.h"
#include "../Dao/HikOpenSdk.h"
class RestServiceConcurrent : public RestServiceI
{
    Q_OBJECT
public:
    RestServiceConcurrent(QObject *parent = nullptr);
    ~RestServiceConcurrent() override;
    void getCameras(const CameraInfoArgs &args) override;

private:
    HikOpenSdk *sdk_ptr_;
};

#endif // RESTSERVICECONCURRENT_H
