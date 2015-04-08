#include "v8.h"
#include "jsrt.h"

namespace v8
{
  Handle<Primitive> Undefined(Isolate* isolate)
  { 
    JsValueRef undefinedValue;
    if (JsGetUndefinedValue(&undefinedValue) != JsNoError)
    {
      return Handle<Primitive>();
    }
    return Handle<Primitive>((Primitive *)undefinedValue); 
  }

  Handle<Primitive> Null(Isolate* isolate)
  {
    JsValueRef nullValue;
    if (JsGetNullValue(&nullValue) != JsNoError)
    {
      return Handle<Primitive>();
    }
    return Handle<Primitive>((Primitive *) nullValue);
  }

  Handle<Boolean> True(Isolate* isolate)
  {
    JsValueRef trueValue;
    if (JsGetTrueValue(&trueValue) != JsNoError)
    {
      return Handle<Primitive>();
    }
    return Handle<Primitive>((Primitive *) trueValue);
  }

  Handle<Boolean> False(Isolate* isolate)
  {
    JsValueRef falseValue;
    if (JsGetFalseValue(&falseValue) != JsNoError)
    {
      return Handle<Primitive>();
    }
    return Handle<Primitive>((Primitive *) falseValue);
  }

  bool SetResourceConstraints(ResourceConstraints *constraints) 
  { 
    // TODO: Ignore for now, we don't support setting the stack limit.
    return true;
  }
}
