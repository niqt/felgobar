#ifndef BARDECODER_H
#define BARDECODER_H

#include <QObject>
#include <QtMultimedia/QVideoSink>
#include <QtMultimedia/QVideoFrame>
//#include "opencv2/wechat_qrcode.hpp"
#include <qqml.h>
#include <QFuture>
#include "opencv2/opencv.hpp"

class Bardecoder : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QObject* videoSink WRITE setVideoSink)

public:
    Bardecoder(QObject *parent = nullptr);
    void setVideoSink(QObject *videoSink);
    bool isDecoding() {return m_decoding; }

public slots:
    void setFrame(const QVideoFrame &frame);


signals:
    void videoSyncChnaged();
    void decoded(const QString &qr);

private:
    QVideoSink *m_videoSink;
    //Ptr<wechat_qrcode::WeChatQRCode> m_detector;
    cv::Ptr<cv::barcode::BarcodeDetector> m_bardet;
    QFuture<void> m_processThread;
    bool m_decoding;

};

#endif // BARDECODER_H
