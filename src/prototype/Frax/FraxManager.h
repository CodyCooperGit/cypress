#ifndef FRAXMANAGER_H
#define FRAXMANAGER_H

#include "../../managers/ManagerBase.h"
#include "../../data/FraxTest.h"

#include <QProcess>
#include <QDir>

class FraxManager : public ManagerBase
{
	Q_OBJECT
public:
    explicit FraxManager(QObject* parent = nullptr);

    void loadSettings(const QSettings&) override;
    void saveSettings(QSettings*) const override;

    QJsonObject toJsonObject() const override;

    void buildModel(QStandardItemModel*) const override {};

    // is the passed string an executable file
    // with the correct path elements ?
    //
    bool isDefined(const QString&) const;

    // set the cognitive test executable full path and name
    // calls isDefined to validate the passed arg
    //
    void setExecutableName(const QString&);

    // call just before closing the application to
    // remove the output txt file from the test if it exists
    //
    void clean();

    QString getExecutableName() const
    {
        return m_executablePath;
    }

    QString getExecutableFullPath() const
    {
        return m_executablePath;
    }

    QString getInputFullPath() const
    {
        return m_inputPath;
    }

    QString getOldInputFullPath() const
    {
        return m_oldInputPath;
    }

    QString getOutputFullPath() const
    {
        return m_outputPath;
    }

    QMap<QString, QVariant> m_inputData;
    QMap<QString, QVariant> m_outputData;
public slots:
    void measure();

    void setInputs(const QMap<QString, QVariant>&);

    void readOutput();

signals:
    void canMeasure();

    void canWrite();
private:
    QString m_executableName;
    QString m_executablePath;
    QString m_outputPath;
    QString m_inputPath;
    QString m_oldInputPath;
    QProcess m_process;

    FraxTest m_test;

    void clearData() override;
    bool createInputsTxt();
    void runBlackBoxExe();
    void readOutputs();
};

#endif // FRAXMANAGER_H