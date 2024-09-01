#include <QApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>
#include <QIcon>
#include <QQmlContext>

#include <MauiKit4/Core/mauiapp.h>
#include <MauiKit4/Archiver/moduleinfo.h>

#include <KAboutData>
#include <KLocalizedString>

#include "../project_version.h"

#include "code/arca.h"

//Useful for setting quickly an app template
#define ORG_NAME "Maui"
#define PROJECT_NAME "Arca"
#define COMPONENT_NAME "arca"
#define PRODUCT_NAME "maui/arca"
#define PROJECT_PAGE "https://mauikit.org"
#define REPORT_PAGE "https://github.com/Nitrux/arca/issues/new"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName(QStringLiteral(ORG_NAME));
    app.setWindowIcon(QIcon(":/logo.png"));


    KLocalizedString::setApplicationDomain(COMPONENT_NAME);

    KAboutData about(QStringLiteral(COMPONENT_NAME),
                     QStringLiteral(PROJECT_NAME),
                     PROJECT_VERSION_STRING,
                     i18n("Archive manager and explorer."),
                     KAboutLicense::LGPL_V3,
                    APP_COPYRIGHT_NOTICE,
                     QString(GIT_BRANCH) + "/" + QString(GIT_COMMIT_HASH));

    about.addAuthor(QStringLiteral("Camilo Higuita"), i18n("Developer"), QStringLiteral("milo.h@aol.com"));

    about.setHomepage(PROJECT_PAGE);
    about.setProductName(PRODUCT_NAME);
    about.setBugAddress(REPORT_PAGE);
    about.setOrganizationDomain(PROJECT_URI);
    about.setProgramLogo(app.windowIcon());

    const auto TData = MauiKitArchiver::aboutData();
    about.addComponent(TData.name(), MauiKitArchiver::buildVersion(), TData.version(), TData.webAddress());

    KAboutData::setApplicationData(about);
    MauiApp::instance()->setIconName("qrc:/logo.svg");

    QCommandLineParser parser;
    parser.setApplicationDescription(about.shortDescription());
    parser.process(app);
    about.processCommandLine(&parser);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    const QUrl url(QStringLiteral("qrc:/app/maui/arca/controls/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);


    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    qmlRegisterSingletonInstance<Arca>(PROJECT_URI, 1, 0, "Arc", Arca::instance());

    engine.load(url);

    return app.exec();
}
