/*
 * @Author: your name
 * @Date: 2021-02-22 19:38:49
 * @LastEditTime: 2021-02-27 21:39:11
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \VideoPlayer\Service\restserviceconcurrent.cpp
 */
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include "resetserviceqt.h"

RestServiceQt::RestServiceQt(QNetworkAccessManager *net_manager, QObject *parent):RestServiceI(parent)
{
    network_manager_ = net_manager;
    address_ = "https://220.163.129.62:1443";
	app_key_ = "21355400";
	app_secret_ = "GJTYYwZ35WhvZI4jsMHg";
}

RestServiceQt::~RestServiceQt()
{
}

void RestServiceQt::getCameras(const CameraInfoArgs &args)
{
    QJsonObject jsBody{
        {"pageNo", args.pageNo},
        {"pageSize", args.pageSize},
        {"treeCode", "0"}
        };
    QJsonDocument jsDoc(jsBody);
    QByteArray byte_body = jsDoc.toJson();
    
    QString path = "/artemis/api/resource/v1/cameras";
    QNetworkRequest request(address_ + path);
    request.setAttribute(QNetworkRequest::AutoDeleteReplyOnFinishAttribute, true);
    request.setRawHeader("Accept", "*/*");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-Ca-Key", app_key_.toLatin1());
    request.setRawHeader("X-Ca-Signature-Headers", "x-ca-key");
    request.setHeader(QNetworkRequest::ContentLengthHeader, byte_body.size());
    request.setRawHeader("X-Ca-Signature", GenerateSignature(request , path));

    QSslConfiguration sslConf = request.sslConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConf);

    request.setTransferTimeout(20000);

    QNetworkReply* resp = network_manager_->post(request, byte_body);
    connect(resp, &QNetworkReply::finished, this, [this, resp]{
        if(resp->error() != QNetworkReply::NoError)
        {
            emit sigError(resp->errorString());
            deleteLater();
            return;
        }
        QByteArray resp_body = resp->readAll();
        
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
        fwatcher->setFuture(QtConcurrent::run([=]()->QString{
            QJsonParseError jsError;
            QJsonDocument jsDoc = QJsonDocument::fromJson(resp_body, &jsError);
            if(jsError.error != QJsonParseError::NoError){
                return jsError.errorString();
            }

            QJsonObject jsObj = jsDoc.object();
            QString code = jsObj.value("code").toString();
            if(code != "0")
            {
                return jsObj.value("msg").toString();
            }

            QJsonObject js_data = jsObj.value("data").toObject();
            resData->pageNo = js_data.value("pageNo").toInt();
            resData->total = js_data.value("total").toInt();
            QJsonArray js_data_list = js_data.value("list").toArray();
            std::transform(js_data_list.begin(),js_data_list.end(),std::back_inserter(resData->datas),[this](QJsonValue val){
                RestServiceI::CameraData data;
                
                QJsonObject js_camera_obj = val.toObject();
                data.cameraIndexCode = js_camera_obj.value("cameraIndexCode").toString();
                data.cameraName = js_camera_obj.value("name").toString();
                data.cameraTypeName = js_camera_obj.value("cameraTypeName").toString();
                data.recordLocationName = js_camera_obj.value("recordLocationName").toString();
                data.status = js_camera_obj.value("status").toInt();
                data.statusName = js_camera_obj.value("statusName").toString();
                data.treatyTypeName = js_camera_obj.value("treatyTypeName").toString();
                return data;
            });
            
            return QString();
        }));
    });
}

void RestServiceQt::getRtspUrl(const QString &cameraIndexCode)
{
    QJsonObject jsBody{
        {"cameraIndexCode", cameraIndexCode}
    };
    QJsonDocument jsDoc(jsBody);
    QByteArray byte_body = jsDoc.toJson();
    
    QString path = "/artemis/api/video/v1/cameras/previewURLs";
    QNetworkRequest request(address_ + path);
    request.setAttribute(QNetworkRequest::AutoDeleteReplyOnFinishAttribute, true);
    request.setRawHeader("Accept", "*/*");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-Ca-Key", app_key_.toLatin1());
    request.setRawHeader("X-Ca-Signature-Headers", "x-ca-key");
    request.setHeader(QNetworkRequest::ContentLengthHeader, byte_body.size());
    request.setRawHeader("X-Ca-Signature", GenerateSignature(request , path));

    QSslConfiguration sslConf = request.sslConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConf);

    request.setTransferTimeout(20000);

    QNetworkReply* resp = network_manager_->post(request, byte_body);
    connect(resp, &QNetworkReply::finished, this, [this, resp]{
        if(resp->error() != QNetworkReply::NoError)
        {
            emit sigError(resp->errorString());
            deleteLater();
            return;
        }
        QByteArray resp_body = resp->readAll();
        
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
        fwatcher->setFuture(QtConcurrent::run([=]()->QString{
            QJsonParseError jsError;
            QJsonDocument jsDoc = QJsonDocument::fromJson(resp_body, &jsError);
            if(jsError.error != QJsonParseError::NoError){
                return jsError.errorString();
            }

            QJsonObject jsObj = jsDoc.object();
            QString code = jsObj.value("code").toString();
            if(code != "0")
            {
                return jsObj.value("msg").toString();
            }

            QJsonObject js_data = jsObj.value("data").toObject();
            *resData = js_data.value("url").toString();
            
            return QString();
        }));
    });
}

QByteArray RestServiceQt::GenerateSignature(const QNetworkRequest &req, const QString &path)
{
    QByteArray text("POST\n");
    text += req.rawHeader("Accept") + "\n";
    text += req.header(QNetworkRequest::ContentTypeHeader).toByteArray() + "\n";
    text += req.rawHeader("X-Ca-Signature-Headers") + ":" + app_key_ + "\n";
    text += path;

    return QMessageAuthenticationCode::hash(text, app_secret_.toLatin1(), QCryptographicHash::Sha256).toBase64();
}