/*
 * @Author: your name
 * @Date: 2021-02-22 19:38:49
 * @LastEditTime: 2021-02-23 14:31:03
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \VideoPlayer\Service\servicei.h
 */
#ifndef SERVICEI_H
#define SERVICEI_H

#include <QObject>
#include <QList>
#include <QThread>

class RestServiceI;
class VideoEncodeI;
class NotifyServiceI;
class ServiceFactoryI
{
public:
    enum RestInterfaceType{
        Concurrent
    };
    enum NotifyInterfaceType{
        Mqtt,
        Kafka,
        WebSocket
    };
    enum VideoEncodeType{
        FFmpeg
    };

    virtual ~ServiceFactoryI(){}
    virtual NotifyServiceI* makeNotifyServiceI(NotifyInterfaceType s = Mqtt) = 0;
    virtual RestServiceI* makeRestServiceI(RestInterfaceType s = Concurrent) = 0;
    virtual VideoEncodeI* makeVideoEncodeI(VideoEncodeType t = FFmpeg) = 0;
};

class RestServiceI : public QObject
{
    Q_OBJECT
public:
    struct CameraData
    {
        QString cameraName;
        QString cameraIndexCode;
        QString rtsp;
        int status;
        QString cameraTypeName;
        QString recordLocationName;
        QString statusName;
        QString treatyTypeName;
    };
    
    struct CameraInfo
    {
        int total;
        int pageNo;
        QList<CameraData> datas;
    };
    
    struct CameraInfoArgs
    {
        int pageNo;
        int pageSize;
    };
    
    RestServiceI(QObject *parent = nullptr):QObject(parent){
        qRegisterMetaType<RestServiceI::CameraInfo>("RestServiceI::CameraInfo");
    }

    virtual void getCameras(const CameraInfoArgs &args) = 0;

signals:
    void sigError(QString);
    void sigCameras(RestServiceI::CameraInfo);
};

class VideoEncodeI : public QThread
{
    Q_OBJECT

public:
    struct EncodeParams
    {
         char input_file[258];
         char out_file[258];
         int fps;
         int code_id;
         int media_type;
         int pix_fmt;
         int width;
         int height;
         int gop_size;
         int keyint_min;
         int thread_count;
         int me_range = 16;
         int max_qdiff;
         float qcompress;
         int max_b_frames;
         bool b_frame_strategy;
         int qmin;
         int qmax;
         int bit_rate;
    };

    VideoEncodeI(QObject* parent = nullptr): QThread(parent){
    }

    virtual void startEncode(const EncodeParams& params) = 0;
    virtual void stopEncode() = 0;

signals:
    void sigError(QString);
    void sigStoped();
    void sigStarted();
};

#endif // SERVICEI_H
