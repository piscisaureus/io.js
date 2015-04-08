#include "v8.h"
#include "jsrt.h"

namespace v8
{
  double Number::Value() const 
  { 
    return NumberValue();
  }

  Local<Number> Number::New(Isolate* isolate, double value)
  {
    JsValueRef ref;

    if (JsDoubleToNumber(value, &ref) != JsNoError)
    {
      return Local<Number>(); 
    }

    return Local<Number>::New((Number*)ref);
  }

  Number *Number::Cast(v8::Value *obj) 
  { 
    if (!obj->IsNumber())
    {
      // TODO: report an error here!
      return nullptr; // TODO: What is the best behavior here? Should we return a pointer to undefined/null instead?
    }

    return static_cast<Number*>(obj);
  }
}
