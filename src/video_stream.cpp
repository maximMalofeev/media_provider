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
  connect(impl_->player, &QMediaPlayer::mediaStatusChanged,
          [this](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia) {
              setState(Stopped);
            }
          });
}

VideoStream::~VideoStream() {}

void VideoStream::start() {
  if (resource()->state() != Resource::Initialised) {
    return;
  }
  if (auto s = state(); s == Invalid || s == Playing) {
    return;
  }
  impl_->player->play();
}

void VideoStream::stop() {
  if (auto s = state(); s == Invalid || s == Stopped) {
    return;
  }
  impl_->player->stop();
}

void VideoStream::onNewFrame(QImage frame, qlonglong timestamp) {
  if (impl_->player->state() == QMediaPlayer::StoppedState ||
      state() == Invalid) {
    return;
  }
  emit newFrame(frame, timestamp == -1 ? impl_->player->position() : timestamp);
}

}  // namespace MediaProvider
