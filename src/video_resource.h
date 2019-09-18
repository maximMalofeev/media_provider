#ifndef VIDEORESOURCE_H
#define VIDEORESOURCE_H

#include "qmediaplayer_based_resource.h"

namespace MediaProvider {

class VideoResource : public QMediaPlayerBasedResource {
 public:
  VideoResource(const QString& resource, QObject* parent = nullptr);
  ~VideoResource();

  Stream* stream() override;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

}  // namespace MediaProvider

#endif  // VIDEORESOURCE_H
