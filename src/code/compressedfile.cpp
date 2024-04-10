#include "compressedfile.h"

#include <KTar>
#include <KZip>
#include <KAr>

#if (defined Q_OS_LINUX || defined Q_OS_FREEBSD) && !defined Q_OS_ANDROID
#include <K7Zip>
#endif

#include <QDirIterator>
#include <QDebug>

#include <MauiKit4/FileBrowsing/fmstatic.h>

#include "temporaryfile.h"


CompressedFile::CompressedFile(QObject *parent)
    : QObject(parent)
    , m_model(new CompressedFileModel(this))
{
}

CompressedFileModel::CompressedFileModel(QObject *parent)
    : MauiList(parent)
    ,m_archive(nullptr)
{
}

CompressedFileModel::~CompressedFileModel()
{
    qDeleteAll(m_previews);
}

const FMH::MODEL_LIST &CompressedFileModel::items() const
{
    return m_list;
}

void CompressedFileModel::setUrl(const QUrl &url)
{
    if(m_url == url)
    {
        return;
    }

    m_url = url;

    m_fileName = QFileInfo(url.toLocalFile()).baseName();
    Q_EMIT fileNameChanged(m_fileName);

    open();
    openDir(m_currentPath);
}

QString CompressedFileModel::currentPath() const
{
    return m_currentPath;
}

QString CompressedFileModel::fileName() const
{
    return m_fileName;
}

bool CompressedFileModel::canGoUp() const
{
    return m_canGoUp;
}

bool CompressedFileModel::opened() const
{
    return m_opened;
}

void CompressedFileModel::refresh()
{
    openDir(m_currentPath);
}

const KArchiveFile *CompressedFileModel::getFile(const QString &path)
{
    if(!m_archive)
    {
        return nullptr;
    }

    if(m_archive->isOpen())
    {
        return m_archive->directory()->file(path);
    }

    return nullptr;
}

void CompressedFileModel::openDir(const QString &path)
{
    if(m_url.isEmpty() || path.isEmpty())
    {
        return;
    }

           //                            if(m_currentPath == path)
           //            {
           //                            return;
           //        }

    if(m_archive->isOpen())
    {

        auto root = m_archive->directory();
        auto entry = root->entry(path);

        if(entry)
        {
            if(entry->isDirectory())
            {
                const KArchiveDirectory* subDir = static_cast<const KArchiveDirectory*>(entry);
                setCurrentPath(path);

                m_list.clear();
                Q_EMIT this->preListChanged();

                for(const auto &file : subDir->entries())
                {
                    const auto e = subDir->entry(file);
                    this->m_list << FMH::MODEL{{FMH::MODEL_KEY::IS_DIR, e->isDirectory() ? "true" : "false"}, {FMH::MODEL_KEY::LABEL, e->name()}, {FMH::MODEL_KEY::ICON, e->isDirectory() ? "folder" : FMStatic::getIconName(e->name())}, {FMH::MODEL_KEY::DATE, e->date().toString()}, {FMH::MODEL_KEY::PATH, QString(path+"%1/").arg(e->name())}, {FMH::MODEL_KEY::USER, e->user()}};

                }

                Q_EMIT this->postListChanged();
                Q_EMIT this->countChanged ();
            }
        }
    }

}

void CompressedFileModel::goUp()
{
    this->openDir(QUrl(m_currentPath).resolved(QUrl("..")).toString());
}

void CompressedFileModel::goToRoot()
{
    this->openDir("/");
}

void CompressedFileModel::close()
{

}

void CompressedFileModel::open()
{
    m_archive = CompressedFile::getKArchiveObject(m_url);

    m_archive->open(QIODevice::ReadOnly);


    assert(m_archive->isOpen() == true);

    m_opened = m_archive->isOpen();
    Q_EMIT openedChanged(m_opened);

}

QString CompressedFileModel::temporaryFile(const QString &path)
{
    if(m_previews.contains(path))
    {
        return m_previews.value(path)->url();
    }

    auto preview = new TemporaryFile;
    m_previews.insert(path, preview);

    auto file = getFile(path);
    preview->setData(file->data(), file->name());
    return preview->url();

}

bool CompressedFileModel::addFiles(const QStringList &urls, const QString &path)
{
    if(urls.isEmpty() || path.isEmpty())
    {
        return false;
    }

    bool success = false;


    m_archive->close();
    m_archive->open(QIODevice::ReadWrite);


    for(const auto &url : urls)
    {
        success = addFile(url, path);
    }

    m_archive->close();
    m_archive->open(QIODevice::ReadOnly);
    refresh();

    return success;
}

