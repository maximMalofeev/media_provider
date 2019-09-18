#include <media_provider/media_provider.h>
#include <QAbstractVideoSurface>
#include <QDebug>
#include "image_provider.h"
#include "rtsp_provider.h"

#ifdef WIN32
#include <SapClassBasic.h>
#include "sapera_win_provider.h"
#endif

#include "video_provider.h"

namespace MediaProvider {

Stream::Stream(QObject *parent) : QObject(parent) {}

Resource::Resource(QObject *parent) : QObject(parent) {}

Provider::Provider(QObject *parent) : QObject(parent) {}

Provider *Provider::createProvider(const QString &providerName,
                                   QObject *parent) {
  if (providerName == "ImageProvider") {
    return new ImageProvider(parent);
  } else if (providerName == "VideoProvider") {
    return new VideoProvider;
  } else if (providerName == "RtspProvider") {
    return new RtspProvider;
  } else if (providerName == "SaperaProvider") {
#ifdef WIN32
    SapManager::SetDisplayStatusMode(SapManager::StatusLog);
#endif
    return new SaperaProvider;
  } else {
    return {};
  }
}

QStringList Provider::availableProviders() {
  return QStringList{"ImageProvider", "VideoProvider", "RtspProvider",
                     "SaperaProvider"};
}

}  // namespace MediaProvider
