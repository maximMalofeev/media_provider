#ifndef SAPERAPROVIDER_H
#define SAPERAPROVIDER_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class DalsaProvider : public Provider {
 public:
  DalsaProvider(QObject *parent = nullptr);
  ~DalsaProvider() override;

  QString provider() const override;
  bool setOrigin(const QString &orig) override;
  Resource *createResource(const QString &resource) override;

  static const QString PROVIDER_NAME;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}  // namespace MediaProvider

#endif  // SAPERAPROVIDER_H
