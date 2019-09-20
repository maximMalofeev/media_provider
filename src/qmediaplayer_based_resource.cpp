#include "qmediaplayer_based_resource.h"
#include <QMediaMetaData>
#include <QMediaPlayer>
#include "video_surface.h"

namespace MediaProvider {

struct QMediaPlayerBasedResource::Implementation {
  QMediaPlayer* player = {};
  VideoSurface* videoSurface = {};
};

QMediaPlayerBasedResource::QMediaPlayerBasedResource(const QString& res,
                                                     QObject* parent)
    : Resource(parent) {
  impl_.reset(new Implementation);
  setResource(res);
  impl_->player = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
  connect(
      impl_->player, &QMediaPlayer::mediaStatusChanged,
      [this](QMediaPlayer::MediaStatus status) {
        qDebug() << "Media status changed to" << status;
        if (status == QMediaPlayer::LoadedMedia && state() == Initialising) {
          setState(Initialised);
          emit sizeChanged();
          emit availableSizesChanged();
          emit colorFormatChanged();
          emit availableColorFormatsChanged();
        }else if(status == QMediaPlayer::InvalidMedia && state() == Initialising){
          setState(Invalid);
          setErrorString(impl_->player->errorString());
        }
      });
  connect(impl_->player,
          QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
          [this](QMediaPlayer::Error error) {
            qDebug() << "Media player error:" << error << "-"
                     << impl_->player->errorString();
            setState(Invalid);
            QString errStr = "Media player error: " + QString::number(error);
            if(!impl_->player->errorString().isEmpty()){
              errStr += " - " + impl_->player->errorString();
            }
            setErrorString(errStr);
          });

  impl_->videoSurface = new VideoSurface(impl_->player);
  impl_->player->setVideoOutput(impl_->videoSurface);
}

QMediaPlayerBasedResource::~QMediaPlayerBasedResource() = default;

QSize QMediaPlayerBasedResource::size() const {
  if (state() != Initialised) {
    return {};
  }
  return impl_->videoSurface->size();
}

bool QMediaPlayerBasedResource::setSize(const QSize& size) {
  if (state() != Initialised) {
    return false;
  }
  if (impl_->videoSurface->size() == size) {
    return true;
  } else {
    setErrorString("The provider not supports setSize feature");
    return false;
  }
}

QList<QVariant> QMediaPlayerBasedResource::availableSizes() const {
  if (state() != Initialised) {
    return {};
  }
  return {impl_->videoSurface->size()};
}

QList<QVariant> QMediaPlayerBasedResource::availableColorFormats() const {
  if (state() != Initialised) {
    return {};
  }
  return {QVariant::fromValue(impl_->videoSurface->format())};
}

QImage::Format QMediaPlayerBasedResource::colorFormat() const {
  if (state() != Initialised) {
    return QImage::Format_Invalid;
  }
  return impl_->videoSurface->format();
}

bool QMediaPlayerBasedResource::setColorFormat(const QImage::Format format) {
  if (format == impl_->videoSurface->format()) {
    return true;
  } else {
    setErrorString("The provider not supports setColorFormat feature");
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
