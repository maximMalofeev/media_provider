#ifndef QMEDIAPLAYERBASEDRESOURCE_H
#define QMEDIAPLAYERBASEDRESOURCE_H

#include <media_provider/media_provider.h>

class QMediaPlayer;

namespace MediaProvider{

class VideoSurface;

class QMediaPlayerBasedResource : public Resource
{
 public:
  QMediaPlayerBasedResource(const QString& res, QObject* parent = nullptr);
  ~QMediaPlayerBasedResource();

  QSize size() const override;
  bool setSize(const QSize &size) override;
  QList<QVariant> availableSizes() const override;
  QList<QVariant> availableColorFormats() const override;
  QImage::Format colorFormat() const override;
  bool setColorFormat(const QImage::Format format) override;

 protected:
  // If stream required to use media player after
  // QMediaPlayerBasedResource::~QMediaPlayerBasedResource() it has to take
  // parentness of it
  QMediaPlayer* qMediaPlayer() const;
  VideoSurface* videoSurface() const;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}

#endif // QMEDIAPLAYERBASEDRESOURCE_H
