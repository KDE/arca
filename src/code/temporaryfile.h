#ifndef TEMPORARYFILE_H
#define TEMPORARYFILE_H

#include <QObject>
#include <QTemporaryFile>

class TemporaryFile : public QObject
{
    Q_OBJECT
public:
    explicit TemporaryFile(QObject *parent = nullptr);

    void setData(const QByteArray &data, const QString &fileName);

    QString url();

private:
    QTemporaryFile *m_file;

signals:

    void fileReady(QString url);

};

#endif // TEMPORARYFILE_H
