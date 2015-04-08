#include "v8.h"
#include "jsrt.h"
#include "jsrtutils.h"

using namespace jsrt;

namespace v8
{
  uint32_t Array::Length() const
  {
    unsigned int length;
    GetArrayLength((JsValueRef)this, &length);
    return length;
  }

  Local<Array> Array::New(Isolate* isolate, int length)
  {
    JsValueRef newArrayRef;

    if (JsCreateArray(length, &newArrayRef) != JsNoError)
    {
      return Local<Array>();
    }

    return Local<Array>::New((Array*)newArrayRef);
  }

  Array *Array::Cast(Value *obj)
  {
    if (!obj->IsArray())
    {
      // TODO: report error?
      return nullptr;
    }

    return static_cast<Array*>(obj);
  }
}
