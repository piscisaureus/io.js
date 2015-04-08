#include "v8.h"

namespace v8
{
  Local<String> Message::GetSourceLine() const 
  { 
    // TODO: Figure out how to transmit this info...?
    return Local<String>(); 
  }

  Handle<Value> Message::GetScriptResourceName() const 
  { 
    // TODO: Figure out how to transmit this info...?
    return Handle<Value>();
  }

  int Message::GetLineNumber() const 
  { 
    // TODO: Figure out how to transmit this info...?
    return 0;
  }

  int Message::GetStartColumn() const 
  { 
    // TODO: Figure out how to transmit this info...?
    return 0;
  }

  int Message::GetEndColumn() const 
  { 
    // TODO: Figure out how to transmit this info...?
    return 0;
  }
}
