#include "v8.h"
#include "v8-debug.h"
#include "jsrtutils.h"

namespace v8
{
  __declspec(thread) bool g_EnableDebug = false;

  bool Debug::EnableAgent(const char *name, int port, bool wait_for_connection)
  {
    HRESULT hr = S_OK;

    if (!g_EnableDebug)
    {
      // JsStartDebugging needs COM initialization
      IfComFailError(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

      g_EnableDebug = true;

      Local<Context> currentContext = Context::GetCurrent();
      if (!currentContext.IsEmpty())
      {
        // Turn on debug mode on current Context (script engine), which was
        // created before start debugging and not in debug mode.
        JsStartDebugging();
      }
    }

  error:
    return SUCCEEDED(hr);
  }

  bool Debug::IsAgentEnabled()
  {
    return g_EnableDebug;
  }

  void Debug::Dispose()
  {
    if (g_EnableDebug)
    {
      CoUninitialize();
    }
  }
}
