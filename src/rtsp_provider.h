#ifndef RTSPPROVIDER_H
#define RTSPPROVIDER_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class RtspProvider : public Provider {
 public:
  RtspProvider(QObject* parent = nullptr);
  ~RtspProvider();

  bool setOrigin(const QString &orig) override;
  Resource *createResource(const QString &resource) override;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}  // namespace MediaProvider

#endif  // RTSPPROVIDER_H
