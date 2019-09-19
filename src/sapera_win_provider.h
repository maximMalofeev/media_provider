#ifndef SAPERAPROVIDER_H
#define SAPERAPROVIDER_H

#include <media_provider/media_provider.h>

class SapManCallbackInfo;

namespace MediaProvider {

class SaperaProvider : public Provider {
 public:
  SaperaProvider(QObject* parent = nullptr);
  ~SaperaProvider();

  bool setOrigin(const QString &orig) override;
  Resource *createResource(const QString &resource) override;

  static void onServerEvent(SapManCallbackInfo *callbackInfo);

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}  // namespace MediaProvider

#endif  // SAPERAPROVIDER_H
