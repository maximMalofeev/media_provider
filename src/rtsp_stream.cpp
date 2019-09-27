#include "rtsp_stream.h"
#include <QMediaPlayer>

namespace MediaProvider {

struct RtspStream::Implementation {
  QMediaPlayer *player{};
};

RtspStream::RtspStream(QMediaPlayer *player, Resource *parent)
    : Stream(parent) {
  impl_.reset(new Implementation{player});
  player->setParent(this);
  connect(impl_->player, &QMediaPlayer::stateChanged,
          [this](QMediaPlayer::State s) {
            if (state() == Invalid) {
              return;
            }
            if (s == QMediaPlayer::PlayingState) {
              setState(Playing);
            } else {
              setState(Stopped);
            }
          });
  connect(impl_->player, &QMediaPlayer::mediaStatusChanged,
          [this](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia) {
              setState(Invalid);
              setErrorString("Unexpected end of rtsp stream");
            }
          });
}

RtspStream::~RtspStream() {
  stop();
}

void RtspStream::start() {
  if(resource()->state() != Resource::Initialised){
    return;
  }
  if (auto s = state(); s == Invalid || s == Playing) {
    return;
  }
  impl_->player->play();
}

void RtspStream::stop() {
  if (auto s = state(); s == Invalid || s == Stopped) {
    return;
  }
  impl_->player->stop();
}

void RtspStream::onNewFrame(QImage frame, qlonglong timestamp) {
  if (impl_->player->state() == QMediaPlayer::StoppedState ||
      state() == Invalid) {
    return;
  }
  emit newFrame(frame, timestamp == -1 ? impl_->player->position() : timestamp);
}

}  // namespace MediaProvider
