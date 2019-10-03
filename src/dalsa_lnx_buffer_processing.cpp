#include "dalsa_lnx_buffer_processing.h"
#include <GenApi/GenApi.h>
#include <gevapi.h>
#include <QDebug>
#include <QTime>

namespace MediaProvider {

QDebug operator<<(QDebug d, const GEV_BUFFER_OBJECT* img) {
  d << "Address:" << img->address << endl;
  d << "State:" << img->state << endl;
  d << "Status:" << img->status << endl;
  d << "Received size:" << img->recv_size << endl;
  d << "TimestampHi:" << img->timestamp_hi << endl;
  d << "TimestampLo:" << img->timestamp_lo << endl;
  return d;
}

static unsigned long us_timer_init(void) {
  struct timeval tm;
  unsigned long msec;

  // Get the time and turn it into a millisecond counter.
  gettimeofday(&tm, nullptr);

  msec = (tm.tv_sec * 1000000) + (tm.tv_usec);
  return msec;
}

constexpr int NUM_BUFFERS = 8;

struct DalsaBufferProcessing::Implementation {
  GEV_CAMERA_HANDLE handle{};
  PUINT8 buffers[NUM_BUFFERS]{};
  bool initialised{false};
  bool stopped{false};
};

DalsaBufferProcessing::DalsaBufferProcessing(void* gevCameraHandle,
                                             QObject* parent)
    : QThread(parent) {
  impl_.reset(new Implementation{gevCameraHandle});

  UINT32 width{}, height{}, x_offset{}, y_offset{}, format{};
  int depth{};
  if (GevGetImageParameters(impl_->handle, &width, &height, &x_offset,
                            &y_offset, &format) != GEVLIB_OK) {
    qWarning() << "Unable to get image parameters";
    return;
  }
  depth = GetPixelSizeInBytes(format);
  auto bufSize = width * height * depth;

  for (int i = 0; i < NUM_BUFFERS; i++) {
    impl_->buffers[i] = new UINT8[bufSize];
    std::fill(impl_->buffers[i], impl_->buffers[i] + bufSize, 0);
  }

  if (GevInitializeTransfer(impl_->handle, Asynchronous, bufSize, NUM_BUFFERS,
                           impl_->buffers) != GEVLIB_OK) {
    qWarning() << "Unable to init image transfer";
    return;
  }

  impl_->initialised = true;
}

DalsaBufferProcessing::~DalsaBufferProcessing() {
  qDebug() << "DalsaBufferProcessing::~DalsaBufferProcessing()";
  stop();
  GevFreeTransfer(impl_->handle);
  for (int i = 0; i < NUM_BUFFERS; i++) {
    delete[] impl_->buffers[i];
  }
}

bool DalsaBufferProcessing::isInitialised() const { return impl_->initialised; }

void DalsaBufferProcessing::stop() {
  if (isRunning()) {
    impl_->stopped = true;
  }
}

void DalsaBufferProcessing::run() {
  qDebug() << "DalsaBufferProcessing started";

  GEV_BUFFER_OBJECT* img = nullptr;
  GEV_STATUS status = 0;
  impl_->stopped = false;
  while (!impl_->stopped) {
    status = GevWaitForNextFrame(impl_->handle, &img, 2000);
    if (status == GEVLIB_OK) {
      if (img != nullptr) {
        if (img->status == GEV_FRAME_STATUS_RECVD) {
          QImage::Format format{};
          if (img->format == fmtMono8) {
            format = QImage::Format_Grayscale8;
          } else if (img->format == fmtRGB8Packed) {
            format = QImage::Format_RGB888;
          } else {
            qWarning() << "Unsupported image format received";
            continue;
          }

          qDebug() << QTime::currentTime();
          auto frameSize = img->w * img->h * img->d;
          uchar* frame = new uchar[frameSize]{0};
          memcpy(frame, img->address, frameSize);

          QImage newImg{frame,
                        static_cast<int>(img->w),
                        static_cast<int>(img->h),
                        format,
                        [](void* p) { delete static_cast<uchar*>(p); },
                        frame};
          emit newFrame(newImg, -1);
        }else{
          switch (img->status) {
            case GEV_FRAME_STATUS_PENDING:
              qDebug() << "GEV_FRAME_STATUS_PENDING";
              break;
            case GEV_FRAME_STATUS_TIMEOUT:
              qDebug() << "GEV_FRAME_STATUS_TIMEOUT";
              break;
            case GEV_FRAME_STATUS_OVERFLOW:
              qDebug() << "GEV_FRAME_STATUS_OVERFLOW";
              break;
            case GEV_FRAME_STATUS_BANDWIDTH:
              qDebug() << "GEV_FRAME_STATUS_BANDWIDTH";
              break;
            case GEV_FRAME_STATUS_LOST:
              qDebug() << "GEV_FRAME_STATUS_LOST";
              break;
            case GEV_FRAME_STATUS_RELEASED:
              qDebug() << "GEV_FRAME_STATUS_RELEASED";
              break;
            default:
              qDebug() << "Unexpected status";
          }
        }
      } else {
        qDebug() << "GEV_BUFFER_OBJECT is nullptr";
      }
    } else {
      qDebug() << "GevWaitForNextImage failed with status:" << status;
      emit error("Wait for next image failed with status: " +
                 QString::number(status));
      break;
    }
  }

  qDebug() << "DalsaBufferProcessing finished";
}

}  // namespace MediaProvider
