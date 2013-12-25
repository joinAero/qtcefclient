#include "cefclient/client_handler.h"

#include <string>
#include <windows.h>
#include <shlobj.h> 

#include "include/cef_browser.h"
#include "include/cef_frame.h"

void ClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    const CefString& url) {
  REQUIRE_UI_THREAD();
  if (!listener_) return;

  if (m_BrowserId == browser->GetIdentifier() && frame->IsMain()) {
    listener_->OnAddressChange(QString::fromStdWString(url.ToWString()));
  }
}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title) {
  REQUIRE_UI_THREAD();
  if (!listener_) return;

  if (m_BrowserId == browser->GetIdentifier()) {
    listener_->OnTitleChange(QString::fromStdWString(title.ToWString()));
  }
}

void ClientHandler::SetLoading(bool isLoading) {
  if (!listener_) return;
  listener_->SetLoading(isLoading);
}

void ClientHandler::SetNavState(bool canGoBack, bool canGoForward) {
  if (!listener_) return;
  listener_->SetNavState(canGoBack, canGoForward);
}
