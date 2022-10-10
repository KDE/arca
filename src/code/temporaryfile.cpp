#include "temporaryfile.h"
#include <QDebug>
#include <QUrl>

TemporaryFile::TemporaryFile(QObject *parent) : QObject(parent)
  ,m_file(new QTemporaryFile(this))
{

}

void TemporaryFile::setData(const QByteArray &data, const QString &fileName)
{
//    m_file->setFileName(fileName);

  if(m_file->open())
{
qDebug() << "trying to write preview data" << fileName;
      m_file->write(data);
      m_file->close();

//      m_file->rename(fileName);
      Q_EMIT fileReady(m_file->fileName());
      qDebug() << "trying to write preview data" << m_file->fileName();


  }
}

QString TemporaryFile::url()
{
    return QUrl::fromLocalFile(m_file->fileName()).toString();
}
