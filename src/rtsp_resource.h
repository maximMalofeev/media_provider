#ifndef RTSPRESOURCE_H
#define RTSPRESOURCE_H

#include "qmediaplayer_based_resource.h"

namespace MediaProvider {

class RtspResource : public QMediaPlayerBasedResource {
 public:
  RtspResource(const QString& resource, QObject* parent = nullptr);
  ~RtspResource();

  void initialise() override;

  Stream* stream() override;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}  // namespace MediaProvider

#endif  // RTSPRESOURCE_H
