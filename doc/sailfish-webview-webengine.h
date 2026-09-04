/****************************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
** All rights reserved.
**
****************************************************************************************/

#include <QObject>

// This file is used only for documentation purposes

namespace SailfishOS {

class WebEngine : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool initialized READ isInitialized NOTIFY initialized)
public:
    // C++ API
    typedef void (*TaskCallback)(void *data);
    typedef void *TaskHandle;

    static void initialize(const QString &profilePath, bool runEmbedding = true);
    static WebEngine *instance();

    explicit WebEngine(QObject *parent = 0);
    virtual ~WebEngine();

    void addObservers(const std::vector<std::string> &aObserversList);
    void removeObservers(const std::vector<std::string> &aObserversList);
    int getNumberOfWindows() const;

    Q_INVOKABLE bool isInitialized() const;
    Q_INVOKABLE bool isAccelerated() const;

    TaskHandle PostUITask(TaskCallback callback, void *data, int timeout = 0);
    TaskHandle PostCompositorTask(TaskCallback callback, void *data, int timeout = 0);
    void CancelTask(TaskHandle handle);

Q_SIGNALS:
    void initialized();
    void contextDestroyed();
    void lastWindowDestroyed();
    void recvObserve(const QString message, const QVariant data);

public Q_SLOTS:
    void setIsAccelerated(bool accelerated);
    void addObserver(const QString &aTopic);
    void removeObserver(const QString  aTopic);
    void notifyObservers(const QString &topic, const QString &value);
    void notifyObservers(const QString &topic, const QVariant &value);
    void addUserStyleSheet(const QUrl &url);
    void removeUserStyleSheet(const QUrl &url);
    void runEmbedding(int aDelay = -1);
    void stopEmbedding();
    void notifyFirstUIInitialized();
};

} // namespace SailfishOS
