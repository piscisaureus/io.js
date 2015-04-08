#include "v8.h"

namespace v8
{
  Local<Signature> Signature::New(Isolate* isolate, Handle<FunctionTemplate> receiver)
  {
    // TODO: Only support "receiver" for now. V8 also supports arg list check.
    return (Signature*)*receiver;
  }

  Local<AccessorSignature> AccessorSignature::New(Isolate* isolate, Handle<FunctionTemplate> receiver)
  {
    // TODO. Ignore as well.
    return Local<AccessorSignature>();
  }
}
