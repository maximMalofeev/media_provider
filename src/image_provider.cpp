#include "image_provider.h"
#include <QDir>
#include "image_resource.h"

namespace MediaProvider {

const QString ImageProvider::PROVIDER_NAME = "IMAGE_PROVIDER";

struct ImageProvider::Implementation {};

ImageProvider::ImageProvider(QObject *parent)
    : FileBasedProvider({"*.jpg", "*.jpeg", "*.png", "*.bmp"}, parent) {
  impl_.reset(new Implementation);
}

ImageProvider::~ImageProvider() = default;

QString MediaProvider::ImageProvider::provider() const { return PROVIDER_NAME; }

Resource *ImageProvider::createResource(const QString &resource) {
  if (availableResources().contains(resource)) {
    return new ImageResource{getPath(resource), this};
  }
  setErrorString(
      "Unable to create unexisting resource, check available resources");
  return nullptr;
}

}  // namespace MediaProvider
