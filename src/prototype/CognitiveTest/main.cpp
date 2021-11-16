#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include "../../auxiliary/CommandLineParser.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("CLSA");
    QApplication::setOrganizationDomain("clsa-elcv.ca");
    QApplication::setApplicationName("CognitiveTest");
    QApplication::setApplicationVersion("1.0.0");

    QApplication app(argc, argv);

    // Need to pull changes from development branch to get all features of commandLineParser
    /*CommandLineParser parser;
    QString errMessage;
    switch (parser.parseCommandLine(app, &errMessage))
    {
    case CommandLineParser::CommandLineHelpRequested:
        QMessageBox::warning(0, QGuiApplication::applicationDisplayName(),
            "<html><head/><body><pre>"
            + parser.helpText() + "</pre></body></html>");
        return 0;
    case  CommandLineParser::CommandLineVersionRequested:
        QMessageBox::information(0, QGuiApplication::applicationDisplayName(),
            QGuiApplication::applicationDisplayName() + ' '
            + QCoreApplication::applicationVersion());
        return 0;
    case CommandLineParser::CommandLineOk:
        break;
    case CommandLineParser::CommandLineError:
    case CommandLineParser::CommandLineInputFileError:
    case CommandLineParser::CommandLineOutputPathError:
    case CommandLineParser::CommandLineMissingArg:
    case CommandLineParser::CommandLineModeError:
        QMessageBox::warning(0, QGuiApplication::applicationDisplayName(),
            "<html><head/><body><h2>" + errMessage + "</h2><pre>"
            + parser.helpText() + "</pre></body></html>");
        return 1;
    }*/

    // Start App
    CognitiveTest w;
    w.setInputFileName("C:/Users/clsa/source/repos/CypressBuilds/CognitiveTest/CognitiveTestInput.json");
    w.setOutputFileName("C:/Users/clsa/source/repos/CypressBuilds/CognitiveTest/Output");
    w.setMode("live");
    w.setVerbose(true);

    w.initialize();
    w.show();
    w.run();
    return app.exec();
}
