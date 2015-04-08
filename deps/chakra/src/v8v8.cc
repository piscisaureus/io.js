#include "v8.h"
#include "v8-debug.h"
#include "jsrt.h"
#include "jsrtutils.h"
#include <algorithm>

namespace v8
{
  __declspec(thread) JsRuntimeHandle runtime = JS_INVALID_REFERENCE;
  bool g_exposeGC = false;

  const char *V8::GetVersion()
  {
    // TODO: Fill in this version with the Chakra version info
    return "11.0.0.0";
  }

  void V8::SetFatalErrorHandler(FatalErrorCallback that)
  {
    // CONSIDER: Ignoring for now, since we don't have an equivalent concept.
  }

  bool ExposeGc()
  {
    JsValueRef collectGarbageRef;
    if (jsrt::GetPropertyOfGlobal(L"CollectGarbage",&collectGarbageRef) != JsNoError)
    {
      return false;
    }

    if (jsrt::SetPropertyOfGlobal(L"gc",collectGarbageRef) != JsNoError)
    {
      return false;
    }

    return true;
  }

  void V8::SetFlagsFromString(const char* str, int length)
  {
    //TODO
  }

  static bool equals(const char* str, const char* pat)
  {
    return strcmp(str, pat) == 0;
  }

  template <size_t N>
  static bool startsWith(const char* str, const char(&prefix)[N])
  {
    return strncmp(str, prefix, N - 1) == 0;
  }

  void V8::SetFlagsFromCommandLine(int *argc, char **argv, bool remove_flags)
  {
    for (int i = 1; i < *argc; i++)
    {
      // Note: Node now exits on invalid options. We may not recognize V8 flags and fail here, causing Node to exit.
      char *arg = argv[i];
      if (equals("--expose-gc", arg) || equals("--expose_gc", arg))
      {
        g_exposeGC = true;
        if (remove_flags)
        {
          argv[i] = nullptr;
        }
      }
      else if (remove_flags &&
        (startsWith(arg, "--debug") // Ignore some flags to reduce unit test noise
        || startsWith(arg, "--harmony")
        || startsWith(arg, "--stack-size=")
        ))
      {
        argv[i] = nullptr;
      }
    }

    if (remove_flags)
    {
      char** end = std::remove(argv + 1, argv + *argc, nullptr);
      *argc = end - argv;
    }
  }

  bool V8::Initialize()
  {
    JsErrorCode error = JsCreateRuntime((JsRuntimeAttributes) (JsRuntimeAttributeAllowScriptInterrupt), JsRuntimeVersionEdge, nullptr, &runtime);
    return error == JsNoError;
  }

  void V8::SetEntropySource(EntropySource entropy_source)
  {
    //TODO
  }

  bool V8::InitializeCurrentContext()
  {
    if (g_exposeGC)
    {
      return ExposeGc();
    }
    else
    {
      return true;
    }
  }

  void V8::SetArrayBufferAllocator(ArrayBuffer::Allocator* allocator)
  {
    // Ignore
  }

  bool V8::IsDead()
  {
    return runtime == JS_INVALID_RUNTIME_HANDLE;
  }

  bool V8::Dispose()
  {
    // nadavbar: I've switched the logig of the if statemet to return false if the dispose failed. @Paul -  is this correct?
    if (JsDisposeRuntime(runtime) != JsNoError)
    {
      return false;	
    }

    runtime = JS_INVALID_RUNTIME_HANDLE;

    Debug::Dispose();
    return true;
  }

  bool V8::AddMessageListener(MessageCallback that, Handle<Value> data)
  {
    return Isolate::GetCurrent()->InternalAddMessageListener(that);
  }


  void V8::RemoveMessageListeners(MessageCallback that)
  {
    Isolate::GetCurrent()->InternalRemoveMessageListeners(that);
  }

  void V8::SetJitCodeEventHandler(JitCodeEventOptions options, JitCodeEventHandler event_handler)
  {
    // TODO: This is for ETW events, we don't have equivalent but might not
    // need it because we do our own ETW tracing.
  }

  void V8::TerminateExecution(Isolate* isolate)
  {
    // TODO
  }

  void V8::CancelTerminateExecution(Isolate* isolate)
  {
    // TODO
  }

  void Isolate::GetHeapStatistics(HeapStatistics *heap_statistics)
  {
    size_t memoryUsage;
    if (JsGetRuntimeMemoryUsage(runtime, &memoryUsage) != JsNoError)
    {
      return;
    }
    // CONSIDER: V8 distinguishes between "total" size and "used" size
    heap_statistics->set_heap_size(memoryUsage);
  }
}
