#include <media_provider/media_provider.h>
#include <QDir>
#include <QtTest>

class TestRtspProvider : public QObject {
  Q_OBJECT

 public:
  TestRtspProvider();
  ~TestRtspProvider();

 private slots:
  void test_checkIfDefaultOriginIsRtspIni();
  void test_setUnexistsOrigin();
  void test_setEmptyOrigin();
  void test_setOrigin();
  void test_checkAvailableResources();
  void test_createResource();

 private:
  MediaProvider::Provider* provider_ = {};
  QStringList availableResources_ = {
    "rtsp://172.21.100.145/Streaming/Channels/1",
    "rtsp://172.21.100.145/Streaming/Channels/2",
    "rtsp://172.21.100.145/Streaming/Channels/3"
  };
};

QString newOrigin = "rtsp_origin.ini";

TestRtspProvider::TestRtspProvider() {
  provider_ = MediaProvider::Provider::createProvider("RtspProvider");
  QVERIFY(provider_);

  QFile rtspOrigin(newOrigin);
  QVERIFY(rtspOrigin.open(QIODevice::ReadWrite | QIODevice::Text));
  for(const auto& item : availableResources_){
    rtspOrigin.write(item.toUtf8());
    rtspOrigin.write("\n");
  }
  rtspOrigin.close();
}

TestRtspProvider::~TestRtspProvider() {
  delete provider_;
  QFile::remove(newOrigin);
}

void TestRtspProvider::test_checkIfDefaultOriginIsRtspIni() {
  QCOMPARE(provider_->origin(), "rtsp_provider.ini");
}

void TestRtspProvider::test_setUnexistsOrigin() {
  QString unexistsOrigin = "f:/mediaFolder/unexists";
  QCOMPARE(provider_->setOrigin(unexistsOrigin), false);
}

void TestRtspProvider::test_setEmptyOrigin() {
  QVERIFY(provider_->setOrigin(""));
  QCOMPARE(provider_->origin(), "rtsp_provider.ini");
}

void TestRtspProvider::test_setOrigin() {
  QVERIFY(provider_->setOrigin(newOrigin));
  QCOMPARE(newOrigin, provider_->origin());
}

void TestRtspProvider::test_checkAvailableResources() {
  auto res = provider_->availableResources();
  QCOMPARE(res, availableResources_);
}

void TestRtspProvider::test_createResource() {
  auto resources = provider_->availableResources();
  if (!resources.size()) {
    QFAIL("There is no resources");
  }
  auto resStr = resources.first();
  auto resource = provider_->createResource(resStr);
  QVERIFY(resource);
  QCOMPARE(resStr, resource->resource());
}

QTEST_MAIN(TestRtspProvider)

#include "tst_rtsp_provider.moc"
