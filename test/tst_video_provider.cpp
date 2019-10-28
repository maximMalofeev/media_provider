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

static QStringList acceptedFiles{"file.mp4", "file.avi", "file.webm"};
static QStringList inacceptedFiles{"file.flv", "file.wmv"};

static QString tmpDir = "./mediaFolder";

void TestVideoProvider::initTestCase() {
  provider_ = MediaProvider::Provider::createProvider("VIDEO_PROVIDER");
  QVERIFY(provider_);
  acceptedFiles.sort();
  for (auto& item : acceptedFiles) {
    QFile f(item);
    QVERIFY(f.open(QIODevice::ReadWrite));
    f.close();
  }

  QDir dir;
  QVERIFY(dir.mkdir(tmpDir));

  for (auto& item : inacceptedFiles) {
    QFile f(item);
    QVERIFY(f.open(QIODevice::ReadWrite));
  }
}

void TestVideoProvider::test_checkIfDefaultOriginIsCurrentPath() {
  QCOMPARE(provider_->origin(), QDir::currentPath());
}

void TestVideoProvider::test_setOrigin() {
#ifdef WIN32
  QString newOrigin = "f:/mediaFolder";
#else
  QString newOrigin = tmpDir;
#endif
  QVERIFY(provider_->setOrigin(newOrigin));
  QCOMPARE(newOrigin, provider_->origin());
}

void TestVideoProvider::test_setUnexistsOrigin() {
#ifdef WIN32
  QString unexistsOrigin = "f:/mediaFolder/unexists";
#else
  QString unexistsOrigin = "/mediaFolder/unexists";
#endif
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
  delete provider_;

  QDir dir;
  dir.rmdir(tmpDir);

  for (auto& item : acceptedFiles) {
    QFile::remove(item);
  }
  for (auto& item : inacceptedFiles) {
    QFile::remove(item);
  }
}

QTEST_MAIN(TestVideoProvider)

#include "tst_video_provider.moc"
