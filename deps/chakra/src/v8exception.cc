#include "v8.h"

namespace v8
{
  Local<Value> Exception::RangeError(Handle<String> message) 
  {
    JsValueRef value;
    if (JsCreateRangeError(*message, &value) != JsNoError)
    {
      return Local<Value>();
    }
    return Local<Value>::New((Value *) value);
  }

  Local<Value> Exception::TypeError(Handle<String> message)
  {
    JsValueRef value;
    if (JsCreateTypeError(*message, &value) != JsNoError)
    {
      return Local<Value>();
    }
    return Local<Value>::New((Value *) value);
  }

  Local<Value> Exception::Error(Handle<String> message) 
  {
    JsValueRef value;
    if (JsCreateError(*message, &value) != JsNoError)
    {
      return Local<Value>();
    }
    return Local<Value>::New((Value *) value);
  }
}
