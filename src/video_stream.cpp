#include "video_stream.h"
#include <QMediaPlayer>

namespace MediaProvider {

struct VideoStream::Implementation {
  QMediaPlayer *player{};
};

VideoStream::VideoStream(QMediaPlayer *player, Resource *parent)
    : Stream(parent) {
  impl_.reset(new Implementation{player});
  connect(impl_->player, &QMediaPlayer::stateChanged,
          [this](QMediaPlayer::State state) {
            if (state == QMediaPlayer::PlayingState) {
              setState(Playing);
            } else {
              setState(Stopped);
            }
          });
}

VideoStream::~VideoStream() = default;

void VideoStream::start() { impl_->player->play(); }

void VideoStream::stop() { impl_->player->stop(); }

void VideoStream::onNewFrame(QImage frame, qlonglong timestamp) {
  if (impl_->player->state() == QMediaPlayer::StoppedState) {
    return;
  }
  emit newFrame(frame, timestamp == -1 ? impl_->player->position() : timestamp);
}

}  // namespace MediaProvider
