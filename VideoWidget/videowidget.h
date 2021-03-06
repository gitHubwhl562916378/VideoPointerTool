/*
 * @Author: your name
 * @Date: 2021-02-22 19:38:50
 * @LastEditTime: 2021-03-02 18:19:26
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \VideoPointerTool\VideoWidget\videowidget.h
 */
#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <atomic>
#include <QOpenGLWidget>
QT_FORWARD_DECLARE_CLASS(RenderThread)
class VideoWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    enum PlayState{
        Ready,
        Play,
        Stopped
    };
    VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget() override;
    QString url() const;
    QString deviceName() const;
    PlayState state() const;

public slots:
    void slotPlay(QString filename, QString device);
    void slotStop();
    void slotPhotoShot();

signals:
    void sigInitized();

    void sigError(QString);
    void sigVideoStarted(int, int);
    void sigVideoStopped();
    void sigFps(int);
    void sigCurFpsChanged(int);
    void sigPhotoShot(QImage);
    void sigPhotoShotError(QString);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private slots:
    void slotFinished();

private:
    RenderThread *m_thread = nullptr;
    QString source_file_, device_name_;
    PlayState m_state_;
    bool is_initized_ = false;
};

#endif // VIDEOWIDGET_H
