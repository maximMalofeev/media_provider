#ifndef RTSPPROVIDER_H
#define RTSPPROVIDER_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class RtspProvider : public Provider {
 public:
  RtspProvider(QObject* parent = nullptr);
  ~RtspProvider();

  State state() const override;
  QString origin() const override;
  bool setOrigin(const QString &orig) override;
  QStringList availableResources() const override;
  Resource *createResource(const QString &resource) const override;
  QString errorString() const override;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}  // namespace MediaProvider

#endif  // RTSPPROVIDER_H
