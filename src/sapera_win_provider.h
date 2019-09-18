#ifndef SAPERAPROVIDER_H
#define SAPERAPROVIDER_H

#include <media_provider/media_provider.h>

class SapManCallbackInfo;

namespace MediaProvider {

class SaperaProvider : public Provider {
 public:
  SaperaProvider(QObject* parent = nullptr);
  ~SaperaProvider();

  State state() const override;
  QString origin() const override;
  bool setOrigin(const QString &orig) override;
  QStringList availableResources() const override;
  Resource *createResource(const QString &resource) const override;

  static void newServerEvent(SapManCallbackInfo *callbackInfo);

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}  // namespace MediaProvider

#endif  // SAPERAPROVIDER_H
