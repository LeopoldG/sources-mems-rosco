#ifndef ABOUTBOX_H
#define ABOUTBOX_H

#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QStyle>
#include <QString>
#include "rosco.h"

/**
 * The "About" dialog that shows information about the program.
 */
class AboutBox : public QDialog
{
    Q_OBJECT

public:
    AboutBox(QStyle *parentStyle, QString title, librosco_version version, QWidget *parent);

private:
    void setupWidgets();
    QString makeVersionString(int maj, int min, int patch);

    QStyle *m_style;
    QGridLayout *m_grid;

    librosco_version m_ver;

    const QString m_urlString;
    const QString m_urlString2;
    const QString m_urlString3;
    const QString m_urlString4;
    const QString m_urlLibString;
    const QString m_aboutString;
    const QString m_urlString5;
    QLabel *m_iconLabel;
    QLabel *m_info;
    QLabel *m_name;
    QLabel *m_infoLib;
    QLabel *m_url;
    QLabel *m_url2;
    QLabel *m_urlDownload;
    QLabel *m_urlLib;
    QLabel *m_urlForum;
    QPushButton *m_ok;
};

#endif // ABOUTBOX_H

