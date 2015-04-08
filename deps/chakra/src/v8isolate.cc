#include "v8.h"
#include "v8-profiler.h"
#include "jsrt.h"
#include <vector>
#include <algorithm>

namespace v8
{
  Isolate dummy;
  HeapProfiler dummyHeapProfiler;
  CpuProfiler dummyCpuProfiler;

  Isolate* Isolate::New()
  {
    return GetCurrent();
  }

  Isolate *Isolate::GetCurrent() 
  { 
    // CONSIDER: Isolates are only used for debugging
    return &dummy; 
  }

  void Isolate::SetAbortOnUncaughtException(abort_on_uncaught_exception_t callback)
  {
    // TODO
  }

  void Isolate::Enter() 
  {
    // CONSIDER: Isolates are only used for debugging
  }

  void Isolate::Exit() 
  {
    // CONSIDER: Isolates are only used for debugging
  }

  void Isolate::Dispose()
  {
    // TODO
  }

  int64_t Isolate::AdjustAmountOfExternalAllocatedMemory(int64_t change_in_bytes)
  {
    // TODO: We don't support adding external memory pressure at the moment.
    return 0;
  }

  static void* s_dummyData[4];

  void Isolate::SetData(uint32_t slot, void* data)
  {
    if (slot < _countof(s_dummyData))
    {
      s_dummyData[slot] = data;
    }
  }

  void* Isolate::GetData(uint32_t slot)
  {
    return slot < _countof(s_dummyData)? s_dummyData[slot] : nullptr;
  }

  uint32_t Isolate::GetNumberOfDataSlots()
  {
    return 0;
  }

  Local<Context> Isolate::GetCurrentContext()
  {
    return Context::GetCurrent();
  }

  void Isolate::RunMicrotasks()
  {
  }

  void Isolate::SetAutorunMicrotasks(bool autorun)
  {
  }

  Local<Value> Isolate::ThrowException(Local<Value> exception)
  {
    JsSetException(*exception);
    return Undefined(this);
  }

  HeapProfiler* Isolate::GetHeapProfiler()
  {
    return &dummyHeapProfiler;
  }

  CpuProfiler* Isolate::GetCpuProfiler()
  {
    return &dummyCpuProfiler;
  }

  void Isolate::AddGCPrologueCallback(GCPrologueCallback callback, GCType gc_type_filter)
  {
  }

  void Isolate::RemoveGCPrologueCallback(GCPrologueCallback callback)
  {
  }

  void Isolate::AddGCEpilogueCallback(GCEpilogueCallback callback, GCType gc_type_filter)
  {
  }

  void Isolate::RemoveGCEpilogueCallback(GCEpilogueCallback callback)
  {
  }

  static std::vector<MessageCallback> s_messageListeners;

  bool Isolate::InternalAddMessageListener(MessageCallback that)
  {
    try
    {
      s_messageListeners.push_back(that);
      return true;
    }
    catch (...)
    {
      return false;
    }
  }

  void Isolate::InternalRemoveMessageListeners(MessageCallback that)
  {
    auto i = std::remove(s_messageListeners.begin(), s_messageListeners.end(), that);
    s_messageListeners.erase(i, s_messageListeners.end());
  }

  void Isolate::InternalInvokeMessageListeners(Handle<Message> message, Handle<Value> error)
  {
    for (auto i = s_messageListeners.begin(); i != s_messageListeners.end(); i++)
    {
      (*i)(message, error);
    }
  }
}
