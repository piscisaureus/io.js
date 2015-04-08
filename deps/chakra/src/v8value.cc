#include "v8.h"
#include "jsrt.h"
#include <math.h>
#include "jsrtUtils.h"

using namespace jsrt;

namespace v8
{
  bool Value::IsUndefined() const
  {
    JsValueType type;
    if(JsGetValueType((JsValueRef)this, &type) != JsNoError)
    {
      return false;
    }

    return (type == JsValueType::JsUndefined);
  }

  bool Value::IsNull() const
  {
    JsValueType type;
    if(JsGetValueType((JsValueRef)this, &type) != JsNoError)
    {
      return false;
    }

    return (type == JsValueType::JsNull);
  }

  bool Value::IsTrue() const
  {
    bool isTrue;
    if (JsEquals(*True(), (JsValueRef)this, &isTrue) != JsNoError)
    {
      return false;
    }

    return isTrue;
  }

  bool Value::IsFalse() const
  {
    bool isFalse;
    if (JsEquals(*False(), (JsValueRef)this, &isFalse) != JsNoError)
    {
      return false;
    }

    return isFalse;
  }

  bool Value::IsString() const
  {
    JsValueType type;
    if(JsGetValueType((JsValueRef)this, &type) != JsNoError)
    {
      return false;
    }

    // TODO: add a check of instance of...
    /* JsValueRef stringType;
    GetGlobalType(L"String", &stringType);
    bool isInstanceOfString;
    InstanceOf(convertedObject, stringType, &isInstanceOfString);*/

    return (type == JsValueType::JsString);
  }

  bool Value::IsFunction() const
  {
    JsValueType type;
    if(JsGetValueType((JsValueRef)this, &type) != JsNoError)
    {
      return false;
    }

    return (type == JsValueType::JsFunction);
  }

  bool Value::IsArray() const
  {
    JsValueType type;
    if(JsGetValueType((JsValueRef)this, &type) != JsNoError)
    {
      return false;
    }

    return (type == JsValueType::JsArray);
  }

  bool Value::IsObject() const
  {
    JsValueType type;
    if(JsGetValueType((JsValueRef)this, &type) != JsNoError)
    {
      return false;
    }

    return (type == JsValueType::JsObject || type == JsValueType::JsFunction || type == JsValueType::JsError);
  }

  bool Value::IsExternal() const
  {
    return External::IsExternal(this);
  }

  bool Value::IsTypedArray() const
  {
    bool result;
    return (IsOfGlobalType((JsValueRef)this, L"Uint8Array", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"Uint8ClampedArray", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"Int8Array", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"Uint16Array", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"Int16Array", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"Uint32Array", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"Int32Array", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"Float32Array", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"Float64Array", &result) == JsNoError && result);
  }

  bool Value::IsBoolean() const
  {
    JsValueType type;
    if(JsGetValueType((JsValueRef)this, &type) != JsNoError)
    {
      return false;
    }

    return (type == JsValueType::JsBoolean);
  }

  bool Value::IsNumber() const
  {
    JsValueType type;
    if(JsGetValueType((JsValueRef)this, &type) != JsNoError)
    {
      return false;
    }

    return (type == JsValueType::JsNumber);
  }

  bool Value::IsInt32() const
  {
    if (!IsNumber())
    {
      return false;
    }

    double value = NumberValue();

    // check that the value is smaller than int 32 bit maximum
    if (value > INT_MAX || value < INT_MIN)
    {
      return false;
    }

    double second;

    return (modf(value, &second) == 0.0);
  }

  bool Value::IsUint32() const
  {
    if (!IsNumber())
    {
      return false;
    }

    double value = NumberValue();
    // check that the value is smaller than 32 bit maximum
    if (value > UINT_MAX)
    {
      return false;
    }


    double second;
    // TODO: nadavbar: replace this with trunc. Not used for since for some reason my math.h file does not contain it
    // Probably a version problem
    return (modf(value, &second) == 0.0 && value >= 0.0);
  }

  bool Value::IsDate() const
  {
    bool result;
    if (IsOfGlobalType((JsValueRef)this, L"Date", &result) != JsNoError)
    {
      return false;
    }

    return result;
  }

