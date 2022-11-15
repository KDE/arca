#include "compressedfile.h"

#include <KArchive/KTar>
#include <KArchive/KZip>
#include <KArchive/kcompressiondevice.h>
#include <KArchive/kfilterdev.h>

#if (defined Q_OS_LINUX || defined Q_OS_FREEBSD) && !defined Q_OS_ANDROID
#include <KArchive/k7zip.h>
#endif

#include <KArchive/kar.h>

#include <QDirIterator>
#include <QDebug>

#include <MauiKit/FileBrowsing/fmstatic.h>

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
                emit this->preListChanged();

                for(const auto file : subDir->entries())
                {
                    const auto e = subDir->entry(file);
                    this->m_list << FMH::MODEL{{FMH::MODEL_KEY::IS_DIR, e->isDirectory() ? "true" : "false"}, {FMH::MODEL_KEY::LABEL, e->name()}, {FMH::MODEL_KEY::ICON, e->isDirectory() ? "folder" : FMStatic::getIconName(e->name())}, {FMH::MODEL_KEY::DATE, e->date().toString()}, {FMH::MODEL_KEY::PATH, QString(path+"%1/").arg(e->name())}, {FMH::MODEL_KEY::USER, e->user()}};

                }

                emit this->postListChanged();
                emit this->countChanged ();
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
    emit currentPathChanged(m_currentPath);

    m_canGoUp = m_currentPath != "/";
    emit canGoUpChanged(m_canGoUp);
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

    if (kArch->isOpen())
    {
        if(kArch->directory()->copyTo(where_, true))
        {
            emit this->extractionFinished (where);
        }

        kArch->close ();

    }
    delete kArch;
}

/*
                                 *
                                 *  CompressTypeSelected is an integer and has to be acorrding with order in Dialog.qml
                                 *
                                 */
