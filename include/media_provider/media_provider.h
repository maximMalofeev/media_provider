#ifndef MEDIA_PROVIDER_H
#define MEDIA_PROVIDER_H

#include <QtCore/qglobal.h>
#if defined(MEDIAPROVIDERLIB_LIBRARY)
#define MEDIAPROVIDERLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#define MEDIAPROVIDERLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#include <QImage>
#include <QObject>
#include <QSize>
#include <QVariant>

class QAbstractVideoSurface;

namespace MediaProvider {

class Resource;

class MEDIAPROVIDERLIBSHARED_EXPORT Stream : public QObject {
  Q_OBJECT
  Q_PROPERTY(State state READ state NOTIFY stateChanged)
  Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)

 public:
  /**
   * @brief The State enum describe current Stream state
   */
  enum State { Stopped, Playing, Invalid };
  Q_ENUM(State)

  ~Stream();
  /**
   * @brief current stream state
   * @return stream state
   */
  virtual State state() const;
  virtual QString errorString() const;

 public slots:
  /**
   * @brief start receiving frames from the stream
   */
  virtual void start() = 0;
  /**
   * @brief stop receiving frames from the stream
   */
  virtual void stop() = 0;

 signals:
  void errorStringChanged();
  void stateChanged();
  void newFrame(QImage frame, qlonglong timestamp);

 protected:
  explicit Stream(Resource* parent = nullptr);
  Q_DISABLE_COPY_MOVE(Stream)

  void setState(State state);
  void setErrorString(const QString& errorStr);
  Resource* resource() const;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

class MEDIAPROVIDERLIBSHARED_EXPORT Resource : public QObject {
  Q_OBJECT
  Q_PROPERTY(State state READ state NOTIFY stateChanged)
  Q_PROPERTY(QString resource READ resource CONSTANT)
  Q_PROPERTY(QList<QVariant> availableSizes READ availableSizes NOTIFY
                 availableSizesChanged)
  Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)
  Q_PROPERTY(QList<QVariant> availableColorFormats READ availableColorFormats
                 NOTIFY availableColorFormatsChanged)
  Q_PROPERTY(QImage::Format colorFormat READ colorFormat WRITE setColorFormat
                 NOTIFY colorFormatChanged)
  Q_PROPERTY(MediaProvider::Stream* stream READ stream NOTIFY streamChanged)
  Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
 public:
  /**
   * @brief The State enum describes resource state,
   * NotInitialised - default state of new object
   * Initialising - while trying to load a resource
   * Initialised - if resource loaded successfully
   * Invalid - if resouce loading failed
   */
  enum State { NotInitialised, Initialising, Initialised, Invalid };
  Q_ENUM(State)

  ~Resource();
  /**
   * @brief current resource state
   * @return current state
   */
  virtual State state() const;
  /**
   * @brief resource string representation
   * @return resource string representation
   */
  virtual QString resource() const;
  /**
   * @brief returns the dimensions of a resource
   * @return dimentions
   */
  virtual QSize size() const = 0;
  /**
   * @brief set the dimensions of a resource
   * @param size - size, received from availableSizes() method
   * @return true if success, false otherwise
   */
  virtual bool setSize(const QSize& size) = 0;
  /**
   * @brief return list of available sizes, at list current size
   * @return available sizes list
   */
  virtual QList<QVariant> availableSizes() const = 0;
  /**
   * @brief return list of available color formats
   * @return available color formats list
   */
  virtual QList<QVariant> availableColorFormats() const = 0;
  /**
   * @brief return the color format of a resource
   * @return color format
   */
  virtual QImage::Format colorFormat() const = 0;
  /**
   * @brief set the color format to a resource
   * @param format - color format, received from availableColorFormats()
   * method
   * @return true if success, false otherwise
   */
  virtual bool setColorFormat(QImage::Format format) = 0;
  /**
   * @brief get the Stream object of a resource
   * @return stream
   */
  virtual Stream* stream() = 0;
  /**
   * @brief return a string describing the last error
   * @return last error string
   */
  virtual QString errorString() const;

 public slots:
  /**
   * @brief initialise resource
   */
  virtual void initialise() = 0;

 signals:
  void stateChanged();
  void availableSizesChanged();
  void sizeChanged();
  void availableColorFormatsChanged();
  void colorFormatChanged();
  void streamChanged();
  void errorStringChanged();

 protected:
  explicit Resource(QObject* parent = nullptr);
  Q_DISABLE_COPY_MOVE(Resource)

  void setState(State state);
  void setResource(const QString& resource);
  void setErrorString(const QString& errorStr);

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

class MEDIAPROVIDERLIBSHARED_EXPORT Provider : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString provider READ provider CONSTANT)
  Q_PROPERTY(State state READ state NOTIFY stateChanged)
  Q_PROPERTY(QString origin READ origin WRITE setOrigin NOTIFY originChanged)
  Q_PROPERTY(QStringList availableResources READ availableResources NOTIFY
                 availableResourcesChanged)
  Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
 public:
  /**
   * @brief The State enum describes provider state,
   * NotInitialised - default state of new object
   * Initialising - while trying to load a resource
   * Initialised - if resource loaded successfully
   * Invalid - if resouce loading failed
   */
  enum State { NotInitialised, Initialising, Initialised, Invalid };
  Q_ENUM(State)

  ~Provider();
  /**
   * @brief provider name
   * @return provider name
   */
  virtual QString provider() const = 0;
  /**
   * @brief current provider state
   * @return current state
   */
  virtual State state() const;
  /**
   * @brief this function return origin of available resources
   * @return string representation of resources origin
   */
  virtual QString origin() const;
  /**
   * @brief set origin of resources, if provider was initialised
   * reinitialise with the new origin
   * @param orig - if case of file resources it is a parent dir,
   * empty origin assume app current path
   * @return true if succes(start reinitialise), otherwise false
   */
  virtual bool setOrigin(const QString& orig);
  /**
   * @brief this function return available resources of current origin
   * @return list of available resources
   */
  virtual QStringList availableResources() const;
  /**
   * @brief creates Resource* from string representation, the resource is a
   * child of the Provider
   * @param resource - resource string representation, could be
   * received from availableResources() method
   * @return Resource* if success, nullptr otherwise
   */
  virtual Resource* createResource(const QString& resource) = 0;
  /**
   * @brief return a string describing the last error
   * @return last error string
   */
  virtual QString errorString() const;
  /**
   * @brief this function create provider
   * @param providerName - name of desired provider
   * @return Provider* if succes, otherwise nullptr
   */
  static Provider* createProvider(const QString& providerName,
                                  QObject* parent = nullptr);
  /**
   * @brief this function return list of available providers
   * @return list of available providers
   */
  static QStringList availableProviders();

 public slots:
  /**
   * @brief initialise provider with an origin
   */
  virtual void initialise() = 0;

 signals:
  void stateChanged();
  void originChanged();
  void availableResourcesChanged();
  void errorStringChanged();

 protected:
  explicit Provider(QObject* parent = nullptr);
  Q_DISABLE_COPY_MOVE(Provider)

  void setState(State state);
  void setAvailableResources(const QStringList& availableResources);
  void setErrorString(const QString& errorStr);

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

}  // namespace MediaProvider

#endif  // MEDIA_PROVIDER_H
