#include <media_provider/media_provider.h>
#include <QtTest>

class TestSaperaProvider : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase();
  void test_checkIfDefaultOriginIsSaperaServer();
  void test_setOrigin();
  void test_setUnexistsOrigin();
  void test_setEmptyOrigin();
  void cleanupTestCase();

 private:
  MediaProvider::Provider* provider_ = {};
};

void TestSaperaProvider::initTestCase() {
  provider_ = MediaProvider::Provider::createProvider("DALSA_PROVIDER");
  QVERIFY(provider_);
}

void TestSaperaProvider::test_checkIfDefaultOriginIsSaperaServer() {
  QCOMPARE(provider_->origin(), "dalsa_server");
}

void TestSaperaProvider::test_setOrigin() {
  QString newOrigin = "dalsa_server";
  QVERIFY(provider_->setOrigin(newOrigin));
  QCOMPARE(newOrigin, provider_->origin());
}

void TestSaperaProvider::test_setUnexistsOrigin() {
  QString unexistsOrigin = "unexistsOrigin";
  QCOMPARE(provider_->setOrigin(unexistsOrigin), false);
}

void TestSaperaProvider::test_setEmptyOrigin() {
  QVERIFY(provider_->setOrigin(""));
  QCOMPARE(provider_->origin(), "dalsa_server");
}

void TestSaperaProvider::cleanupTestCase() { delete provider_; }

QTEST_MAIN(TestSaperaProvider)

#include "tst_sapera_provider.moc"
