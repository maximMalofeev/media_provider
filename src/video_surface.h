#ifndef VIDEOSURFACE_H
#define VIDEOSURFACE_H

#include <QAbstractVideoSurface>
#include <QMutex>
#include <QObject>
#include <QTime>

namespace MediaProvider {

class VideoSurface : public QAbstractVideoSurface {
  Q_OBJECT

 public:
  VideoSurface(QObject* parent = nullptr);
  ~VideoSurface();

  QList<QVideoFrame::PixelFormat> supportedPixelFormats(
      QAbstractVideoBuffer::HandleType handleType =
          QAbstractVideoBuffer::NoHandle) const override;
  bool present(const QVideoFrame& frame) override;
  bool start(const QVideoSurfaceFormat &format) override;

  QSize size() const;
  QImage::Format format() const;

 signals:
  void newFrame(QImage frame, qlonglong timestamp);

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

}  // namespace MediaProvider

#endif  // VIDEOSURFACE_H
