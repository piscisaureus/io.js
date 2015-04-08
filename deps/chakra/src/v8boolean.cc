#include "v8.h"
#include "jsrt.h"

namespace v8
{
  bool Boolean::Value() const 
  { 
    return BooleanValue();
  }

  Handle<Boolean> Boolean::New(Isolate* isolate, bool value)
  { 
    if (value)
    {
      return True();
    }
    else
    {
      return False();
    }
  }
}
