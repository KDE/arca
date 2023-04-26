#pragma once

#include <QObject>
#include <QString>
#include <QSettings>

class Arca : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Arca)

    Q_PROPERTY(QString defaultSaveDir READ defaultSaveDir WRITE setDefaultSaveDir NOTIFY defaultSaveDirChanged)

public:
    static Arca * instance()
    {
        static Arca arca;
        return &arca;
    }

    QString defaultSaveDir() const;
    void setDefaultSaveDir(QString defaultSaveDir);

private:
    explicit Arca(QObject *parent = nullptr);
    ~Arca();

    QString m_defaultSaveDir;
    QSettings *m_settings;

Q_SIGNALS:
    void defaultSaveDirChanged(QString defaultSaveDir);
};

