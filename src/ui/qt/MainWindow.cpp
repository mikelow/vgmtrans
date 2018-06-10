/*
 * VGMTrans (c) 2018
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include <QFileInfo>
#include <QMimeData>
#include <QVBoxLayout>
#include "QtVGMRoot.h"
#include "MainWindow.h"
#include "HeaderContainer.h"

#include "VGMFile.h"

MainWindow::MainWindow() : QMainWindow(nullptr) {
  setWindowTitle("VGMTrans");
  setUnifiedTitleAndToolBarOnMac(true);
  setAcceptDrops(true);
  setContextMenuPolicy(Qt::NoContextMenu);

  CreateElements();
  RouteSignals();

  ui_statusbar->showMessage("Ready", 3000);
}

void MainWindow::CreateElements() {
  ui_menu_bar = new MenuBar(this);
  setMenuBar(ui_menu_bar);
  ui_iconbar = new IconBar(this);
  addToolBar(ui_iconbar);

  ui_statusbar = new QStatusBar(this);
  ui_statusbar_offset = new QLabel();
  ui_statusbar_length = new QLabel();
  ui_statusbar->addPermanentWidget(ui_statusbar_offset);
  ui_statusbar->addPermanentWidget(ui_statusbar_length);
  setStatusBar(ui_statusbar);

  ui_rawfiles_list = new RawFileListView();
  HeaderContainer* ui_rawfiles_list_container =
      new HeaderContainer(ui_rawfiles_list, QStringLiteral("Imported Files"));

  ui_vgmfiles_list = new VGMFileListView();
  HeaderContainer* ui_vgmfiles_list_container =
      new HeaderContainer(ui_vgmfiles_list, QStringLiteral("Detected files"));

  ui_colls_list = new VGMCollListView();
  HeaderContainer* ui_colls_list_container =
      new HeaderContainer(ui_colls_list, QStringLiteral("Detected collections"));

  ui_tabs_area = new MdiArea(this);

  ui_logger = new Logger(this);
  addDockWidget(Qt::BottomDockWidgetArea, ui_logger);

  vertical_splitter = new QSplitter(Qt::Vertical, this);
  horizontal_splitter = new QSplitter(Qt::Horizontal, vertical_splitter);
  vertical_splitter_left = new QSplitter(Qt::Vertical, horizontal_splitter);

  vertical_splitter->addWidget(ui_colls_list_container);
  vertical_splitter->setHandleWidth(1);

  horizontal_splitter->addWidget(ui_tabs_area);
  horizontal_splitter->setHandleWidth(1);

  vertical_splitter_left->addWidget(ui_rawfiles_list_container);
  vertical_splitter_left->addWidget(ui_vgmfiles_list_container);
  vertical_splitter_left->setHandleWidth(1);

  setCentralWidget(vertical_splitter);
}

void MainWindow::RouteSignals() {
  connect(&qtVGMRoot, &QtVGMRoot::UI_AddLogItem, ui_logger, &Logger::LogMessage);
  connect(&qtVGMRoot, &QtVGMRoot::UI_RemoveVGMFile, ui_vgmfiles_list,
          &VGMFileListView::RemoveVGMFileItem);

  connect(ui_menu_bar, &MenuBar::OpenFile, this, &MainWindow::OpenFile);
  connect(ui_menu_bar, &MenuBar::Exit, this, &MainWindow::close);
  connect(ui_iconbar, &IconBar::OpenPressed, this, &MainWindow::OpenFile);

  connect(ui_vgmfiles_list, &VGMFileListView::AddMdiTab, ui_tabs_area, &MdiArea::addSubWindow);
  connect(ui_vgmfiles_list, &VGMFileListView::RemoveMdiTab, ui_tabs_area, &MdiArea::RemoveTab);

  connect(ui_menu_bar, &MenuBar::LoggerToggled,
          [=] { ui_logger->setHidden(!ui_menu_bar->IsLoggerToggled()); });
  connect(ui_logger, &Logger::closeEvent, [=] { ui_menu_bar->SetLoggerHidden(); });

  connect(ui_vgmfiles_list, &VGMCollListView::clicked, [=] {
    if (!ui_vgmfiles_list->currentIndex().isValid())
      return;

    VGMFile* clicked_item = qtVGMRoot.vVGMFile[ui_vgmfiles_list->currentIndex().row()];
    ui_statusbar_offset->setText("Offset: 0x" +
                                 QString::number(clicked_item->dwOffset, 16).toUpper());
    ui_statusbar_length->setText("Length: 0x" + QString::number(clicked_item->size(), 16).toUpper());
  });
}

void MainWindow::OpenFile() {
  auto filename = qtVGMRoot.UI_GetOpenFilePath();

  if (!filename.empty())
    qtVGMRoot.OpenRawFile(filename);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
  event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* event) {
  const auto& files = event->mimeData()->urls();

  if (files.isEmpty())
    return;

  for (const auto& file : files) {
    // Leave sanity checks to the backend
    qtVGMRoot.OpenRawFile(QFileInfo(file.toLocalFile()).filePath().toStdWString());
  }
}