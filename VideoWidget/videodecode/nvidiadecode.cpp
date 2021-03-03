/*
 * @Author: your name
 * @Date: 2020-08-03 18:38:52
 * @LastEditTime: 2021-03-03 17:52:27
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \qt_project\VideoWidget\VideoWidget\videodecode\nvidiadecode.cpp
 */
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/pixdesc.h"
#include "libavutil/opt.h"
#include "libavutil/avassert.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}
#include <QDateTime>
#include <QDebug>
#include "../renderthread.h"
#include "nvidiadecode.h"

NvidiaDecode::NvidiaDecode(DecodeTaskManagerImpl *taskManger, CreateDecoderFunc func, RenderThread *render_thr):
    DecodeTask(render_thr),
    taskManager_(taskManger),
    createDecoderFunc_(func)
{

}

NvidiaDecode::~NvidiaDecode()
{
    qDebug() << "NvidiaDecode::~NvidiaDecode()";
    if(decoder_){
        delete decoder_;
        decoder_ = nullptr;
    }
}

extern CreateDecoderFunc g_gpurender_fuc_;
void NvidiaDecode::decode(const QString &url)
{
    if(!createDecoderFunc_){
        return;
    }

    decoder_ = createDecoderFunc_();
    if(!decoder_){
        return;
    }
    std::string error;
    decoder_->decode(url.toStdString().data(), true, [&](void* ptr, const int pix, const int width, const int height, const std::string &err){
        if(thread()->isInterruptionRequested()){
            decoder_->stop();
        }
        error = err;
        if(!err.empty())
        {
            thread()->sigError(QString::fromStdString(err));
        }
        if(pix != AV_PIX_FMT_NV12){
            return;
        }

        qint64 end_pt = QDateTime::currentMSecsSinceEpoch();
        if((end_pt - start_pt_) >= 1000)
        {
            if(decode_frames_ != curFps_)
            {
                curFps_ = decode_frames_;
                thread()->sigCurFpsChanged(curFps_);
            }
            start_pt_ = end_pt;
            decode_frames_ = 0;
        }
        decode_frames_++;

        thread()->Render([&](){
            if(!render_)
            {
                thread()->setExtraData(decoder_->context());
                render_ = thread()->getRender(AV_PIX_FMT_CUDA);
                render_->initialize(width, height);
                if(decoder_->fps())
                {
                    thread()->sigFps(decoder_->fps());
                }
                thread()->sigVideoStarted(width, height);
            }
            render_->upLoad(reinterpret_cast<unsigned char*>(ptr), width, height);
        });

        if(thread()->photoShot().load())
        {
            int nv12_size = av_image_get_buffer_size(AV_PIX_FMT_NV12, width,
                                                  height, 1);
            uint8_t *nv12_buffer = new uint8_t[nv12_size];
            int code = decoder_->memCpy(nv12_buffer, ptr, nv12_size, 2);
            if(code)
            {
                thread()->sigPhotoShotError(QString("cuda mem cpy dev to host failed, code: %1").arg(code));
                thread()->photoShot().store(false);
                return;
            }
            uint8_t* srcSlice[2]{nv12_buffer, nv12_buffer + width * height};
            int srcStride[2]{width, width};

            SwsContext *sws_ctx = sws_getContext(width, height, AV_PIX_FMT_NV12, width, height, AV_PIX_FMT_RGB24, SWS_BICUBIC, nullptr,nullptr,nullptr);
            if(!sws_ctx)
            {
                thread()->sigPhotoShotError("sws_getContext return nullptr");
                thread()->photoShot().store(false);
                return;
            }
            
            int rgb24_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width,
                                                  height, 1);
            uint8_t* rgb24_buffer = new uint8_t[rgb24_size];
            uint8_t* dstSlice[1]{rgb24_buffer};
            int dstStride[1]{width * 3};
            sws_scale(sws_ctx, srcSlice, srcStride, 0, height, dstSlice, dstStride);
            QImage rgb_img((uchar*)rgb24_buffer, width, height, QImage::Format_RGB888, [](void *info){delete info;}, rgb24_buffer);

            thread()->sigPhotoShot(rgb_img);
            sws_freeContext(sws_ctx);
            delete nv12_buffer;

            thread()->photoShot().store(false);
        }
    });

    if(!thread()->isInterruptionRequested()){
        if(url.left(4) == "rtsp" && error.empty()){
            thread()->sigError("AVERROR_EOF");
        }
    }
}
