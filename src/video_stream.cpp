#include "video_stream.h"

namespace MediaProvider {

struct VideoStream::Implementation {
  QMediaPlayer *player = {};
  State state = State::Stopped;
};

VideoStream::VideoStream(QMediaPlayer *player, QObject *parent)
    : Stream(parent) {
  impl_.reset(new Implementation{player});
  connect(impl_->player, &QMediaPlayer::stateChanged,
          [this](QMediaPlayer::State state) {
            if (state == QMediaPlayer::PlayingState) {
              impl_->state = Playing;
            } else {
              impl_->state = Stopped;
            }
            emit stateChanged();
          });
}

VideoStream::~VideoStream() = default;

Stream::State VideoStream::state() const { return impl_->state; }

void VideoStream::start() { impl_->player->play(); }

void VideoStream::stop() { impl_->player->stop(); }

void VideoStream::onNewFrame(QImage frame, qlonglong timestamp) {
  if (impl_->player->state() == QMediaPlayer::StoppedState) {
    return;
  }
  emit newFrame(frame, timestamp == -1 ? impl_->player->position() : timestamp);
}

}  // namespace MediaProvider
