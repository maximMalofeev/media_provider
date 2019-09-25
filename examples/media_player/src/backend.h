#ifndef BACKEND_H
#define BACKEND_H

#include <media_provider/media_provider.h>
#include <QObject>
#include <QVariant>

class QAbstractVideoSurface;

class Backend : public QObject {
  Q_OBJECT
  Q_PROPERTY(QStringList providers READ providers CONSTANT)
  Q_PROPERTY(
      MediaProvider::Provider* provider MEMBER provider_ NOTIFY providerChanged)
  Q_PROPERTY(
      MediaProvider::Resource* resource MEMBER resource_ NOTIFY resourceChanged)
  Q_PROPERTY(QAbstractVideoSurface* videoSurface MEMBER videoSurface_)

 public:
  explicit Backend(QObject* parent = nullptr);
  ~Backend();

  QStringList providers() const;

 signals:
  void providerChanged();
  void resourceChanged();

 public slots:
  void createProvider(const QString& providerName);
  void createResource(const QString& resourceName);
  void onNewFrame(QImage image, qlonglong timestamp);

 private:
  MediaProvider::Provider* provider_{};
  MediaProvider::Resource* resource_{};
  QAbstractVideoSurface* videoSurface_{};
};

#endif  // BACKEND_H
