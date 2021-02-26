/*
 * @Author: your name
 * @Date: 2021-02-22 19:38:49
 * @LastEditTime: 2021-02-26 21:35:15
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \VideoPlayer\Service\restserviceqt.h
 */
#ifndef RESETSERVICEQT_H
#define RESETSERVICEQT_H

#include "servicei.h"
QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkRequest)
class RestServiceQt : public RestServiceI
{
    Q_OBJECT
public:
    RestServiceQt(QNetworkAccessManager *net_manager, QObject *parent = nullptr);
    ~RestServiceQt() override;
    void getCameras(const CameraInfoArgs &args) override;
    void getRtspUrl(const QString &cameraIndexCode) override;

private:
    QByteArray GenerateSignature(const QNetworkRequest &req, const QString &path);

    QNetworkAccessManager *network_manager_;
    QString address_, app_key_, app_secret_;
};

#endif // RESETSERVICEQT_H
