#ifndef RTSPPROVIDER_H
#define RTSPPROVIDER_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class RtspProvider : public Provider {
 public:
  RtspProvider(QObject* parent = nullptr);
  ~RtspProvider();

  QString provider() const override;
  bool setOrigin(const QString &orig) override;
  void initialise() override;
  Resource *createResource(const QString &resource) override;

  static const QString PROVIDER_NAME;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}  // namespace MediaProvider

#endif  // RTSPPROVIDER_H
