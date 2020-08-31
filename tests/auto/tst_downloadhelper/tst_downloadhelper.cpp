/****************************************************************************
**
** Copyright (C) 2015-2016 Jolla Ltd.
** Copyright (c) 2020 Open Mobile Platform LLC.
** Contact: Raine Makelainen <raine.makelainen@jolla.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "downloadhelper.h"

#include <QtTest>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

static const QByteArray TEST_CONTENT = "Hello World!";

class tst_downloadhelper : public QObject
{
    Q_OBJECT

public:
    tst_downloadhelper(QObject *parent = 0);

private slots:
    void init();
    void cleanup();

    void uniqueFileName_data();
    void uniqueFileName();

private:
    SailfishOS::WebEngineUtils::DownloadHelper *downloadHelper;
    QString dataLocation;
};

tst_downloadhelper::tst_downloadhelper(QObject *parent)
    : QObject(parent)
    , downloadHelper(new SailfishOS::WebEngineUtils::DownloadHelper(this))
    , dataLocation(QStandardPaths::writableLocation(QStandardPaths::DataLocation))
{
}

void tst_downloadhelper::init()
{
    // set up test case
    QDir dir(dataLocation);
    // Fail on test init if direction creation doesn't work.
    // If this assert occurs there is something else work in the system.
    bool dirCreated = dir.mkpath(dataLocation);
    Q_ASSERT_X(dirCreated, Q_FUNC_INFO, QString("Can't create directory %1").arg(dataLocation).constData());
}

void tst_downloadhelper::cleanup()
{
    // tear down
    QDir dir(dataLocation);
    dir.removeRecursively();
}

void tst_downloadhelper::uniqueFileName_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QList<QString> >("existingFiles");
    QTest::addColumn<QString>("expectedName");

    QList<QString> existingFiles;

    existingFiles.clear();
    QTest::newRow("new_file") << "some_picture.jpg" << existingFiles << "some_picture.jpg";

    existingFiles.clear();
    QTest::newRow("new_file_no_ext") << "some_file" << existingFiles << "some_file";

    existingFiles.clear();
    QTest::newRow("one_symbol_name") << "a" << existingFiles << "a";

    existingFiles.clear();
    existingFiles << "a";
    QTest::newRow("file_exists_one_symbol_name") << "a" << existingFiles << "a(2)";

    existingFiles.clear();
    QTest::newRow("one_dot_name") << "." << existingFiles << "unnamed_file";

    existingFiles.clear();
    QTest::newRow("two_dots_name") << ".." << existingFiles << "unnamed_file";

    existingFiles.clear();
    QTest::newRow("name_with_dots_at_the_end") << "some_file.tar.gz..." << existingFiles << "some_file.tar.gz";

    existingFiles.clear();
    QTest::newRow("one_space_name") << " " << existingFiles << "unnamed_file";

    existingFiles.clear();
    QTest::newRow("two_space_name") << "  " << existingFiles << "unnamed_file";

    existingFiles.clear();
    QTest::newRow("name_with_leading_spaces") << "   some_file.tar.gz" << existingFiles << "some_file.tar.gz";

    existingFiles.clear();
    QTest::newRow("name_with_trailing_spaces") << "some_file.tar.gz   " << existingFiles << "some_file.tar.gz";

    existingFiles.clear();
    QTest::newRow("name_with_leading_dots") << "...some_file.tar.gz" << existingFiles << "...some_file.tar.gz";

    existingFiles.clear();
    QTest::newRow("name_with_trailing_dots") << "some_file.tar.gz..." << existingFiles << "some_file.tar.gz";

    existingFiles.clear();
    QTest::newRow("name_with_leading_underscores") << "___some_file.tar.gz" << existingFiles << "some_file.tar.gz";

    existingFiles.clear();
    QTest::newRow("name_with_trailing_underscores") << "some_file.tar.gz___" << existingFiles << "some_file.tar.gz";

    existingFiles.clear();
    existingFiles << "some_picture.jpg";
    QTest::newRow("file_exists") << "some_picture.jpg" << existingFiles << "some_picture(2).jpg";

    existingFiles.clear();
    existingFiles << "some_picture.jpg";
    QTest::newRow("new_file_2") << "some_picture.png" << existingFiles << "some_picture.png";

    existingFiles.clear();
    existingFiles << "some-picture.jpg";
    QTest::newRow("new_file_3") << "some-picture.png" << existingFiles << "some-picture.png";

    existingFiles.clear();
    existingFiles << "some_picture.jpg" << "some_picture(2).jpg" << "some_picture(3).jpg";
    QTest::newRow("many_files_exist") << "some_picture.jpg" << existingFiles << "some_picture(4).jpg";

    existingFiles.clear();
    existingFiles << "some_picture.jpg" << "some_picture(3).jpg";
    QTest::newRow("many_files_exist_2") << "some_picture.jpg" << existingFiles << "some_picture(2).jpg";

    existingFiles.clear();
    existingFiles << "some_picture(2).jpg" << "some_picture(3).jpg";
    QTest::newRow("many_files_exist_3") << "some_picture.jpg" << existingFiles << "some_picture.jpg";

    existingFiles.clear();
    existingFiles << "and(2)_some(2).picture.jpg" << "and(2)_some(2)(2).picture.jpg";
    QTest::newRow("complicated_case") << "and(2)_some(2).picture.jpg" << existingFiles << "and(2)_some(2)(3).picture.jpg";

    existingFiles.clear();
    existingFiles << "some_file.tar.gz";
    QTest::newRow("file_exists_complex_ext") << "some_file.tar.gz" << existingFiles << "some_file(2).tar.gz";

    existingFiles.clear();
    existingFiles << "some_file";
    QTest::newRow("file_exists_no_ext") << "some_file" << existingFiles << "some_file(2)";

    existingFiles.clear();
    existingFiles << "some_file" << "some_file(2)" << "some_file(3)";
    QTest::newRow("many_files_exist_no_ext") << "some_file" << existingFiles << "some_file(4)";

    existingFiles.clear();
    QTest::newRow("illegal_symbols_in_name") << "some /\\?%*:|\"<> file name.tar.gz" << existingFiles << "some_file_name.tar.gz";

    existingFiles.clear();
    existingFiles << "some_file_name.tar.gz";
    QTest::newRow("file_exists_illegal_symbols_in_name") << "some /\\?%*:|\"<> file name.tar.gz" << existingFiles << "some_file_name(2).tar.gz";

    existingFiles.clear();
    QTest::newRow("name_with_whitespace_sequences") << "some \t\n\v\f\r file \t \n \v \f \r name.tar.gz" << existingFiles << "some_file_name.tar.gz";

    const QString prefix = QStringLiteral("some_file.");
    const QString extension = QStringLiteral(".tar.gz");
}

void tst_downloadhelper::uniqueFileName()
{
    QFETCH(QString, fileName);
    QFETCH(QList<QString>, existingFiles);
    QFETCH(QString, expectedName);

    foreach (const QString& existingFile, existingFiles) {
        QFile file(dataLocation + "/" + existingFile);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream out(&file);
        out << TEST_CONTENT;
        file.close();
    }

    // actual test
    QCOMPARE(downloadHelper->createUniqueFileUrl(fileName, dataLocation), dataLocation + "/" + expectedName);
}

QTEST_GUILESS_MAIN(tst_downloadhelper)

#include "tst_downloadhelper.moc"
