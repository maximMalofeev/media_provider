#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include "file_based_provider.h"

namespace MediaProvider {

class ImageProvider : public FileBasedProvider {
 public:
  ImageProvider(QObject* parent = nullptr);
  ~ImageProvider();

  QString provider() const override;
  Resource *createResource(const QString &resource) override;

  static const QString PROVIDER_NAME;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

}  // namespace MediaProvider

#endif  // IMAGEPROVIDER_H
