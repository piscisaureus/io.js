#include "v8.h"
#include "jsrt.h"
#include "jsrtutils.h"

using namespace jsrt;

namespace v8
{
  const wchar_t * EXTERNAL_PROP_NAME = L"__isexternal__";

  Local<Value> External::Wrap(void* data)
  {
    return External::New(Isolate::GetCurrent(), data);
  }

  inline void* External::Unwrap(Handle<v8::Value> obj)
  {
    if (!obj->IsExternal())
    {
      return nullptr;
    }

    return obj.As<External>()->Value();
  }

  Local<External> External::New(Isolate* isolate, void* value)
  {
    JsValueRef externalRef;

    if (JsCreateExternalObject(value, nullptr, &externalRef) != JsNoError)
    {
      return Local<External>();
    }

    JsValueRef trueRef;
    if (JsGetTrueValue(&trueRef) != JsNoError)
    {
      return Local<External>();
    }

    if (jsrt::DefineProperty(externalRef, 
      EXTERNAL_PROP_NAME, 
      jsrt::PropertyDescriptorOptionValues::False, 
      jsrt::PropertyDescriptorOptionValues::False, 
      jsrt::PropertyDescriptorOptionValues::False,
      trueRef,
      JS_INVALID_REFERENCE,
      JS_INVALID_REFERENCE) != JsNoError)
    {
      return Local<External>();
    }

    return Local<External>::New((External*)externalRef); 
  }

  bool External::IsExternal(const v8::Value* value)
  {
    if (!value->IsObject())
    {
      return false;
    }

    JsPropertyIdRef propIdRef;
    if (JsGetPropertyIdFromName(EXTERNAL_PROP_NAME, &propIdRef) != JsNoError)
    {
      return false;
    }

    bool hasProp;
    if (JsHasProperty((JsValueRef)value, propIdRef, &hasProp) != JsNoError)
    {
      return false;
    }

    return hasProp;
  }

  External* External::Cast(v8::Value* obj)
  {
    if (!obj->IsExternal())
    {
      return nullptr; 
    }

    return static_cast<External*>(obj);
  }

  void* External::Value() const
  {
    void* data;
    if (JsGetExternalData((JsValueRef)this, &data) != JsNoError)
    {
      return nullptr;
    }

    return data;
  }
}
