#include <media_provider/media_provider.h>
#include <QDir>
#include <QtTest>

class TestImageProvider : public QObject {
  Q_OBJECT

 public:
  TestImageProvider();
  ~TestImageProvider();

 private slots:
  void test_checkIfDefaultOriginIsCurrentPath();
  void test_setOrigin();
  void test_setUnexistsOrigin();
  void test_setEmptyOrigin();
  void test_checkAvailableResources();
  void test_createResource();

 private:
  MediaProvider::Provider* provider_ = {};
};

QStringList acceptedFiles{"file.jpg", "file.jpeg", "file.bmp", "file.png"};
QStringList inacceptedFiles{"file.gif", "file.svg"};

TestImageProvider::TestImageProvider() {
  provider_ = MediaProvider::Provider::createProvider("ImageProvider");
  QVERIFY(provider_);
  acceptedFiles.sort();
  for (auto& item : acceptedFiles) {
    QFile f(item);
    QVERIFY(f.open(QIODevice::ReadWrite));
    f.close();
  }
  for (auto& item : inacceptedFiles) {
    QFile f(item);
    QVERIFY(f.open(QIODevice::ReadWrite));
    f.close();
  }
}

TestImageProvider::~TestImageProvider() {
  delete provider_;
  for (auto& item : acceptedFiles) {
    QFile::remove(item);
  }
  for (auto& item : inacceptedFiles) {
    QFile::remove(item);
  }
}

void TestImageProvider::test_checkIfDefaultOriginIsCurrentPath() {
  QCOMPARE(provider_->origin(), QDir::currentPath());
}

void TestImageProvider::test_setOrigin() {
  QString newOrigin = "f:/mediaFolder";
  QVERIFY(provider_->setOrigin(newOrigin));
  QCOMPARE(newOrigin, provider_->origin());
}

void TestImageProvider::test_setUnexistsOrigin() {
  QString unexistsOrigin = "f:/mediaFolder/unexists";
  QCOMPARE(provider_->setOrigin(unexistsOrigin), false);
}

void TestImageProvider::test_setEmptyOrigin() {
  QVERIFY(provider_->setOrigin(""));
  QCOMPARE(provider_->origin(), QDir::currentPath());
}

void TestImageProvider::test_checkAvailableResources() {
  auto res = provider_->availableResources();
  QCOMPARE(res, acceptedFiles);
}

void TestImageProvider::test_createResource() {
  auto resources = provider_->availableResources();
  if (!resources.size()) {
    QFAIL("There is no resources");
  }
  auto resStr = resources.first();
  auto resource = provider_->createResource(resStr);
  QSignalSpy stateChangedSpy(resource, SIGNAL(stateChanged()));
  QVERIFY(resource);
  QCOMPARE(provider_->origin() + "/" + resStr, resource->resource());
  QVERIFY(stateChangedSpy.wait());
}

QTEST_MAIN(TestImageProvider)

#include "tst_image_provider.moc"
