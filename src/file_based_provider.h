#ifndef FILEBASEDPROVIDER_H
#define FILEBASEDPROVIDER_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class FileBasedProvider : public Provider {
 public:
  FileBasedProvider(const QStringList &filters, QObject *parent = nullptr);
  ~FileBasedProvider() override;

  bool setOrigin(const QString &orig) override;
  void initialise() override;

 protected:
  void timerEvent(QTimerEvent *event) override;
  QString getPath(const QString& resource) const;

 private:
  Q_DISABLE_COPY(FileBasedProvider)
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

}  // namespace MediaProvider

#endif  // FILEBASEDPROVIDER_H
