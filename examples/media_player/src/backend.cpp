#include "backend.h"
#include <media_provider/media_provider.h>
#include <QAbstractVideoSurface>
#include <QDebug>
#include <QVideoSurfaceFormat>

Backend::Backend(QObject *parent) : QObject(parent) {}

Backend::~Backend() {
  if (provider_) {
    delete provider_;
  }
}

QStringList Backend::availableProviders() const {
  return MediaProvider::Provider::availableProviders();
}

void Backend::resetProvider() {
  resource_ = nullptr;
  if (provider_) {
    provider_->deleteLater();
    provider_ = nullptr;
    emit providerChanged();
  }
}

void Backend::applyResource() {
  if (videoSurface_) {
    if (videoSurface_->isActive()) {
      videoSurface_->stop();
    }
    if (resource_) {
      QVideoSurfaceFormat format(
          resource_->size(),
          QVideoFrame::pixelFormatFromImageFormat(QImage::Format_RGB32));
      videoSurface_->nearestFormat(format);
      videoSurface_->start(format);
    }
  }
  connect(resource_->stream(), &MediaProvider::Stream::newFrame, this,
          &Backend::onNewFrame);
}

void Backend::createProvider(const QString &providerName) {
  auto provider = MediaProvider::Provider::createProvider(providerName, this);
  if (provider) {
    qDebug() << providerName << "created";
    if (provider_) {
      provider_->deleteLater();
    }
    provider_ = provider;
    emit providerChanged();
    provider_->setOrigin("");
  }
}

void Backend::createResource(const QString &resourceName) {
  auto resource = provider_->createResource(resourceName);
  if (resource) {
    qDebug() << resourceName << "created";
    if (resource_) {
      resource_->deleteLater();
    }
    resource_ = resource;
    resource_->initialise();
    emit resourceChanged();
  }
}

void Backend::onNewFrame(QImage image, qlonglong timestamp) {
  Q_UNUSED(timestamp)
  QVideoFrame frame(image.convertToFormat(QImage::Format_RGB32));
  if (!frame.isValid()) {
    qWarning() << "frame invalid";
    return;
  }
  if (!videoSurface_->present(frame)) {
    qWarning() << "unable to present frame";
  }
}