bool CompressedFileModel::addFile(const QString &url, const QString &path)
{


    auto localUrl = QUrl(url).toLocalFile();
    QFileInfo file(localUrl);

    if(!file.exists())
    {
        return false;
    }

    if(file.isDir())
    {
        return m_archive->addLocalDirectory(localUrl, path+file.fileName());
    }


    if(m_archive->addLocalFile(localUrl, path+file.fileName()))
    {
        qDebug() << "Trying to insert file to archive"<< url << localUrl << path << path+file.fileName();
        return true;

    }


    return false;

}

bool CompressedFileModel::extractFiles(const QStringList &urls, const QString &where)
{
    return false;

}

bool CompressedFileModel::extractFile(const QString &url, const QString &where)
{
    return false;

}

void CompressedFileModel::setCurrentPath(QString currentPath)
{
    if (m_currentPath == currentPath)
        return;

    m_currentPath = currentPath;
    Q_EMIT currentPathChanged(m_currentPath);

    m_canGoUp = m_currentPath != "/";
    Q_EMIT canGoUpChanged(m_canGoUp);
}

void CompressedFile::extract(const QUrl &where, const QString &directory)
{
    if (!m_url.isLocalFile())
        return;

    qDebug() << "@gadominguez File:fm.cpp Funcion: extractFile  "
             << "URL: " << m_url << "WHERE: " << where.toString() << " DIR: " << directory;

    QString where_ = where.toLocalFile() + "/" + directory;

    auto kArch = CompressedFile::getKArchiveObject(m_url);
    kArch->open(QIODevice::ReadOnly);

    qDebug() << "@gadominguez File:fm.cpp Funcion: extractFile  " << kArch->directory()->entries();

    assert(kArch->isOpen() == true);

    bool ok = false;
    if (kArch->isOpen())
    {
        ok = kArch->directory()->copyTo(where_, true);
        kArch->close ();

    }
    delete kArch;

    Q_EMIT this->extractionFinished (where.toString(), ok);
}

/*
 *
 *  CompressTypeSelected is an integer and has to be acorrding with order in Dialog.qml
 *
 */
