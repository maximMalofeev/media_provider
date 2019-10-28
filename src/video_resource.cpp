#include "video_resource.h"
#include <QFileInfo>
#include <QMediaPlayer>
#include <QUrl>
#include "video_stream.h"
#include "video_surface.h"

namespace MediaProvider {

struct VideoResource::Implementation {
  VideoStream* stream = {};
  QFileInfo fi;
};

VideoResource::VideoResource(const QString& res, QObject* parent)
    : QMediaPlayerBasedResource(res, parent) {
  impl_.reset(new Implementation);
  impl_->stream = new VideoStream{qMediaPlayer(), this};
  impl_->fi = QFileInfo(res);
  setResource(impl_->fi.fileName());
  connect(videoSurface(), &VideoSurface::newFrame, impl_->stream,
          &VideoStream::onNewFrame);
}

void VideoResource::initialise() {
  setState(Initialising);
  qMediaPlayer()->setMedia(QUrl::fromLocalFile(impl_->fi.absoluteFilePath()));
}

VideoResource::~VideoResource() = default;

Stream* VideoResource::stream() { return impl_->stream; }

}  // namespace MediaProvider
