#ifndef SAPERAPROVIDER_H
#define SAPERAPROVIDER_H

#include <media_provider/media_provider.h>

class SapManCallbackInfo;

namespace MediaProvider {

class DalsaProvider : public Provider {
  Q_OBJECT

 public:
  DalsaProvider(QObject* parent = nullptr);
  ~DalsaProvider();

  bool setOrigin(const QString &orig) override;
  Resource *createResource(const QString &resource) override;

  static void onServerEvent(SapManCallbackInfo *callbackInfo);

 signals:
  void serverDisconnected(QString serverName);

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}  // namespace MediaProvider

#endif  // SAPERAPROVIDER_H
