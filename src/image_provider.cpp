#include "image_provider.h"
#include <QDir>
#include "image_resource.h"

namespace MediaProvider {

struct ImageProvider::Implementation {};

ImageProvider::ImageProvider(QObject *parent)
    : FileBasedProvider({"*.jpg", "*.jpeg", "*.png", "*.bmp"}, parent) {
  impl_.reset(new Implementation);
}

ImageProvider::~ImageProvider() = default;

Resource *ImageProvider::createResource(const QString &resource) const {
  if (availableResources().contains(resource)) {
    return new ImageResource(getPath(resource));
  }
  setErrorString(
      "Unable to create unexisting resource, check available resources");
  return nullptr;
}

}  // namespace MediaProvider
