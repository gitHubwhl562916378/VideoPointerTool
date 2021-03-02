/*
 * @Author: your name
 * @Date: 2021-02-22 19:38:50
 * @LastEditTime: 2021-03-02 18:19:04
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \VideoPointerTool\VideoWidget\renderthread.h
 */
#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QImage>
#include "render/factory.h"
#include "render/videorender.h"
QT_FORWARD_DECLARE_CLASS(QOpenGLContext)
QT_FORWARD_DECLARE_CLASS(QSurface)
class RenderThread : public QThread
{
    Q_OBJECT
public:
    RenderThread(QSurface *surface, QOpenGLContext *ctx = nullptr, QObject *parent = nullptr);
    ~RenderThread() override;
    virtual QMutex* renderLocker(){ return  &render_mtx; }
    VideoRender* currentRender() {return  render_;}

    virtual VideoRender* getRender(int);
    virtual void Render(const std::function<void (void)>);
    virtual void setExtraData(void *ctx){exte_data_ = ctx;}

    void setFileName(QString);
    void setDevice(QString);
    std::atomic_bool& photoShot();

signals:
    void sigError(QString);
    void sigVideoStarted(int, int);
    void sigFps(int);
    void sigCurFpsChanged(int);
    void sigPhotoShot(QImage);
    void sigPhotoShotError(QString);

    void sigTextureReady();

protected:
    void run() override;

private:
    static Factory<VideoRender, int>* renderFactoryInstance();
    static std::atomic_bool isInited_;

    QOpenGLContext *context_;
    QSurface *surface_;
    VideoRender *render_ = nullptr;
    QMutex render_mtx;

    QString file_name_, device_;
    void *exte_data_{nullptr};
    std::atomic_bool need_photo_shot_;
};

#endif // RENDERTHREAD_H
