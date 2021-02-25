/*
 * @Author: your name
 * @Date: 2021-02-22 19:38:50
 * @LastEditTime: 2021-02-25 14:09:15
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \VideoPlayer\mainwindow.cpp
 */
#include <cmath>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QDebug>
#include <QPushButton>
#include <QApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include "player.h"
#include "pageindicator.h"
#include "Service/servicefacetory.h"
#include "waitinglabel.h"
#include "mainwindow.h"

#pragma execution_character_set("utf-8")
MainWindow::MainWindow(QWidget *parent)
    : WidgetI(parent)
{
    setObjectName(tr("主窗口"));
    flushAllBtn_ = new QPushButton(tr("刷新"));
    flushAllBtn_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    tableW_ = new QTableWidget;
    pageIndicator_ = new PageIndicator;
    videoPlayer_ = new Player;
    videoPlayer_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    decode_label_ = new QLabel(tr("解码方式"));
    decodeCombox_ = new QComboBox;
    videoGroupbox_ = new QGroupBox;
    playerMenu_ = new QMenu(videoPlayer_);

    QVBoxLayout *mainLay = new QVBoxLayout;

    QVBoxLayout *vlay = new QVBoxLayout;
    vlay->addWidget(tableW_);
    QHBoxLayout *hlay = new QHBoxLayout;
    hlay->addWidget(flushAllBtn_);
    hlay->addStretch();
    hlay->addWidget(pageIndicator_);
    vlay->addLayout(hlay);
    mainLay->addLayout(vlay,2);

    vlay = new QVBoxLayout;
    vlay->addWidget(decode_label_);
    vlay->addWidget(decodeCombox_);
    vlay->addStretch();
    hlay = new QHBoxLayout;
    hlay->addLayout(vlay);
    hlay->addWidget(videoPlayer_);
    videoGroupbox_->setLayout(hlay);
    mainLay->addWidget(videoGroupbox_,3);

    mainLay->setMargin(5);
    setLayout(mainLay);

    tableW_->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStringList headers_list;
    headers_list << tr("cameraName") << tr("cameraIndexCode") << tr("cameraTypeName") << tr("status") << tr("statusName") << tr("treatyTypeName");
    tableW_->setColumnCount(headers_list.count());
    tableW_->setHorizontalHeaderLabels(headers_list);
    tableW_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableW_->setEditTriggers(QTableWidget::NoEditTriggers);
    decode_label_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    videoPlayer_->setContextMenuPolicy(Qt::CustomContextMenu);
    playerMenu_->addAction(tr("输入cameraIndexCode播放"),[&]{
        bool is_ok;
        QString cameraIndexCode = QInputDialog::getText(this, tr("播放rtsp"), tr("cameraIndexCode"), QLineEdit::Normal, QString(), &is_ok);
        if(!is_ok)
        {
            return;
        }
        PlayVideoByCameraIndexCode(cameraIndexCode);
    });
    playerMenu_->addAction(tr("输入rtsp播放"),[&]{
        bool is_ok;
        QString rtsp = QInputDialog::getText(this, tr("播放rtsp"), tr("rtsp"), QLineEdit::Normal, QString(), &is_ok);
        if(!is_ok)
        {
            return;
        }
        videoPlayer_->setToolTip(rtsp);
        videoPlayer_->startPlay(rtsp, decodeCombox_->currentText());
    });
    connect(videoPlayer_,&Player::customContextMenuRequested,this,[&](const QPoint &p){
        playerMenu_->move(videoPlayer_->mapToGlobal(p));
        playerMenu_->show();
    });

    decodeCombox_->addItems(QStringList() << "cpu" << "qsv" << "cuda" << "cuda_plugin");
    pageIndicator_->setEnabled(false);
    connect(flushAllBtn_, SIGNAL(clicked()), this, SLOT(slotFlushBtnClicked()));
    connect(pageIndicator_, SIGNAL(sigPageClicked(int)), this, SLOT(slotPageindicatorActived(int)));
    connect(tableW_, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(slotTableItemDClicked(QTableWidgetItem*)));
    connect(videoPlayer_, SIGNAL(sigVideoStarted(int,int)), this, SLOT(slotOnVideoStarted(int,int)));
    connect(videoPlayer_, SIGNAL(sigError(QString)), this, SLOT(slotOnVideoError(QString)));

    setUserStyle(userStyle());
}

MainWindow::~MainWindow()
{

}

void MainWindow::setUserStyle(int s)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(73,89,108));
    setPalette(pal);
    setAutoFillBackground(true);
    pageIndicator_->setUserStyle();

    pal = decode_label_->palette();
    pal.setColor(QPalette::Foreground, Qt::white);
    decode_label_->setPalette(pal);
}

QSize MainWindow::sizeHint() const
{
    return QSize(1367, 953);    
}

