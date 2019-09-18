#ifndef SAPERARESOURCE_H
#define SAPERARESOURCE_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class SaperaResource : public Resource {
  Q_OBJECT

 public:
  SaperaResource(const QString& resource, QObject* parent = nullptr);
  ~SaperaResource();

  State state() const override;
  QString resource() const override;
  QSize size() const override;
  bool setSize(const QSize& size) override;
  QList<QVariant> availableSizes() const override;
  QList<QVariant> availableColorFormats() const override;
  QImage::Format colorFormat() const override;
  bool setColorFormat(const QImage::Format format) override;
  Stream* stream() override;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}  // namespace MediaProvider

#endif  // SAPERARESOURCE_H
