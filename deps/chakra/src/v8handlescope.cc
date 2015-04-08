#include "v8.h"

namespace v8
{
  __declspec(thread) HandleScope *current = nullptr;

  HandleScope::HandleScope(Isolate* isolate) :
    _prev(current),
    _refs(JS_INVALID_REFERENCE),
    _count(0)
  {
    current = this;
  }

  HandleScope::~HandleScope()
  {
    current = _prev;
  }

  HandleScope *HandleScope::GetCurrent()
  {
    return current;
  }
}