bool CompressedFile::compress(const QVariantList &files, const QUrl &where, const QString &fileName, const int &compressTypeSelected)
{
    bool error = true;
    assert(compressTypeSelected >= 0 && compressTypeSelected <= 8);
    for (const auto &uri : files) {
        qDebug() << "@gadominguez File:fm.cpp Funcion: compress  " << QUrl(uri.toString()).toLocalFile() << " " << fileName;

        if (!QFileInfo(QUrl(uri.toString()).toLocalFile()).isDir()) {
            auto file = QFile(QUrl(uri.toString()).toLocalFile());
            file.open(QIODevice::ReadWrite);
            if (file.isOpen() == true) {
                switch (compressTypeSelected) {
                case 0: //.ZIP
                {
                    auto kzip = new KZip(QUrl(where.toString() + "/" + fileName + ".zip").toLocalFile());
                    kzip->open(QIODevice::ReadWrite);
                    assert(kzip->isOpen() == true);

                    error = kzip->writeFile(uri.toString().remove(where.toString(), Qt::CaseSensitivity::CaseSensitive), // Mirror file path in compressed file from current directory
                                            file.readAll(),
                                            0100775,
                                            QFileInfo(file).owner(),
                                            QFileInfo(file).group(),
                                            QDateTime(),
                                            QDateTime(),
                                            QDateTime());
                    (void)kzip->close();
                    // WriteFile returns if the file was written or not,
                    // but this function returns if some error occurs so for this reason it is needed to toggle the value
                    error = !error;
                    break;
                }
                case 1: // .TAR
                {
                    auto ktar = new KTar(QUrl(where.toString() + "/" + fileName + ".tar").toLocalFile());
                    ktar->open(QIODevice::ReadWrite);
                    assert(ktar->isOpen() == true);
                    error = ktar->writeFile(uri.toString().remove(where.toString(), Qt::CaseSensitivity::CaseSensitive), // Mirror file path in compressed file from current directory
                                            file.readAll(),
                                            0100775,
                                            QFileInfo(file).owner(),
                                            QFileInfo(file).group(),
                                            QDateTime(),
                                            QDateTime(),
                                            QDateTime());
                    (void)ktar->close();
                    break;
                }
                case 2: //.7ZIP
                {
#ifdef K7ZIP_H

                    // TODO: KArchive no permite comprimir ficheros del mismo modo que con TAR o ZIP. Hay que hacerlo de otra forma y requiere disponer de una libreria actualizada de KArchive.
                    auto k7zip = new K7Zip(QUrl(where.toString() + "/" + fileName + ".7z").toLocalFile());
                    k7zip->open(QIODevice::ReadWrite);
                    assert(k7zip->isOpen() == true);
                    error = k7zip->writeFile(uri.toString().remove(where.toString(), Qt::CaseSensitivity::CaseSensitive), // Mirror file path in compressed file from current directory
                                             file.readAll(),
                                             0100775,
                                             QFileInfo(file).owner(),
                                             QFileInfo(file).group(),
                                             QDateTime(),
                                             QDateTime(),
                                             QDateTime());
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
                    auto kar = new KAr(QUrl(where.toString() + "/" + fileName + ".ar").toLocalFile());
                    kar->open(QIODevice::ReadWrite);
                    assert(kar->isOpen() == true);
                    error = kar->writeFile(uri.toString().remove(where.toString(), Qt::CaseSensitivity::CaseSensitive), // Mirror file path in compressed file from current directory
                                           file.readAll(),
                                           0100775,
                                           QFileInfo(file).owner(),
                                           QFileInfo(file).group(),
                                           QDateTime(),
                                           QDateTime(),
                                           QDateTime());
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
            } else {
                qDebug() << "ERROR. CURRENT USER DOES NOT HAVE PEMRISSION FOR WRITE IN THE CURRENT DIRECTORY.";
                error = true;
            }
        } else {
            qDebug() << "Dir: " << QUrl(uri.toString()).toLocalFile();
            auto dir = QDirIterator(QUrl(uri.toString()).toLocalFile(), QDirIterator::Subdirectories);
            switch (compressTypeSelected) {
            case 0: //.ZIP
            {
                auto kzip = new KZip(QUrl(where.toString() + "/" + fileName + ".zip").toLocalFile());
                kzip->open(QIODevice::ReadWrite);
                assert(kzip->isOpen() == true);
                while (dir.hasNext()) {
                    auto entrie = dir.next();

                    qDebug() << entrie << " " << where.toString() << QFileInfo(entrie).isFile();
                    if (QFileInfo(entrie).isFile() == true) {
                        auto file = QFile(entrie);
                        file.open(QIODevice::ReadOnly);
                        qDebug() << entrie << entrie.remove(QUrl(where).toLocalFile(), Qt::CaseSensitivity::CaseSensitive);
                        error = kzip->writeFile(entrie.remove(QUrl(where).toLocalFile(), Qt::CaseSensitivity::CaseSensitive), // Mirror file path in compressed file from current directory
                                                file.readAll(),
                                                0100775,
                                                QFileInfo(file).owner(),
                                                QFileInfo(file).group(),
                                                QDateTime(),
                                                QDateTime(),
                                                QDateTime());
                        // WriteFile returns if the file was written or not,
                        // but this function returns if some error occurs so for this reason it is needed to toggle the value
                        error = !error;
                    }
                }
                (void)kzip->close();
                break;
            }
            case 1: // .TAR
            {
                auto ktar = new KTar(QUrl(where.toString() + "/" + fileName + ".tar").toLocalFile());
                ktar->open(QIODevice::ReadWrite);
                assert(ktar->isOpen() == true);
                while (dir.hasNext()) {
                    auto entrie = dir.next();

                    qDebug() << entrie << " " << where.toString() << QFileInfo(entrie).isFile();
                    if (QFileInfo(entrie).isFile() == true) {
                        auto file = QFile(entrie);
                        file.open(QIODevice::ReadOnly);
                        qDebug() << entrie << entrie.remove(QUrl(where).toLocalFile(), Qt::CaseSensitivity::CaseSensitive);
                        error = ktar->writeFile(entrie.remove(QUrl(where).toLocalFile(), Qt::CaseSensitivity::CaseSensitive), // Mirror file path in compressed file from current directory
                                                file.readAll(),
                                                0100775,
                                                QFileInfo(file).owner(),
                                                QFileInfo(file).group(),
                                                QDateTime(),
                                                QDateTime(),
                                                QDateTime());
                        // WriteFile returns if the file was written or not,
                        // but this function returns if some error occurs so for this reason it is needed to toggle the value
                        error = !error;
                    }
                }
                (void)ktar->close();
                break;
            }
            case 2: //.7ZIP
            {
#ifdef K7ZIP_H

                auto k7zip = new K7Zip(QUrl(where.toString() + "/" + fileName + ".7z").toLocalFile());
                k7zip->open(QIODevice::ReadWrite);
                assert(k7zip->isOpen() == true);
                while (dir.hasNext()) {
                    auto entrie = dir.next();

                    qDebug() << entrie << " " << where.toString() << QFileInfo(entrie).isFile();
                    if (QFileInfo(entrie).isFile() == true) {
                        auto file = QFile(entrie);
                        file.open(QIODevice::ReadOnly);
                        qDebug() << entrie << entrie.remove(QUrl(where).toLocalFile(), Qt::CaseSensitivity::CaseSensitive);
                        error = k7zip->writeFile(entrie.remove(QUrl(where).toLocalFile(), Qt::CaseSensitivity::CaseSensitive), // Mirror file path in compressed file from current directory
                                                 file.readAll(),
                                                 0100775,
                                                 QFileInfo(file).owner(),
                                                 QFileInfo(file).group(),
                                                 QDateTime(),
                                                 QDateTime(),
                                                 QDateTime());
                        // WriteFile returns if the file was written or not,
                        // but this function returns if some error occurs so for this reason it is needed to toggle the value
                        error = !error;
                    }
                }
                (void)k7zip->close();
#endif
                break;
            }
            case 3: //.AR
            {
                auto kAr = new KAr(QUrl(where.toString() + "/" + fileName + ".ar").toLocalFile());
                kAr->open(QIODevice::ReadWrite);
                assert(kAr->isOpen() == true);
                while (dir.hasNext()) {
                    auto entrie = dir.next();

                    qDebug() << entrie << " " << where.toString() << QFileInfo(entrie).isFile();
                    if (QFileInfo(entrie).isFile() == true) {
                        auto file = QFile(entrie);
                        file.open(QIODevice::ReadOnly);
                        qDebug() << entrie << entrie.remove(QUrl(where).toLocalFile(), Qt::CaseSensitivity::CaseSensitive);
                        error = kAr->writeFile(entrie.remove(QUrl(where).toLocalFile(), Qt::CaseSensitivity::CaseSensitive), // Mirror file path in compressed file from current directory
                                               file.readAll(),
                                               0100775,
                                               QFileInfo(file).owner(),
                                               QFileInfo(file).group(),
                                               QDateTime(),
                                               QDateTime(),
                                               QDateTime());
                        // WriteFile returns if the file was written or not,
                        // but this function returns if some error occurs so for this reason it is needed to toggle the value
                        error = !error;
                    }
                }
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

    return error;
}

KArchive *CompressedFile::getKArchiveObject(const QUrl &url)
{
    KArchive *kArch = nullptr;

    /*
                                     * This checks depends on type COMPRESSED_MIMETYPES in file fmh.h
                                     */
    qDebug() << "@gadominguez File: fmstatic.cpp Func: getKArchiveObject MimeType: " << FMStatic::getMime(url);

    if (FMStatic::getMime(url).contains("application/x-tar") || FMStatic::getMime(url).contains("application/x-compressed-tar")) {
        kArch = new KTar(url.toLocalFile());
    } else if (FMStatic::getMime(url).contains("application/zip")) {
        kArch = new KZip(url.toLocalFile());
    } else if (FMStatic::getMime(url).contains("application/x-7z-compressed")) {
#ifdef K7ZIP_H
        kArch = new K7Zip(url.toLocalFile());
#endif
    } else {
        qDebug() << "ERROR. COMPRESSED FILE TYPE UNKOWN " << url.toString();
    }

    return kArch;
}

void CompressedFile::setUrl(const QUrl &url)
{
    if (m_url == url)
        return;

    m_url = url;
    emit this->urlChanged();

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

