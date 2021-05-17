/****************************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
** All rights reserved.
**
****************************************************************************************/

#include <QObject>

// This file is used only for documentation purposes

// These items are all part of QMozContext, but inherited by WebEngine, so we
// take some notational liberty and present them as part of the subclass.

namespace SailfishOS {

class WebEngine : public QMozContext {
    Q_OBJECT
    Q_PROPERTY(bool initialized READ isInitialized NOTIFY initialized)
public:
    // C++ API
    static void initialize(const QString &profilePath, bool runEmbedding = true);
    static WebEngine *instance();

    explicit WebEngine(QObject *parent = 0);
    virtual ~WebEngine();

    void addObservers(const std::vector<std::string> &aObserversList);
    void removeObservers(const std::vector<std::string> &aObserversList);

    Q_INVOKABLE bool isInitialized() const;

Q_SIGNALS:
    void initialized();
    void contextDestroyed();
    void lastViewDestroyed();
    void lastWindowDestroyed();
    void recvObserve(const QString message, const QVariant data);

public Q_SLOTS:
    void addComponentManifest(const QString &manifestPath);
    void addObserver(const QString &aTopic);
    void removeObserver(const QString  aTopic);
    void notifyObservers(const QString &topic, const QString &value);
    void notifyObservers(const QString &topic, const QVariant &value);
    void runEmbedding(int aDelay = -1);
    void stopEmbedding();
    void notifyFirstUIInitialized();
    void setProfile(const QString &profilePath);
    void setViewCreator(QMozViewCreator *viewCreator);
    void createView();
};

} // namespace SailfishOS
