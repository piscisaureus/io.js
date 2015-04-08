#include "v8.h"

namespace v8 
{
  Local<Value> Arguments::operator [](int i) const 
  { 
    if (i >= 0 && i < _length)
    {
      return Local<Value>(_args[i]);
    }

    return Local<Value>((Value*)*Undefined());
  }

  Arguments::Arguments(Value** args, int length, Local<Object> _this, bool isConstructorCall, Local<Function> callee)
    : _args(args), _length(length), _thisPointer(_this), _isConstructorCall(isConstructorCall), _callee(callee)
  {
  }
}