bool MainWindow::event(QEvent *event)
{   
    return WidgetI::event(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    qDebug() << size();
}

void MainWindow::slotFlushBtnClicked()
{
    cameraInfoPageNeedInited_ = true;
    slotPageindicatorActived(1);
}

void MainWindow::slotPageindicatorActived(int pageNo)
{
    ServiceFactoryI *serI = reinterpret_cast<ServiceFactoryI*>(qApp->property(FACETORY_KEY).toULongLong());
    RestServiceI *service = serI->makeRestServiceI();
    WaitingLabel *label = new WaitingLabel(this);
    connect(service, &RestServiceI::sigCameras, this, [this, label](RestServiceI::CameraInfo infos){
        label->close();
        delete label;
        pageIndicator_->setEnabled(true);
        flushAllBtn_->setEnabled(true);
        slotGetCameras(infos);
    });
    connect(service, &RestServiceI::sigError, this, [&, label](QString str){
        label->close();
        delete label;
        pageIndicator_->setEnabled(true);
        flushAllBtn_->setEnabled(true);
        QMessageBox::information(this, tr("获取摄像头"), str);
    });
    RestServiceI::CameraInfoArgs args;
    args.pageNo = pageNo;
    args.pageSize = table_row_count_;
    service->getCameras(args);
    pageIndicator_->setEnabled(false);
    flushAllBtn_->setEnabled(false);
    label->show(500);
}

void MainWindow::slotGetCameras(RestServiceI::CameraInfo infos)
{
    if(cameraInfoPageNeedInited_){
        int pageNo = infos.pageNo;
        int totalRecord = infos.total;
        pageIndicator_->setPageInfo(::ceil((float)totalRecord/table_row_count_), totalRecord);
        cameraInfoPageNeedInited_ = false;
    }
    pageIndicator_->adjustRow();
    tableW_->model()->removeRows(0,tableW_->rowCount());
    foreach (const RestServiceI::CameraData &camera, infos.datas) {
        tableW_->insertRow(tableW_->rowCount());
        tableW_->setItem(tableW_->rowCount() - 1,0,new QTableWidgetItem(camera.cameraName));
        tableW_->setItem(tableW_->rowCount() - 1,1,new QTableWidgetItem(camera.cameraIndexCode));
        tableW_->setItem(tableW_->rowCount() - 1,2,new QTableWidgetItem(camera.cameraTypeName));
        tableW_->setItem(tableW_->rowCount() - 1,3,new QTableWidgetItem(QString::number(camera.status)));
        tableW_->setItem(tableW_->rowCount() - 1,4,new QTableWidgetItem(camera.statusName));
        tableW_->setItem(tableW_->rowCount() - 1,5,new QTableWidgetItem(camera.treatyTypeName));
    }
}

void MainWindow::slotTableItemDClicked(QTableWidgetItem* item)
{
    PlayVideoByCameraIndexCode(tableW_->item(item->row(), 1)->text());
}

void MainWindow::slotOnVideoStarted(int w,int h)
{
    if(video_wait_label_)
    {
        video_wait_label_->close();
        delete video_wait_label_;
        video_wait_label_ = nullptr; 
    }
    pageIndicator_->setEnabled(true);
    flushAllBtn_->setEnabled(true);
    tableW_->setEnabled(true);
}

void MainWindow::slotOnVideoError(QString msg)
{
    if(video_wait_label_)
    {
        video_wait_label_->close();
        delete video_wait_label_;
        video_wait_label_ = nullptr; 
    }
    pageIndicator_->setEnabled(true);
    flushAllBtn_->setEnabled(true);
    tableW_->setEnabled(true);
    QMessageBox::information(this, tr("播放视频"), videoPlayer_->toolTip() + ": " + msg);
}

void MainWindow::PlayVideoByCameraIndexCode(const QString &cameraIndexCode)
{
    video_wait_label_ = new WaitingLabel(videoPlayer_);
    
    ServiceFactoryI *serI = reinterpret_cast<ServiceFactoryI*>(qApp->property(FACETORY_KEY).toULongLong());
    RestServiceI *service = serI->makeRestServiceI();
    connect(service, &RestServiceI::sigRtspUrl, this, [this](QString rtsp){
        videoPlayer_->setToolTip(rtsp);
        videoPlayer_->startPlay(rtsp, decodeCombox_->currentText());
    });
    connect(service, &RestServiceI::sigError, this, [&](QString str){
        if(video_wait_label_)
        {
            video_wait_label_->close();
            delete video_wait_label_;
            video_wait_label_ = nullptr; 
        }
        pageIndicator_->setEnabled(true);
        flushAllBtn_->setEnabled(true);
        tableW_->setEnabled(true);
        QMessageBox::information(this, tr("获取rtsp"), str);
    });

    pageIndicator_->setEnabled(false);
    flushAllBtn_->setEnabled(false);
    tableW_->setEnabled(false);
    service->getRtspUrl(cameraIndexCode);
    video_wait_label_->show(500);
}