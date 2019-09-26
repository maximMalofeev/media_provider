#ifndef SAPERAPROVIDER_H
#define SAPERAPROVIDER_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class SaperaProvider : public Provider {
 public:
  SaperaProvider(QObject *parent = nullptr);
  ~SaperaProvider();

  bool setOrigin(const QString &orig);
  Resource *createResource(const QString &resource);

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}  // namespace MediaProvider

#endif  // SAPERAPROVIDER_H
