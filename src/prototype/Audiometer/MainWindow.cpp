#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QCloseEvent>
#include <QDate>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QSettings>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainWindow)
    , m_verbose(false)
    , m_manager(this)
{
    ui->setupUi(this);

    // allocate 2 columns x 8 rows of hearing measurement items
    //
    for(int col=0;col<2;col++)
    {
      for(int row=0;row<8;row++)
      {
        QStandardItem* item = new QStandardItem();
        m_model.setItem(row,col,item);
      }
    }
    m_model.setHeaderData(0,Qt::Horizontal,"Left",Qt::DisplayRole);
    m_model.setHeaderData(1,Qt::Horizontal,"Right",Qt::DisplayRole);
    ui->testdataTableView->setModel(&m_model);

    ui->testdataTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->testdataTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->testdataTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->testdataTableView->verticalHeader()->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// set up signal slot connections between GUI front end
// and device management back end
//
void MainWindow::initialize()
{
  m_manager.setVerbose(m_verbose);
  m_manager.setMode(m_mode);

  // Read inputs, such as interview barcode
  //
  readInput();

  // Populate barcode display
  //
  if(m_inputData.contains("barcode") && m_inputData["barcode"].isValid())
     ui->barcodeLineEdit->setText(m_inputData["barcode"].toString());
  else
     ui->barcodeLineEdit->setText("00000000"); // dummy

  // Read the .ini file for cached device data
  //
  QDir dir = QCoreApplication::applicationDirPath();
  QSettings settings(dir.filePath(m_manager.getGroup() + ".ini"), QSettings::IniFormat);
  m_manager.loadSettings(settings);

  // disable all buttons by default
  //
  for(auto&& x : this->findChildren<QPushButton *>())
      x->setEnabled(false);
  /**
  // Save button to store measurement and device info to .json
  //
  ui->saveButton->setEnabled(false);

  // Read the measurement off the device
  //
  ui->measureButton->setEnabled(false);

  // Connect to the device
  //
  ui->connectButton->setEnabled(false);

  // Disconnect from the device
  //
  ui->disconnectButton->setEnabled(false);
  */
  // Close the application
  //
  ui->closeButton->setEnabled(true);

  // Scan for devices
  //
  connect(&m_manager, &AudiometerManager::scanningDevices,
          this,[this]()
    {
      ui->deviceComboBox->clear();
      ui->statusBar->showMessage("Discovering serial ports...");
    }
  );

  // Update the drop down list as devices are discovered during scanning
  //
  connect(&m_manager, &AudiometerManager::deviceDiscovered,
          this, &MainWindow::updateDeviceList);

  // Prompt user to select a device from the drop down list when previously
  // cached device information in the ini file is unavailable or invalid
  //
  connect(&m_manager, &AudiometerManager::canSelectDevice,
          this,[this](){
      ui->statusBar->showMessage("Ready to select...");
      QMessageBox::warning(
        this, QApplication::applicationName(),
        tr("Select the port from the list.  If the device "
        "is not in the list, quit the application and check that the port is "
        "working and connect the audiometer to it before running this application."));
  });

  // Select a device (serial port) from drop down list
  //
  connect(ui->deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this,[this]()
    {
      if(m_verbose)
          qDebug() << "device selected from list " <<  ui->deviceComboBox->currentText();
      m_manager.selectDevice(ui->deviceComboBox->currentText());
    }
  );

  // Ready to connect device
  //
  connect(&m_manager, &AudiometerManager::canConnectDevice,
          this,[this](){
      ui->statusBar->showMessage("Ready to connect...");
      ui->connectButton->setEnabled(true);
      ui->disconnectButton->setEnabled(false);
      ui->measureButton->setEnabled(false);
      ui->saveButton->setEnabled(false);
  });

  // Connect to device
  //
  connect(ui->connectButton, &QPushButton::clicked,
          &m_manager, &AudiometerManager::connectDevice);

  // Connection is established: enable measurement requests
  //
  connect(&m_manager, &AudiometerManager::canMeasure,
          this,[this](){
      ui->statusBar->showMessage("Ready to measure...");
      ui->connectButton->setEnabled(false);
      ui->disconnectButton->setEnabled(true);
      ui->measureButton->setEnabled(true);
  });

  // Disconnect from device
  //
  connect(ui->disconnectButton, &QPushButton::clicked,
          &m_manager, &AudiometerManager::disconnectDevice);

  // Request a measurement from the device
  //
  connect(ui->measureButton, &QPushButton::clicked,
        &m_manager, &AudiometerManager::measure);

  // Update the UI with any data
  //
  connect(&m_manager, &AudiometerManager::dataChanged,
          this,[this](){
      m_manager.buildModel(&m_model);

      QHeaderView *h = ui->testdataTableView->horizontalHeader();
      QSize ts_pre = ui->testdataTableView->size();
      h->resizeSections(QHeaderView::ResizeToContents);
      ui->testdataTableView->setColumnWidth(0,h->sectionSize(0));
      ui->testdataTableView->setColumnWidth(1,h->sectionSize(1));
      ui->testdataTableView->resize(
                  h->sectionSize(0)+h->sectionSize(1)+1,
                  8*ui->testdataTableView->rowHeight(0)+1+
                  h->height());
      QSize ts_post = ui->testdataTableView->size();
      int dx = ts_post.width()-ts_pre.width();
      int dy = ts_post.height()-ts_pre.height();
      this->resize(this->width()+dx,this->height()+dy);
  });

  // All measurements received: enable write test results
  //
  connect(&m_manager, &AudiometerManager::canWrite,
          this,[this](){
      ui->statusBar->showMessage("Ready to write...");
      ui->saveButton->setEnabled(true);
  });

  // Write test data to output
  //
  connect(ui->saveButton, &QPushButton::clicked,
    this, &MainWindow::writeOutput);

  // Close the application
  //
  connect(ui->closeButton, &QPushButton::clicked,
          this, &MainWindow::close);

  emit m_manager.dataChanged();
}

void MainWindow::updateDeviceList(const QString &label)
{
    // Add the device to the list
    //
    int index = ui->deviceComboBox->findText(label);
    bool oldState = ui->deviceComboBox->blockSignals(true);
    if(-1 == index)
    {
        ui->deviceComboBox->addItem(label);
    }
    ui->deviceComboBox->blockSignals(oldState);
}

void MainWindow::run()
{
    m_manager.start();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(m_verbose)
        qDebug() << "close event called";
    QDir dir = QCoreApplication::applicationDirPath();
    QSettings settings(dir.filePath(m_manager.getGroup() + ".ini"), QSettings::IniFormat);
    m_manager.saveSettings(&settings);
    m_manager.finish();
    event->accept();
}

void MainWindow::readInput()
{
    // TODO: if the run mode is not debug, an input file name is mandatory, throw an error
    //
    if(m_inputFileName.isEmpty())
    {
        qDebug() << "no input file";
        return;
    }
    QFileInfo info(m_inputFileName);
    if(info.exists())
    {
      QFile file;
      file.setFileName(m_inputFileName);
      file.open(QIODevice::ReadOnly | QIODevice::Text);
      QString val = file.readAll();
      file.close();
      qDebug() << val;

      QJsonDocument jsonDoc = QJsonDocument::fromJson(val.toUtf8());
      QJsonObject jsonObj = jsonDoc.object();
      QMapIterator<QString,QVariant> it(m_inputData);
      QList<QString> keys = jsonObj.keys();
      for(int i=0;i<keys.size();i++)
      {
          QJsonValue v = jsonObj.value(keys[i]);
          // TODO: error report all missing expected key values
          //
          if(!v.isUndefined())
          {
              m_inputData[keys[i]] = v.toVariant();
              qDebug() << keys[i] << v.toVariant();
          }
      }
    }
    else
        qDebug() << m_inputFileName << " file does not exist";
}

void MainWindow::writeOutput()
{
   if(m_verbose)
       qDebug() << "begin write process ... ";

   QJsonObject jsonObj = m_manager.toJsonObject();

   QString barcode = ui->barcodeLineEdit->text().simplified().remove(" ");
   jsonObj.insert("barcode",QJsonValue(barcode));

   if(m_verbose)
       qDebug() << "determine file output name ... ";

   QString fileName;

   // Use the output filename if it has a valid path
   // If the path is invalid, use the directory where the application exe resides
   // If the output filename is empty default output .json file is of the form
   // <participant ID>_<now>_<devicename>.json
   //
   bool constructDefault = false;

   // TODO: if the run mode is not debug, an output file name is mandatory, throw an error
   //
   if(m_outputFileName.isEmpty())
       constructDefault = true;
   else
   {
     QFileInfo info(m_outputFileName);
     QDir dir = info.absoluteDir();
     if(dir.exists())
       fileName = m_outputFileName;
     else
       constructDefault = true;
   }
   if(constructDefault)
   {
       QDir dir = QCoreApplication::applicationDirPath();
       if(m_outputFileName.isEmpty())
       {
         QStringList list;
         list
           << barcode
           << QDate().currentDate().toString("yyyyMMdd")
           << m_manager.getGroup()
           << "test.json";
         fileName = dir.filePath( list.join("_") );
       }
       else
         fileName = dir.filePath( m_outputFileName );
   }

   QFile saveFile( fileName );
   saveFile.open(QIODevice::WriteOnly);
   saveFile.write(QJsonDocument(jsonObj).toJson());

   if(m_verbose)
       qDebug() << "wrote to file " << fileName;

   ui->statusBar->showMessage("Audiometer data recorded.  Close when ready.");
}
