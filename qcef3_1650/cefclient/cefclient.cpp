#include "cefclient/cefclient.h"
#include <string.h>
#include "include/cef_app.h"
#include "include/cef_client.h"
#include "cefclient/client_app.h"

#include <QDebug>

// Whether to use a separate sub-process executable? cefclient_process.exe
//#define SUB_PROCESS_DISABLED

namespace {

// Initialize the CEF settings.
void CefInitSettings(CefSettings& settings) {
  // Make browser process message loop run in a separate thread.
  settings.multi_threaded_message_loop = true;
  // Store cache data will on disk.
  std::string cache_path = AppGetWorkingDirectory().toStdString() + "/.cache";
  CefString(&settings.cache_path) = CefString(cache_path);
  // Completely disable logging.
  settings.log_severity = LOGSEVERITY_DISABLE;
  // The resources(cef.pak and/or devtools_resources.pak) directory.
  CefString(&settings.resources_dir_path) = CefString();
  // The locales directory.
  CefString(&settings.locales_dir_path) = CefString();
  // Enable remote debugging on the specified port.
  settings.remote_debugging_port = 8088;
  // Ignore errors related to invalid SSL certificates.
  //settings.ignore_certificate_errors = true;
}

}  // namespace

CefRefPtr<ClientHandler> g_handler;

int CefInit(int &argc, char **argv) {
  qDebug() << __FUNCTION__;
  HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);

  CefMainArgs main_args(hInstance);
  CefRefPtr<ClientApp> app(new ClientApp);

#ifdef SUB_PROCESS_DISABLED
  // Execute the secondary process, if any.
  int exit_code = CefExecuteProcess(main_args, app.get());
  if (exit_code >= 0)
    return exit_code;
#endif

  CefSettings settings;
  CefInitSettings(settings);
  
#ifndef SUB_PROCESS_DISABLED
  // Specify the path for the sub-process executable.
  CefString(&settings.browser_subprocess_path).FromASCII("cefclient_process.exe");
#endif

  // Initialize CEF.
  CefInitialize(main_args, settings, app.get());

  g_handler = new ClientHandler();

  return -1;
}

void CefLoadPlugins(bool isWow64) {
  // Adobe Flash Player plug-in:
  // https://support.google.com/chrome/answer/108086
  // How to load chrome flash plugin:
  // https://code.google.com/p/chromiumembedded/issues/detail?id=130

  // Load flash system plug-in on Windows.
  CefString flash_plugin_dir = isWow64 ? "C:\\Windows\\SysWOW64\\Macromed\\Flash"
                                       : "C:\\Windows\\System32\\Macromed\\Flash";
  CefAddWebPluginDirectory(flash_plugin_dir);

  CefRefreshWebPlugins();
}

void CefQuit() {
  qDebug() << __FUNCTION__;
  // Shut down CEF.
  CefShutdown();
}
