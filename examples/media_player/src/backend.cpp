#include "backend.h"
#include <media_provider/media_provider.h>
#include <QAbstractVideoSurface>
#include <QDebug>
#include <QVideoSurfaceFormat>

Backend::Backend(QObject *parent) : QObject(parent) {}

Backend::~Backend() {
  qDebug() << "Backend::~Backend()";
  if(resource_){
    delete resource_;
  }
  if(provider_){
    delete provider_;
  }
}

QStringList Backend::providers() const {
  return MediaProvider::Provider::availableProviders();
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
    if (resource_->state() == MediaProvider::Resource::Initialised) {
      if (videoSurface_) {
        qDebug() << "==VideoSurface available==";
        if (videoSurface_->isActive()) {
          videoSurface_->stop();
        }
        if (resource_) {
          QVideoSurfaceFormat format(
              resource_->size(), QVideoFrame::pixelFormatFromImageFormat(
                                     QImage::Format_RGB32));
          videoSurface_->nearestFormat(format);
          videoSurface_->start(format);
        }
      }
    } else {
      connect(resource_, &MediaProvider::Resource::stateChanged, [this]() {
        if (resource_->state() == MediaProvider::Resource::Initialised) {
          if (videoSurface_) {
            qDebug() << "==VideoSurface available==";
            if (videoSurface_->isActive()) {
              videoSurface_->stop();
            }
            if (resource_) {
              QVideoSurfaceFormat format(
                  resource_->size(), QVideoFrame::pixelFormatFromImageFormat(
                                         QImage::Format_RGB32));
              videoSurface_->nearestFormat(format);
              videoSurface_->start(format);
            }
          }
        }
      });
    }
    connect(resource_->stream(), &MediaProvider::Stream::newFrame, this,
            &Backend::onNewFrame);
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
