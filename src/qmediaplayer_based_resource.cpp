#include "qmediaplayer_based_resource.h"
#include <QMediaMetaData>
#include <QMediaPlayer>
#include "video_surface.h"

namespace MediaProvider {

struct QMediaPlayerBasedResource::Implementation {
  QString resource;
  Resource::State state = Resource::Initialising;
  QMediaPlayer* player = {};
  VideoSurface* videoSurface = {};
};

QMediaPlayerBasedResource::QMediaPlayerBasedResource(const QString& resource,
                                                     QObject* parent)
    : Resource(parent) {
  impl_.reset(new Implementation);
  impl_->resource = resource;
  impl_->player = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
  connect(impl_->player, &QMediaPlayer::mediaStatusChanged,
          [this](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::LoadedMedia &&
                impl_->state == Initialising) {
              impl_->state = Initialised;
              emit stateChanged();
              emit sizeChanged();
              emit availableSizesChanged();
              emit colorFormatChanged();
              emit availableColorFormatsChanged();
              qDebug() << "Media loaded";
            }
          });
  connect(impl_->player,
          QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
          [this](QMediaPlayer::Error error) {
            qDebug() << "Media player error:" << error << "-"
                     << impl_->player->errorString();
            impl_->state = Invalid;
            emit stateChanged();
          });

  impl_->videoSurface = new VideoSurface(impl_->player);
  impl_->player->setVideoOutput(impl_->videoSurface);
}

QMediaPlayerBasedResource::~QMediaPlayerBasedResource() = default;

Resource::State QMediaPlayerBasedResource::state() const {
  return impl_->state;
}

QString QMediaPlayerBasedResource::resource() const { return impl_->resource; }

QSize QMediaPlayerBasedResource::size() const {
  if (impl_->state != Initialised) {
    return {};
  }
  return impl_->videoSurface->size();
}

bool QMediaPlayerBasedResource::setSize(const QSize& size) {
  if (impl_->state != Initialised) {
    return false;
  }
  if (impl_->player->metaData(QMediaMetaData::Resolution).toSize() == size) {
    return true;
  } else {
    return false;
  }
}

QList<QVariant> QMediaPlayerBasedResource::availableSizes() const {
  if (impl_->state != Initialised) {
    return {};
  }
  return {impl_->player->metaData(QMediaMetaData::Resolution).toSize()};
}

QList<QVariant> QMediaPlayerBasedResource::availableColorFormats() const {
  return {QVariant::fromValue(impl_->videoSurface->format())};
}

QImage::Format QMediaPlayerBasedResource::colorFormat() const {
  return impl_->videoSurface->format();
}

bool QMediaPlayerBasedResource::setColorFormat(const QImage::Format format) {
  if (format == impl_->videoSurface->format()) {
    return true;
  } else {
    return false;
  }
}

QMediaPlayer* QMediaPlayerBasedResource::qMediaPlayer() const {
  return impl_->player;
}

VideoSurface* QMediaPlayerBasedResource::videoSurface() const {
  return impl_->videoSurface;
}

}  // namespace MediaProvider
