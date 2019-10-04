#include "rtsp_resource.h"
#include <QMediaPlayer>
#include <QUrl>
#include "rtsp_stream.h"
#include "video_stream.h"
#include "video_surface.h"

namespace MediaProvider {

struct RtspResource::Implementation {
  RtspStream *stream = {};
};

RtspResource::RtspResource(const QString &resource, QObject *parent)
    : QMediaPlayerBasedResource(resource, parent) {
  impl_.reset(new Implementation);
  impl_->stream = new RtspStream{qMediaPlayer(), this};
  qMediaPlayer()->setMedia(QUrl(resource));
  connect(videoSurface(), &VideoSurface::newFrame, impl_->stream,
          &RtspStream::onNewFrame);
}

RtspResource::~RtspResource() {};

Stream *RtspResource::stream() { return impl_->stream; }

}  // namespace MediaProvider
