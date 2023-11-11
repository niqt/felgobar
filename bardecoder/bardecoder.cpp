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
    m_bardet = makePtr<barcode::BarcodeDetector>("", ""); // create the bardetector
}

void Bardecoder::setVideoSink(QObject *videoSink)
{
    m_videoSink = qobject_cast<QVideoSink*>(videoSink);

    if (m_videoSink) {
        QObject::connect(m_videoSink, &QVideoSink::videoFrameChanged, // when a frame is received call setFrame
                         this, &Bardecoder::setFrame);
    }
}

void Bardecoder::setFrame(const QVideoFrame &frame)
{
    if(m_run && !isDecoding() && m_processThread.isFinished()) { // if start decode and is not decoding and the previous is finished => decode the frame
        m_decoding = true;
        QImage image = frame.toImage().convertToFormat(QImage::Format_RGB32).rgbSwapped(); // convert the frame to an imageso can be used from opecv

        m_processThread = QtConcurrent::run([=]() {
            if(image.isNull()) {
                m_decoding = false;
                return;
            }
            cv::Mat img(image.height(), image.width(), CV_8UC4, (void *) image.bits(), image.bytesPerLine());
            vector<Mat> points;

            vector<Point> corners;
            vector<cv::String> decode_info;
            vector<string> decode_type;
            m_bardet->detectMulti(img, corners);
            m_bardet->detectAndDecodeWithType(img, decode_info, decode_type, corners);
            if (decode_info.size() > 0) {
                auto code = QString(decode_info.at(0).c_str());
                if (code.length() > 0) // if the barcode is decoded emit the signal
                    emit decoded(QString(decode_info.at(0).c_str()));
            }
            m_decoding = false;
        });
    }
}

void Bardecoder::setRun(bool run)
{
    m_run = run;
}
