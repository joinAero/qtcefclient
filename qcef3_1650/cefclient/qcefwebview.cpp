#include "qcefwebview.h"
#include "include/cef_browser.h"
#include <QCoreApplication>
#include "cefclient/message_event.h"

//#include <QDebug>
//#include <QThread>

extern CefRefPtr<ClientHandler> g_handler;

QCefWebView::QCefWebView(QWidget* parent)
  : QWidget(parent),
    browser_state_(kNone),
    need_resize_(false) {
  //qDebug() << __FUNCTION__ << QThread::currentThreadId();
  setAttribute(Qt::WA_NativeWindow);
  setAttribute(Qt::WA_DontCreateNativeAncestors);
}

QCefWebView::~QCefWebView() {
  //qDebug() << __FUNCTION__;
}

void QCefWebView::load(const QUrl& url) {
  //qDebug() << __FUNCTION__ << url;
  url_ = url;
  if (GetBrowser().get() && !url.isEmpty()) {
    CefString cefurl(url_.toString().toStdWString());
    GetBrowser()->GetMainFrame()->LoadURL(cefurl);
    return;
  }
  browser_state_ = kNone;
  CreateBrowser(size());
}

void QCefWebView::setHtml(const QString& html, const QUrl& baseUrl) {
  // Custom Scheme and Request Handling:
  // https://code.google.com/p/chromiumembedded/wiki/GeneralUsage#Request_Handling
  if (GetBrowser().get()) {
    QUrl url = baseUrl.isEmpty() ? this->url(): baseUrl;
    if (!url.isEmpty()) {
      CefRefPtr<CefFrame> frame = GetBrowser()->GetMainFrame();
      frame->LoadString(CefString(html.toStdWString()),
                        CefString(url.toString().toStdWString()));
    }
  }
}

QUrl QCefWebView::url() const {
  if (GetBrowser().get()) {
    CefString url = GetBrowser()->GetMainFrame()->GetURL();
    return QUrl(QString::fromStdWString(url.ToWString()));
  }
  return QUrl();
}

void QCefWebView::back() {
  CefRefPtr<CefBrowser> browser = GetBrowser();
  if (browser.get())
    browser->GoBack();
}

void QCefWebView::forward() {
  CefRefPtr<CefBrowser> browser = GetBrowser();
  if (browser.get())
    browser->GoForward();
}

void QCefWebView::reload() {
  CefRefPtr<CefBrowser> browser = GetBrowser();
  if (browser.get())
    browser->Reload();
}

void QCefWebView::stop() {
  CefRefPtr<CefBrowser> browser = GetBrowser();
  if (browser.get())
    browser->StopLoad();
}

QVariant QCefWebView::evaluateJavaScript(const QString& scriptSource) {
  if (GetBrowser().get()) {
    CefString code(scriptSource.toStdWString());
    GetBrowser()->GetMainFrame()->ExecuteJavaScript(code, "", 0);
    return true;
  }
  return false;
}

void QCefWebView::resizeEvent(QResizeEvent* e) {
  //qDebug() << __FUNCTION__ << e->size();
  // On WinXP, if you load a url immediately after constructed, you will
  // CreateBrowser() with the wrong Size(). At the same time, it calls
  // resizeEvent() to resize. However the browser has not been created now,
  // ResizeBrowser() will fail, and it won't displayed with the right size.
  // Although resize(0, 0) can fix it, the other platforms maybe
  // give you the right size and it will make CreateBrowser() later.
  switch (browser_state_) {
  case kNone:
    CreateBrowser(e->size()); break;
  case kCreating:
    need_resize_ = true; break;
  default:
    ResizeBrowser(e->size());
  }
}

void QCefWebView::closeEvent(QCloseEvent* e) {
  if (g_handler.get() && !g_handler->IsClosing()) {
    CefRefPtr<CefBrowser> browser = g_handler->GetBrowser();
    if (browser.get()) {
      browser->GetHost()->CloseBrowser(false);
    }
  }
  e->accept();
}

void QCefWebView::showEvent(QShowEvent* e) {
  CreateBrowser(size());
}

void QCefWebView::customEvent(QEvent* e) {
  //qDebug() << __FUNCTION__ << QThread::currentThreadId();
  if (e->type() == MessageEvent::MessageEventType) {
    MessageEvent* event = static_cast<MessageEvent*>(e);
    QString name = event->name();
    QVariantList args = event->args();
    //qDebug() << __FUNCTION__ << name << args;
    emit jsMessage(name, args);
  }
}

void QCefWebView::OnAddressChange(const QString& url) {
  //qDebug() << __FUNCTION__ << url;
  emit urlChanged(QUrl(url));
}

void QCefWebView::OnTitleChange(const QString& title) {
  //qDebug() << __FUNCTION__ << title;
  emit titleChanged(title);
}

void QCefWebView::SetLoading(bool isLoading) {
  //qDebug() << __FUNCTION__ << isLoading;
  if (isLoading) {
    emit loadStarted();
  } else {
    emit loadFinished(true);
  }
}

void QCefWebView::SetNavState(bool canGoBack, bool canGoForward) {
  //qDebug() << __FUNCTION__ << canGoBack << canGoForward;
  emit navStateChanged(canGoBack, canGoForward);
}

void QCefWebView::OnAfterCreated() {
  //qDebug() << __FUNCTION__;
  browser_state_ = kCreated;
  if (need_resize_) {
    ResizeBrowser(size());
    need_resize_ = false;
  }
}

void QCefWebView::OnMessageEvent(MessageEvent* e) {
  //qDebug() << __FUNCTION__ << QThread::currentThreadId();
  // Cross thread. Not in ui thread here.
  QCoreApplication::postEvent(this, e, Qt::HighEventPriority);
}

bool QCefWebView::CreateBrowser(const QSize& size) {
  if (browser_state_ != kNone || size.isEmpty()) {
    return false;
  }
  RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = size.width();
  rect.bottom = size.height();

  CefWindowInfo info;
  CefBrowserSettings settings;

  // By default, as a child window.
  info.SetAsChild(this->winId(), rect);

  g_handler->set_listener(this);

  CefString url = url_.isEmpty() ? "about:blank"
                                 : CefString(url_.toString().toStdWString());
  CefBrowserHost::CreateBrowser(info, g_handler.get(), url, settings, NULL);

  browser_state_ = kCreating;
  return true;
}

CefRefPtr<CefBrowser> QCefWebView::GetBrowser() const {
  CefRefPtr<CefBrowser> browser;
  if (g_handler.get())
    browser = g_handler->GetBrowser();
  return browser;
}

void QCefWebView::ResizeBrowser(const QSize& size) {
  if (g_handler.get() && g_handler->GetBrowser()) {
    CefWindowHandle hwnd =
        g_handler->GetBrowser()->GetHost()->GetWindowHandle();
    if (hwnd) {
      HDWP hdwp = BeginDeferWindowPos(1);
      hdwp = DeferWindowPos(hdwp, hwnd, NULL,
        0, 0, size.width(), size.height(),
        SWP_NOZORDER);
      EndDeferWindowPos(hdwp);
    }
  }
}