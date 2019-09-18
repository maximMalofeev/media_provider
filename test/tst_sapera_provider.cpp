#include <media_provider/media_provider.h>
#include <QtTest>

class TestSaperaProvider : public QObject {
  Q_OBJECT

 public:
  TestSaperaProvider();
  ~TestSaperaProvider();

 private slots:
  void test_checkIfDefaultOriginIsSaperaServer();
  void test_setOrigin();
  void test_setUnexistsOrigin();
  void test_setEmptyOrigin();

 private:
  MediaProvider::Provider* provider_ = {};
};

TestSaperaProvider::TestSaperaProvider() {
  provider_ = MediaProvider::Provider::createProvider("SaperaProvider");
  QVERIFY(provider_);
}

TestSaperaProvider::~TestSaperaProvider() {
  delete provider_;
}

void TestSaperaProvider::test_checkIfDefaultOriginIsSaperaServer() {
  QCOMPARE(provider_->origin(), "sapera_server");
}

void TestSaperaProvider::test_setOrigin() {
  QString newOrigin = "sapera_server";
  QVERIFY(provider_->setOrigin(newOrigin));
  QCOMPARE(newOrigin, provider_->origin());
}

void TestSaperaProvider::test_setUnexistsOrigin() {
  QString unexistsOrigin = "unexistsOrigin";
  QCOMPARE(provider_->setOrigin(unexistsOrigin), false);
}

void TestSaperaProvider::test_setEmptyOrigin() {
  QVERIFY(provider_->setOrigin(""));
  QCOMPARE(provider_->origin(), "sapera_server");
}

QTEST_MAIN(TestSaperaProvider)

#include "tst_sapera_provider.moc"
