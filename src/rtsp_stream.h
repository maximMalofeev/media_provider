#ifndef RTSPSTREAM_H
#define RTSPSTREAM_H

#include <media_provider/media_provider.h>

class QMediaPlayer;

namespace MediaProvider {

class RtspStream : public Stream
{
 public:
  RtspStream(QMediaPlayer* player, Resource* parent);
  ~RtspStream();

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;

public slots:
  void start() override;
  void stop() override;
  void onNewFrame(QImage frame, qlonglong timestamp);
};

}

#endif // RTSPSTREAM_H
