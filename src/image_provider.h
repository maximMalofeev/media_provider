#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include "file_based_provider.h"

namespace MediaProvider {

class ImageProvider : public FileBasedProvider {
 public:
  ImageProvider(QObject* parent = nullptr);
  ~ImageProvider();

  Resource *createResource(const QString &resource) const override;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

}  // namespace MediaProvider

#endif  // IMAGEPROVIDER_H
