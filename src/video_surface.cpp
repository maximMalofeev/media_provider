#include "video_surface.h"
#include <QDebug>
#include <QThread>
#include <QVideoSurfaceFormat>
#include <QtConcurrent/QtConcurrent>

namespace MediaProvider {

struct VideoSurface::Implementation {
  QFutureWatcher<QImage> imageWatcher;
  qlonglong timestamp = -1;
  QSize size;
  QVideoFrame::PixelFormat format;
};

VideoSurface::VideoSurface(QObject* parent) : QAbstractVideoSurface(parent) {
  impl_.reset(new Implementation);
  connect(&impl_->imageWatcher, &QFutureWatcher<QImage>::finished, [this]() {
    emit newFrame(impl_->imageWatcher.result(), impl_->timestamp);
  });
}

QList<QVideoFrame::PixelFormat> VideoSurface::supportedPixelFormats(
    QAbstractVideoBuffer::HandleType handleType) const {
  if (handleType == QAbstractVideoBuffer::NoHandle) {
    return {QVideoFrame::Format_RGB32};
  } else {
    return {};
  }
}

bool VideoSurface::present(const QVideoFrame& frame) {
  if (!isActive()) {
    qWarning() << "VideoSurface not started";
    setError(StoppedError);
    return false;
  }

  if (!impl_->imageWatcher.isFinished()) {
    return false;
  }

  if (frame.pixelFormat() == QVideoFrame::Format_Invalid ||
      frame.pixelFormat() != QVideoFrame::Format_RGB32) {
    setError(IncorrectFormatError);
    return false;
  }

  if (frame.isValid()) {
    QVideoFrame cloneFrame(frame);

    auto future = QtConcurrent::run(
        [this](QVideoFrame cloneFrame) {
          if (cloneFrame.map(QAbstractVideoBuffer::ReadOnly)) {
            auto timestamp = cloneFrame.startTime();
            auto mappedBytes = cloneFrame.mappedBytes();
            uchar* frameBuf = new uchar[mappedBytes];
            std::memcpy(frameBuf, cloneFrame.bits(), mappedBytes);
            QImage image(
                frameBuf, cloneFrame.width(), cloneFrame.height(),
                cloneFrame.bytesPerLine(),
                QVideoFrame::imageFormatFromPixelFormat(
                    cloneFrame.pixelFormat()),
                [](void* info) {
                  auto frameBuf = static_cast<uchar*>(info);
                  if (frameBuf) {
                    delete[] frameBuf;
                  }
                },
                frameBuf);
            cloneFrame.unmap();

            return image;
          }

          return QImage{};
        },
        cloneFrame);

    impl_->imageWatcher.setFuture(future);
  } else {
    setError(ResourceError);
    qWarning() << "Invalid frame";
    return false;
  }
  return true;
}

bool VideoSurface::start(const QVideoSurfaceFormat& format) {
  impl_->size = format.frameSize();
  impl_->format = format.pixelFormat();

  return QAbstractVideoSurface::start(format);
}

QSize VideoSurface::size() const { return impl_->size; }

QImage::Format VideoSurface::format() const {
  return QVideoFrame::imageFormatFromPixelFormat(impl_->format);
}

}  // namespace MediaProvider
