#include "v8.h"
#include "jsrt.h"
#include "jsrtutils.h"

using namespace jsrt;

namespace v8
{
  Local<Integer> Integer::New(Isolate* isolate, int32_t value)
  {
    JsValueRef ref;

    if (JsIntToNumber(value, &ref) != JsNoError)
    {
      return Local<Integer>(); 
    }

    return Local<Integer>::New((Integer*)ref);
  }

  Local<Integer> Integer::NewFromUnsigned(Isolate* isolate, uint32_t value)
  {
    JsValueRef ref;

    if (JsIntToNumber(value, &ref) != JsNoError)
    {
      return Local<Integer>(); 
    }

    return Local<Integer>::New((Integer*)ref);
  }

  Integer *Integer::Cast(v8::Value *obj)
  {
    // TODO: should we call convert here? or just do cast?
    if (!obj->IsInt32())
    {
      return nullptr; // TODO: report an error?
    }

    return (Integer*)obj;
  }

  int64_t Integer::Value() const
  {
    return IntegerValue();
  }
}
