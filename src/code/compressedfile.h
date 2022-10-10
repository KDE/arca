#pragma once

#include <QObject>

#include <MauiKit/Core/fmh.h>
#include <MauiKit/Core/mauilist.h>

class TemporaryFile;
class KArchive;
class KArchiveFile;
class CompressedFileModel : public MauiList
{
    Q_OBJECT
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
    Q_PROPERTY(bool canGoUp READ canGoUp NOTIFY canGoUpChanged)
    Q_PROPERTY(bool opened READ opened NOTIFY openedChanged)

public:
    explicit CompressedFileModel(QObject *parent);
    ~CompressedFileModel();
    const FMH::MODEL_LIST &items() const override final;

    void setUrl(const QUrl &url);

    QString currentPath() const;

    QString fileName() const;

    bool canGoUp() const;

    bool opened() const;

    const KArchiveFile *getFile(const QString &path);

public slots:
    void openDir(const QString &path);
    void goUp();
    void goToRoot();
    void close();
    void open();

    QString temporaryFile(const QString &path);

    void setCurrentPath(QString currentPath);

signals:
    void currentPathChanged(QString currentPath);

    void fileNameChanged(QString fileName);

    void canGoUpChanged(bool canGoUp);

    void openedChanged(bool opened);

private:
    KArchive *m_archive;

    QHash<QString, TemporaryFile*> m_previews;
    FMH::MODEL_LIST m_list;
    QUrl m_url;
    QString m_currentPath = "/";
    QString m_fileName;
    bool m_canGoUp = false;
    bool m_opened;
};

class CompressedFile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(CompressedFileModel *model READ model CONSTANT FINAL)

public:
    explicit CompressedFile(QObject *parent = nullptr);
    static KArchive *getKArchiveObject(const QUrl &url);

    void setUrl(const QUrl &url);
    QUrl url() const;

    CompressedFileModel *model() const;

private:
    QUrl m_url;
    CompressedFileModel *m_model;

public slots:
    void extract(const QUrl &where, const QString &directory = QString());
    bool compress(const QVariantList &files, const QUrl &where, const QString &fileName, const int &compressTypeSelected);

signals:
    void urlChanged();
    void extractionFinished(QUrl url);
    void compressionFinished(QUrl url);
};

