#include "video_resource.h"
#include <QMediaPlayer>
#include <QUrl>
#include <QFileInfo>
#include "video_stream.h"
#include "video_surface.h"

namespace MediaProvider {

struct VideoResource::Implementation {
  VideoStream* stream = {};
};

VideoResource::VideoResource(const QString& res, QObject* parent)
    : QMediaPlayerBasedResource(res, parent) {
  impl_.reset(new Implementation);
  impl_->stream = new VideoStream{qMediaPlayer(), this};
  qMediaPlayer()->setMedia(QUrl::fromLocalFile(res));
  QFileInfo fi(res);
  setResource(fi.fileName());
  connect(videoSurface(), &VideoSurface::newFrame, impl_->stream, &VideoStream::onNewFrame);
}

VideoResource::~VideoResource() = default;

Stream* VideoResource::stream() { return impl_->stream; }

}  // namespace MediaProvider
