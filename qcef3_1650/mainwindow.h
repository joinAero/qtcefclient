#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "cefclient/qcefwebview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
  ~MainWindow();

 protected:
  virtual void closeEvent(QCloseEvent*);

 private slots:
  void onTitleChanged(const QString& title);
  void onUrlChanged(const QUrl& url);
  void onLoadStarted();
  void onLoadFinished(bool ok);
  void onNavStateChanged(bool canGoBack, bool canGoForward);
  void onJsMessage(const QString& name, const QVariantList& args);

  void on_backButton_clicked();
  void on_forwardButton_clicked();
  void on_reloadButton_clicked();
  void on_stopButton_clicked();

  void on_lineEdit_returnPressed();

  void on_actionExit_triggered();
  void on_actionAbout_triggered();
  void on_actionSendMessage_triggered();
  void on_actionLoadHtml_triggered();

 private:
  void SetupUi();

  Ui::MainWindow *ui_;
  QCefWebView* webview_;

  Q_DISABLE_COPY(MainWindow)
};

#endif // MAINWINDOW_H
