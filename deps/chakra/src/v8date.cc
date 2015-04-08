#include "v8.h"
#include "jsrt.h"
#include "jsrtutils.h"

using namespace jsrt;

namespace v8
{
  __declspec(thread) JsValueRef dateTypeRef = JS_INVALID_REFERENCE;

  Local<Value> Date::New(double time) 
  { 
    JsValueRef newDateRef;
    JsValueRef numberRef;

    if (dateTypeRef == JS_INVALID_REFERENCE)
    {
      if (GetGlobalType(L"Date",&dateTypeRef) != JsNoError)
      {
        return Local<Value>();
      }
    }

    if (JsDoubleToNumber(time, &numberRef) != JsNoError)
    {
      return Local<Value>();
    }

    JsValueRef args[] = { nullptr, numberRef};

    if (JsConstructObject(dateTypeRef, args, _countof(args), &newDateRef) != JsNoError)
    {
      return Local<Value>();
    }

    return Local<Date>::New((Date*)newDateRef);
  }

  Local<Value> Date::New() 
  { 
    JsValueRef newDateRef;

    if (dateTypeRef == JS_INVALID_REFERENCE)
    {
      if (GetGlobalType(L"Date",&dateTypeRef) != JsNoError)
      {
        return Local<Value>();
      }
    }

    JsValueRef args[] = { nullptr };

    if (JsConstructObject(dateTypeRef, args,1, &newDateRef) != JsNoError)
    {
      return Local<Value>();
    }

    return Local<Date>::New((Date*)newDateRef);
  }

  // Not Implemented
  Date *Date::Cast(v8::Value *obj) 
  { 
    if (!obj->IsDate())
    {
      // TODO: what should we return in this case?
      // just exit and print?
      return nullptr;
    }

    return static_cast<Date*>(obj);
  }
}
