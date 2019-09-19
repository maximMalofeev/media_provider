#include "video_provider.h"
#include <QDir>
#include "video_resource.h"

namespace MediaProvider {

struct VideoProvider::Implementation {};

VideoProvider::VideoProvider(QObject *parent)
    : FileBasedProvider({"*.avi", "*.mp4", "*.webm"}, parent) {
  impl_.reset(new Implementation);
}

VideoProvider::~VideoProvider() = default;

Resource *VideoProvider::createResource(const QString &resource) const {
  if (availableResources().contains(resource)) {
    return new VideoResource(getPath(resource));
  }
  setErrorString(
      "Unable to create unexisting resource, check available resources");
  return nullptr;
}

}  // namespace MediaProvider