  bool Value::IsNativeError() const
  {
    bool result;
    return (IsOfGlobalType((JsValueRef)this, L"Error", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"EvalError", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"RangeError", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"ReferenceError", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"SyntaxError", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"TypeError", &result) == JsNoError && result)
      || (IsOfGlobalType((JsValueRef)this, L"URIError", &result) == JsNoError && result);
  }

  bool Value::IsRegExp() const
  {
    bool result;
    if (IsOfGlobalType((JsValueRef)this, L"RegExp", &result) != JsNoError)
    {
      return false;
    }

    return result;
  }

  Local<Boolean> Value::ToBoolean() const
  {
    JsValueRef value;
    if (JsConvertValueToBoolean((JsValueRef)this,&value) != JsNoError)
    {
      return Local<Boolean>();
    }

    return Local<Boolean>::New((Boolean*)value);
  }

  Local<Number> Value::ToNumber() const
  {
    JsValueRef value;
    if (JsConvertValueToNumber((JsValueRef)this,&value) != JsNoError)
    {
      return Local<Number>();
    }

    return Local<Number>::New((Number*)value);
  }

  Local<String> Value::ToString() const
  {
    JsValueRef value;
    if (JsConvertValueToString((JsValueRef)this,&value) != JsNoError)
    {
      return Local<String>();
    }

    return Local<String>::New((String*)value);
  }

  Local<Object> Value::ToObject() const
  {
    JsValueRef value;
    if (JsConvertValueToObject((JsValueRef)this,&value) != JsNoError)
    {
      return Local<Object>();
    }

    return Local<Object>::New((Object*)value);
  }

  Local<Integer> Value::ToInteger() const
  {
    int64_t value = this->IntegerValue();

    JsValueRef integerRef;

    if (JsIntToNumber(static_cast<int>(value), &integerRef) != JsNoError)
    {
      return Local<Integer>();
    }

    return Local<Integer>::New((Integer*)integerRef);
  }


  Local<Uint32> Value::ToUint32() const
  {
    Local<Integer> jsValue = Integer::NewFromUnsigned(Isolate::GetCurrent(), this->Uint32Value());
    return Local<Uint32>((Uint32*)*jsValue);
  }

  Local<Int32> Value::ToInt32() const
  {
    Local<Integer> jsValue = Integer::New(Isolate::GetCurrent(), this->Int32Value());
    return Local<Int32>((Int32*)*jsValue);
  }

  bool Value::BooleanValue() const
  {
    JsValueRef ref;
    if (IsBoolean())
    {
      ref = (JsValueRef)this;
    }
    else
    {
      if(JsConvertValueToBoolean((JsValueRef)this, &ref) != JsNoError)
      {
        return false;
      }
    }

    bool value;

    if(JsBooleanToBool((JsValueRef)this, &value) != JsNoError)
    {
      return false;
    }

    return value;
  }

  double Value::NumberValue() const
  {
    JsValueRef ref;
    if (IsNumber())
    {
      ref = (JsValueRef)this;
    }
    else
    {
      if(JsConvertValueToNumber((JsValueRef)this, &ref) != JsNoError)
      {
        return 0;
      }
    }

    double value;

    if(JsNumberToDouble(ref, &value) != JsNoError)
    {
      return false;
    }

    return value;
  }

  int64_t Value::IntegerValue() const
  {
    return (int64_t)NumberValue();
  }

  uint32_t Value::Uint32Value() const
  {
    return (uint32_t)Int32Value();
  }

  int32_t Value::Int32Value() const
  {
    JsValueRef ref;
    if (JsConvertValueToNumber((JsValueRef)this, &ref) != JsNoError)
    {
      return 0;
    }

    int intValue;
    if (JsNumberToInt(ref, &intValue) != JsNoError)
    {
      return 0;
    }

    return intValue;
  }

  bool Value::Equals(Handle<Value> that) const
  {
    bool equals;
    if (JsEquals((JsValueRef)this,*that,&equals) != JsNoError)
    {
      return false;
    }

    return equals;
  }

  bool Value::StrictEquals(Handle<Value> that) const
  {
    bool strictEquals;
    if (JsStrictEquals((JsValueRef)this,*that,&strictEquals) != JsNoError)
    {
      return false;
    }

    return strictEquals;
  }
}
