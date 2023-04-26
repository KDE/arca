#include "arca.h"
#include <MauiKit/FileBrowsing/fmstatic.h>

Arca::Arca(QObject *parent) : QObject(parent)
  ,m_defaultSaveDir(FMStatic::DocumentsPath)
  ,m_settings(new QSettings(this))
{
    m_settings->beginGroup("General");
    m_defaultSaveDir = m_settings->value("DefaultSaveDir", m_defaultSaveDir).toString();
    m_settings->endGroup();
}

Arca::~Arca()
{
    m_settings->sync();
}

QString Arca::defaultSaveDir() const
{
    return m_defaultSaveDir;
}

void Arca::setDefaultSaveDir(QString defaultSaveDir)
{
    if (m_defaultSaveDir == defaultSaveDir)
        return;

    m_defaultSaveDir = defaultSaveDir;

    m_settings->beginGroup("General");
    m_settings->setValue("DefaultSaveDir", m_defaultSaveDir);
    m_settings->endGroup();

    Q_EMIT defaultSaveDirChanged(m_defaultSaveDir);
}
