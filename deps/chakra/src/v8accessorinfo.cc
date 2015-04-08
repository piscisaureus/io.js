#include "v8.h"

namespace v8
{
  AccessorInfo::AccessorInfo(Local<Value> data, Local<Object> thisObject) : 
    _data(data),
    _thisObject(thisObject)
  {

  }

  Local<Value> AccessorInfo::Data() const 
  { 
    return _data; 
  }

  Local<Object> AccessorInfo::This() const 
  {
    return _thisObject;
  }
}
