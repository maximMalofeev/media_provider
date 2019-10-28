#include <media_provider/media_provider.h>
#include <QDir>
#include <QtTest>

class TestVideoProvider : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase();
  void test_checkIfDefaultOriginIsCurrentPath();
  void test_setOrigin();
  void test_setUnexistsOrigin();
  void test_setEmptyOrigin();
  void test_checkAvailableResources();
  void cleanupTestCase();

 private:
  MediaProvider::Provider* provider_ = {};
};

QStringList acceptedFiles{"file.mp4", "file.avi", "file.webm"};
QStringList inacceptedFiles{"file.flv", "file.wmv"};

void TestVideoProvider::initTestCase() {
  provider_ = MediaProvider::Provider::createProvider("VIDEO_PROVIDER");
  QVERIFY(provider_);
  acceptedFiles.sort();
  for (auto& item : acceptedFiles) {
    QFile f(item);
    QVERIFY(f.open(QIODevice::ReadWrite));
  }
  for (auto& item : inacceptedFiles) {
    QFile f(item);
    QVERIFY(f.open(QIODevice::ReadWrite));
  }
}

void TestVideoProvider::test_checkIfDefaultOriginIsCurrentPath() {
  QCOMPARE(provider_->origin(), QDir::currentPath());
}

void TestVideoProvider::test_setOrigin() {
  QString newOrigin = "f:/mediaFolder";
  QVERIFY(provider_->setOrigin(newOrigin));
  QCOMPARE(newOrigin, provider_->origin());
}

void TestVideoProvider::test_setUnexistsOrigin() {
  QString unexistsOrigin = "f:/mediaFolder/unexists";
  QCOMPARE(provider_->setOrigin(unexistsOrigin), false);
}

void TestVideoProvider::test_setEmptyOrigin() {
  QVERIFY(provider_->setOrigin(""));
  QCOMPARE(provider_->origin(), QDir::currentPath());
}

void TestVideoProvider::test_checkAvailableResources() {
  provider_->initialise();
  auto res = provider_->availableResources();
  QCOMPARE(res, acceptedFiles);
}

void TestVideoProvider::cleanupTestCase() {
  provider_->deleteLater();
  for (auto& item : acceptedFiles) {
    QFile::remove(item);
  }
  for (auto& item : inacceptedFiles) {
    QFile::remove(item);
  }
}

QTEST_MAIN(TestVideoProvider)

#include "tst_video_provider.moc"
