#include "bardecoder.h"
#include <QUrl>
#include <QtConcurrent/QtConcurrent>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


using namespace cv;
using namespace std;

Bardecoder::Bardecoder(QObject *parent)
    : QObject{parent}
{
    m_bardet = makePtr<barcode::BarcodeDetector>("", "");
}

void Bardecoder::setVideoSink(QObject *videoSink)
{
    m_videoSink = qobject_cast<QVideoSink*>(videoSink);

    if (m_videoSink) {
        QObject::connect(m_videoSink, &QVideoSink::videoFrameChanged,
                         this, &Bardecoder::setFrame);
    }
}

void Bardecoder::setFrame(const QVideoFrame &frame)
{
    if(!isDecoding() && m_processThread.isFinished()) {
        m_decoding = true;
        QImage image = frame.toImage().convertToFormat(QImage::Format_RGB32).rgbSwapped();

        m_processThread = QtConcurrent::run([=]() {

            if(image.isNull()) {
                m_decoding = false;
                return;
            }
            const int count_experiments = 10;
            cv::Mat img(image.height(), image.width(), CV_8UC4, (void *) image.bits(), image.bytesPerLine());
            vector<Mat> points;
            /*auto res = m_detector->detectAndDecode(img, points);
            if (res.size() > 0) {
                for (const auto& value : res) {
                    qDebug() << " opencv " << QString(value.c_str());
                    emit decoded(QString(value.c_str()));
                }
            }*/
            vector<Point> corners;
            vector<cv::String> decode_info;
            vector<string> decode_type;
            m_bardet->detectMulti(img, corners);
            m_bardet->detectAndDecodeWithType(img, decode_info, decode_type, corners);
            if (decode_info.size() > 0) {
                qDebug() << " opencv " << QString(decode_info.at(0).c_str());
                emit decoded(QString(decode_info.at(0).c_str()));
            }
            m_decoding = false;
        });
    }
}

