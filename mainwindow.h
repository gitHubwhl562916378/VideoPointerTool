/*
 * @Author: your name
 * @Date: 2021-02-22 19:38:50
 * @LastEditTime: 2021-02-23 17:02:36
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \VideoPlayer\mainwindow.h
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Service/servicei.h"
#include "widgetinterface.h"
QT_FORWARD_DECLARE_CLASS(QTableWidget)
QT_FORWARD_DECLARE_CLASS(Player)
QT_FORWARD_DECLARE_CLASS(PageIndicator)
QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QGroupBox)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QTableWidgetItem)
QT_FORWARD_DECLARE_CLASS(WaitingLabel)
class MainWindow : public WidgetI
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setUserStyle(int s = 0) override;
    QSize sizeHint() const override;
    
protected:
    bool event(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void slotFlushBtnClicked();
    void slotPageindicatorActived(int);
    void slotGetCameras(RestServiceI::CameraInfo);
    void slotTableItemDClicked(QTableWidgetItem*);
    
private:
    QTableWidget *tableW_;
    PageIndicator *pageIndicator_;
    QComboBox *decodeCombox_;
    Player *videoPlayer_;
    QGroupBox *videoGroupbox_;
    QPushButton *flushAllBtn_;
    bool cameraInfoPageNeedInited_;
    int table_row_count_ = 10;

    WaitingLabel *video_wait_label_ = nullptr;
};

#endif // MAINWINDOW_H
