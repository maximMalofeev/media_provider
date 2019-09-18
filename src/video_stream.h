#ifndef FILESTREAM_H
#define FILESTREAM_H

#include <media_provider/media_provider.h>
#include <QMediaPlayer>

namespace MediaProvider {

class VideoStream : public Stream {
 public:
  VideoStream(QMediaPlayer* player, QObject* parent = nullptr);
  ~VideoStream();

  State state() const override;

 public slots:
  void start() override;
  void stop() override;
  void onNewFrame(QImage frame, qlonglong timestamp);

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}  // namespace MediaProvider

#endif  // FILESTREAM_H
