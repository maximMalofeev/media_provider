#include "video_provider.h"
#include <QDir>
#include "video_resource.h"

namespace MediaProvider {

const QString VideoProvider::PROVIDER_NAME = "VIDEO_PROVIDER";

struct VideoProvider::Implementation {};

VideoProvider::VideoProvider(QObject *parent)
    : FileBasedProvider({"*.avi", "*.mp4", "*.webm"}, parent) {
  impl_.reset(new Implementation);
}

VideoProvider::~VideoProvider() = default;

QString VideoProvider::provider() const { return PROVIDER_NAME; }

Resource *VideoProvider::createResource(const QString &resource) {
  if (availableResources().contains(resource)) {
    return new VideoResource{getPath(resource), this};
  }
  setErrorString(
      "Unable to create unexisting resource, check available resources");
  return nullptr;
}

}  // namespace MediaProvider