bool CompressedFile::compress(const QStringList &files, const QUrl &where, const QString &fileName, const int &compressTypeSelected)
{
    auto fileWriter = [&where](KArchive *archive, QFile &file) -> bool
    {
        if(!archive)
            return false;

        return archive->writeFile(file.fileName().remove(where.toLocalFile(), Qt::CaseSensitivity::CaseSensitive), // Mirror file path in compressed file from current directory
                                  file.readAll(),
                                  0100775,
                                  QFileInfo(file).owner(),
                                  QFileInfo(file).group(),
                                  QDateTime(),
                                  QDateTime(),
                                  QDateTime());
    };

    auto dirWriter = [&fileWriter](KArchive *archive, QDirIterator &dir, bool &error)
    {
        if(!archive)
            return;

        while (dir.hasNext())
        {
            auto entrie = dir.next();

            if (QFileInfo(entrie).isFile())
            {
                auto file = QFile(entrie);
                file.open(QIODevice::ReadOnly);

                if (!file.isOpen())
                {
                    qDebug() << "ERROR. CURRENT USER DOES NOT HAVE PEMRISSION FOR WRITE IN THE CURRENT DIRECTORY.";
                    error = true;
                    continue;
                }

                error = fileWriter(archive, file);

                       // WriteFile returns if the file was written or not,
                       // but this function returns if some error occurs so for this reason it is needed to toggle the value
                error = !error;
            }
        }
    };

    auto url = [&where, &fileName](const int &type) -> QString
    {
        QString format;
        switch(type)
        {
        case 0: format = ".zip"; break;
        case 1: format = ".tar"; break;
        case 2: format = ".7zip"; break;
        case 3: format = ".ar"; break;
        }

        return QUrl(where.toString() + "/" + fileName + format).toLocalFile();
    };

    auto openCheck = [](KArchive *archive)
    {
        archive->open(QIODevice::ReadWrite);
        assert(archive->isOpen() == true);
    };

    bool error = true;
    const QString fileUrl = url(compressTypeSelected);

    assert(compressTypeSelected >= 0 && compressTypeSelected <= 8);

    for (const auto &uri : files)
    {
        qDebug() << "@gadominguez File:fm.cpp Funcion: compress  " << QUrl(uri).toLocalFile() << " " << fileName;

        if (!QFileInfo(QUrl(uri).toLocalFile()).isDir())
        {
            auto file = QFile(QUrl(uri).toLocalFile());
            file.open(QIODevice::ReadWrite);

            if (!file.isOpen())
            {
                qDebug() << "ERROR. CURRENT USER DOES NOT HAVE PEMRISSION FOR WRITE IN THE CURRENT DIRECTORY.";
                error = true;
                continue;
            }

                switch (compressTypeSelected)
                {
                case 0: //.ZIP
                {
                    auto kzip = new KZip(fileUrl);
                    openCheck(kzip);

                    error = fileWriter(kzip, file);

                    (void)kzip->close();
                    // WriteFile returns if the file was written or not,
                    // but this function returns if some error occurs so for this reason it is needed to toggle the value
                    error = !error;
                    break;
                }
                case 1: // .TAR
                {
                    auto ktar = new KTar(fileUrl);
                    openCheck(ktar);

                    error = fileWriter(ktar, file);

                    (void)ktar->close();
                    break;
                }
                case 2: //.7ZIP
                {
#ifdef K7ZIP_H

                           // TODO: KArchive no permite comprimir ficheros del mismo modo que con TAR o ZIP. Hay que hacerlo de otra forma y requiere disponer de una libreria actualizada de KArchive.
                    auto k7zip = new K7Zip(fileUrl);
                    openCheck(k7zip);

                    error = fileWriter(k7zip, file);

                    k7zip->close();
                    // WriteFile returns if the file was written or not,
                    // but this function returns if some error occurs so for this reason it is needed to toggle the value
                    error = !error;
#endif
                    break;
                }
                case 3: //.AR
                {
                    // TODO: KArchive no permite comprimir ficheros del mismo modo que con TAR o ZIP. Hay que hacerlo de otra forma y requiere disponer de una libreria actualizada de KArchive.
                    auto kar = new KAr(fileUrl);
                    openCheck(kar);

                    error = fileWriter(kar, file);

                    (void)kar->close();
                    // WriteFile returns if the file was written or not,
                    // but this function returns if some error occurs so for this reason it is needed to toggle the value
                    error = !error;
                    break;
                }
                default:
                    qDebug() << "ERROR. COMPRESSED TYPE SELECTED NOT COMPATIBLE";
                    break;
                }


        } else
        {
            qDebug() << "Dir: " << QUrl(uri).toLocalFile();
            auto dir = QDirIterator(QUrl(uri).toLocalFile(), QDirIterator::Subdirectories);

            switch (compressTypeSelected)
            {
            case 0: //.ZIP
            {
                auto kzip = new KZip(fileUrl);
                openCheck(kzip);

                dirWriter(kzip, dir, error);

                (void)kzip->close();
                break;
            }
            case 1: // .TAR
            {
                auto ktar = new KTar(fileUrl);
                openCheck(ktar);

                dirWriter(ktar, dir, error);

                (void)ktar->close();
                break;
            }
            case 2: //.7ZIP
            {
#ifdef K7ZIP_H

                auto k7zip = new K7Zip(fileUrl);
                openCheck(k7zip);

                dirWriter(k7zip, dir, error);

                (void)k7zip->close();
#endif
                break;
            }
            case 3: //.AR
            {
                auto kAr = new KAr(fileUrl);
                openCheck(kAr);

                dirWriter(kAr, dir, error);

                (void)kAr->close();
                break;
            }
            default:
                qDebug() << "ERROR. COMPRESSED TYPE SELECTED NOT COMPATIBLE";
                break;
            }
        }
    }

           // kzip->prepareWriting("Hello00000.txt", "gabridc", "gabridc", 1024, 0100777, QDateTime(), QDateTime(), QDateTime());
           // kzip->writeData("Hello", sizeof("Hello"));
           // kzip->finishingWriting();

    if(!error)
    {
        this->setUrl(QUrl::fromLocalFile(fileUrl));
    }

    Q_EMIT compressionFinished(fileUrl, !error);
    return error;
}

KArchive *CompressedFile::getKArchiveObject(const QUrl &url)
{
    KArchive *kArch = nullptr;

    /*
     * This checks depends on type COMPRESSED_MIMETYPES in file fmh.h
     */
    qDebug() << "@gadominguez File: fmstatic.cpp Func: getKArchiveObject MimeType: " << FMStatic::getMime(url);

    auto mime = FMStatic::getMime(url);

    if (mime.contains("application/x-tar") || mime.contains("application/x-compressed-tar"))
    {
        kArch = new KTar(url.toLocalFile());
    } else if (mime.contains("application/zip"))
    {
        kArch = new KZip(url.toLocalFile());

    } else if (mime.contains("application/x-7z-compressed"))
    {
#ifdef K7ZIP_H
        kArch = new K7Zip(url.toLocalFile());
#endif
    } else
    {
        qDebug() << "ERROR. COMPRESSED FILE TYPE UNKOWN " << url.toString();
    }

    return kArch;
}

void CompressedFile::setUrl(const QUrl &url)
{
    if (m_url == url)
        return;

    m_url = url;
    Q_EMIT this->urlChanged();

    if(!FMStatic::fileExists(url))
    {
        qWarning()<< "File does not exists and can not be opened.";
        return;
    }

    m_model->setUrl(m_url);
}

QUrl CompressedFile::url() const
{
    return m_url;
}

CompressedFileModel *CompressedFile::model() const
{
    return m_model;
}

