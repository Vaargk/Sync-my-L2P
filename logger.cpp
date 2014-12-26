#include <QSettings>
#include <QFileDialog>

#include "logger.h"
#include "ui_logger.h"

#include "qslog/QsLog.h"
#include "qslog/QsLogDest.h"

#include "utils.h"

Logger::Logger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Logger)
{
    ui->setupUi(this);

    ui->logLevelCB->addItem("standard");
    ui->logLevelCB->addItem("erweitert");

    QsLogging::Logger::instance().addDestination(QsLogging::DestinationFactory::MakeFunctorDestination(this, SLOT(logSlot(QString,int))));
}

Logger::~Logger()
{
    delete ui;
}

void Logger::loadSettings()
{
    QSettings settings;
    settings.beginGroup("logger");
    ui->logLevelCB->setCurrentText(settings.value("logLevel", "standard").toString());
    settings.endGroup();
}

void Logger::saveSettings()
{
    QSettings settings;
    settings.beginGroup("logger");
    settings.setValue("logLevel", ui->logLevelCB->currentText());
}

/// Empfänger für alle Nachrichten, die im Log auftauchen sollen
void Logger::logSlot(QString message, int level)
{
    (void) level;
    ui->logList->addItem(message);
}

/// Ausgewählte Logstufe an den Logger weitergeben
void Logger::on_logLevelCB_currentIndexChanged(const QString &logLevel)
{
    if(logLevel == QString("standard"))
    {
        QsLogging::Logger::instance().setLoggingLevel(QsLogging::InfoLevel);
        QLOG_INFO() << "Setze Logging auf \"standard\".";
    }
    else if(logLevel == QString("erweitert"))
    {
        QsLogging::Logger::instance().setLoggingLevel(QsLogging::TraceLevel);
        QLOG_INFO() << "Setze Logging auf \"erweitert\".";
    }
}

void Logger::on_savePB_clicked()
{
    QString textToWrite = getLogText();

    QString filepath = QFileDialog::getSaveFileName(this,
                                                    "Speicherort für das Logfile",
                                                    "",
                                                    "Textdateien (*.txt)");

    QLOG_DEBUG() << "Ausgewählter Speicherort für das Logfile: " << filepath;

    QFile file(filepath);
    if(!file.open(QIODevice::WriteOnly))
    {
        QLOG_ERROR() << "Fehler beim initialisieren des Logfiles: " << file.errorString();
        return;
    }

    if(file.write(textToWrite.toLatin1()) == -1)
    {
        QLOG_ERROR() << "Fehler beim Schreiben des Logfiles: " << file.errorString();
        return;
    }

    file.close();
}

void Logger::on_copyPB_clicked()
{
    Utils::copyTextToClipboard(getLogText());
}

QString Logger::getLogText()
{
    QList<QListWidgetItem*> items = ui->logList->findItems("*", Qt::MatchWildcard);

    QString logText("");

    foreach(QListWidgetItem* item, items)
    {
        logText.append( item->text() + "\r\n");
    }

    return logText;
}
