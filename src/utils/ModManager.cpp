#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <src/utils/Registry.h>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QNetworkReply>
#include "ModManager.h"

/**
 * Downloads one file at a time for a given mod, recurse if it encounters a folder
 * Put the downloaded files in destination
 * @param mod
 * @param destination
 */
void ModManager::download(QString &mod, QString destination) {
    // @todo separate files discovery and download
    QStringList list;

    listFilesRecursively(mod, list);

    int i = 1;
    for (auto &file : list) {
        qInfo() << "Downloading" << i++ << "/" << list.size() << file << "...";
        downloadAndSave(file, destination);
    }

    qDebug() << "Installed.";
    emit installed();
}

/**
 * Creates a list of all the files to download in a given directory and puts them in "list" synchronously.
 * @param directory
 */
void ModManager::listFilesRecursively(QString &directory, QStringList &list) {
    m_fetcher.getSync(m_baseurl + directory, [this, &list] (QNetworkReply* reply) {
        QJsonArray rootObj = QJsonDocument::fromJson(reply->readAll()).array();

        for (auto obj: rootObj) {
            QString type = obj.toObject().value("type").toString();
            QString relative_path = obj.toObject().value("path").toString();

            if (type == "dir") {
                qDebug() << "Directory found, recursing...";
                listFilesRecursively(relative_path, list);
            } else if (type == "file") {
                qDebug() << "File found, added to download list...";
                list.push_back(relative_path);
            } else {
                qDebug() << "Unknown type: " << type;
            }
        }
    });
}

/**
 * Downloads what lies behind a target url to GORN mods directory + file relative path. Creates directories if needed.
 * @param relative_path
 * @param destination
 */
void ModManager::downloadAndSave(QString relative_path, QString destination) {
    m_fetcher.getSync(m_baseurl + relative_path, [this, relative_path, destination] (QNetworkReply* reply) {
        QString dest_path = QDir(destination).filePath(relative_path);
        QFile file(QDir(destination).filePath(relative_path));

        // creates directory if it does not exist
        QDir(QFileInfo(file).absoluteDir()).mkpath(".");

        // @todo check if open went well
        file.open(QIODevice::WriteOnly);
        file.write(reply->readAll());
        file.close();
    });
}

void ModManager::remove(QString &mod) {
    QString mod_path = QDir(QDir(Registry::getPlatformConfig().path).filePath("GORN_Data/mods")).filePath(mod);
    QDir dir(mod_path);

    qDebug() << "Removing " << mod_path;

    dir.removeRecursively();

    emit removed();
}

/**
 * Lists all the mods available in github's repository
 */
void ModManager::listAvailable() {
    m_fetcher.get(m_baseurl, [this] (QNetworkReply* reply) {
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
        QJsonArray root = document.array();
        QStringList list;
        for (auto obj: root) {
            QString mod_name = obj.toObject().value("name").toString();
            if (not mod_name.contains("MemeLoader")) {
                list.push_back(mod_name);
            }
        }
        emit availableListed(list);
    });
}

/**
 * Lists all the mods installed in user's directory
 */
void ModManager::listInstalled() {
    std::vector<QString> files;

    QDir mods_dir(QDir(Registry::getPlatformConfig().path).filePath("GORN_Data/mods"));
    QStringList list = mods_dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    list.removeOne("MemeLoader");

    emit installedListed(list);
}


