#ifndef SAPERARESOURCE_H
#define SAPERARESOURCE_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class DalsaResource : public Resource {
  Q_OBJECT

 public:
  DalsaResource(const QString& res, QObject* parent = nullptr);
  ~DalsaResource();

  QSize size() const override;
  bool setSize(const QSize& size) override;
  QList<QVariant> availableSizes() const override;
  QList<QVariant> availableColorFormats() const override;
  QImage::Format colorFormat() const override;
  bool setColorFormat(QImage::Format format) override;
  Stream* stream() override;

  void initialise() override;

 public slots:
  void onServerDisconnected(const QString& serverName);

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}  // namespace MediaProvider

#endif  // SAPERARESOURCE_H
