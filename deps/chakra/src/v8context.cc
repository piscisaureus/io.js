#include "v8.h"
#include "v8chakra.h"
#include "jsrt.h"
#include "jsrtutils.h"
#include <vector>
#include <unordered_map>

namespace v8
{
  //TODO: thread racing?
  std::unordered_map<JsContextRef, std::vector<void*>> s_contextEmbedderData;

  static void CALLBACK JsContextBeforeCollectCallback(_In_ JsRef ref, _In_opt_ void *data)
  {
    s_contextEmbedderData.erase(ref);
  }

  Context::Scope::Scope(Handle<Context> context)
  {
    // TODO: Error handling?
    JsGetCurrentContext(&previous);
    JsSetCurrentContext(*context);

    // If started debugging, ensure new Context (script engine) go to debug mode.
    if (g_EnableDebug)
    {
      JsStartDebugging();
    }

    V8::InitializeCurrentContext();
  }

  Context::Scope::~Scope()
  {
    // TODO: Error handling?
    JsSetCurrentContext(previous);
  }

  Local<Object> Context::Global()
  {
    JsValueRef global;
    if (JsGetGlobalObject(&global) != JsNoError)
    {
      return Local<Object>();
    }
    // We don't need a ref to keep the global object alive.
    return Local<Object>((Object *)global);
  }

  Local<Context> Context::New(
    Isolate* external_isolate,
    ExtensionConfiguration* extensions,
    Handle<ObjectTemplate> global_template,
    Handle<Value> global_object)
  {
    JsContextRef context;

    if (JsCreateContext(runtime, &context) != JsNoError)
    {
      return Local<Context>();
    }

    if (JsSetObjectBeforeCollectCallback(context, nullptr, JsContextBeforeCollectCallback) != JsNoError)
    {
      return Local<Context>();
    }

    return Local<Context>((Context *)context);
  }

  Local<Context> Context::GetCurrent()
  {
    JsContextRef current;
    if (JsGetCurrentContext(&current) != JsNoError)
    {
      return Local<Context>();
    }

    // Don't need to create a ref here, because a context can't keep itself alive anyway.
    return Local<Context>((Context *) current);
  }

  Isolate* Context::GetIsolate()
  {
    return Isolate::GetCurrent();
  }

  void* Context::GetAlignedPointerFromEmbedderData(int index)
  {
    auto i = s_contextEmbedderData.find((JsContextRef)this);
    if (i != s_contextEmbedderData.end()
      && index >= 0
      && static_cast<std::vector<void*>::size_type>(index) < i->second.size())
    {
      return i->second.at(index);
    }
    return nullptr;
  }

  void Context::SetAlignedPointerInEmbedderData(int index, void* value)
  {
    try
    {
      std::vector<void*>& data = s_contextEmbedderData[(JsContextRef)this];
      auto minSize = static_cast<std::vector<void*>::size_type>(index) + 1;
      if (index >= 0 && data.size() < minSize)
      {
        data.resize(minSize);
        data[index] = value;
      }
    }
    catch (const std::exception&)
    {
    }
  }

  void Context::SetSecurityToken(Handle<Value> token)
  {
    //TODO
  }

  Handle<Value> Context::GetSecurityToken()
  {
    //TODO
    return Handle<Value>();
  }
}
