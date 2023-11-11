#ifndef BARDECODER_H
#define BARDECODER_H

#include <QObject>
#include <QtMultimedia/QVideoSink>
#include <QtMultimedia/QVideoFrame>
#include <qqml.h>
#include <QFuture>
#include "opencv2/opencv.hpp"

/*
 * This class implement a qml wrapper for the OpenCV bar code feature
 * */

class Bardecoder : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QObject* videoSink WRITE setVideoSink) // the video sync used to get the frames
    Q_PROPERTY(bool run READ run WRITE setRun) // start/top the decode
public:
    Bardecoder(QObject *parent = nullptr);
    void setVideoSink(QObject *videoSink);
    bool isDecoding() {return m_decoding; } // the status of the decoding - decondig in progress
    void setRun(bool run);
    bool run() {return m_run;}
public slots:
    void setFrame(const QVideoFrame &frame);


signals:
    void videoSyncChnaged();
    void decoded(const QString &code);

private:
    QVideoSink *m_videoSink;
    cv::Ptr<cv::barcode::BarcodeDetector> m_bardet;
    QFuture<void> m_processThread;
    bool m_decoding;
    bool m_run;
};

#endif // BARDECODER_H
