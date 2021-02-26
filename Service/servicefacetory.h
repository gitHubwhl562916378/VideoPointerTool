/*
 * @Author: your name
 * @Date: 2021-02-26 11:53:59
 * @LastEditTime: 2021-02-26 18:06:54
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \VideoPointerTool\Service\servicefacetory.h
 */
#ifndef SERVICEFACETORY_H
#define SERVICEFACETORY_H

#include <QNetworkAccessManager>
#include "servicei.h"
#include "restserviceconcurrent.h"
#include "resetserviceqt.h"
#include "videoencodebyffmpeg.h"
class ServiceFactory : public ServiceFactoryI
{
public:
    ServiceFactory() = default;
    inline NotifyServiceI* makeNotifyServiceI(NotifyInterfaceType s) override{
        switch (s) {
        default:
            break;
        }
        return nullptr;
    }
    inline RestServiceI* makeRestServiceI(RestInterfaceType s) override{
        RestServiceI *ptr{nullptr};
        switch (s) {
        case HttplibConcurrent:
            ptr = new RestServiceConcurrent;
            break;
        case QNetworkConcurrent:
            if(!network_access_manager_)
            {
                network_access_manager_ = new QNetworkAccessManager;
            }
            ptr = new RestServiceQt(network_access_manager_);
        }
        return ptr;
    }
    inline VideoEncodeI* makeVideoEncodeI(VideoEncodeType t = FFmpeg) override{
        VideoEncodeI *ptr{nullptr};
        switch (t) {
        case FFmpeg:
            ptr = new VideoEncodeByFFmpeg;
            break;
        }
        return ptr;
    }

private:
    QNetworkAccessManager *network_access_manager_ = nullptr;
};
#endif // SERVICEFACETORY_H
