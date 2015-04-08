#include "v8.h"
#include "jsrt.h"
#include "jsrtutils.h"

#include <memory>

using namespace jsrt;

namespace v8
{
  Local<Object> Function::NewInstance() const 
  { 
    return NewInstance(0, nullptr); 
  }

  Local<Object> Function::NewInstance(int argc, Handle<Value> argv []) const 
  { 
    std::unique_ptr<JsValueRef[]> args(new JsValueRef[argc + 1]);
    args.get()[0] = nullptr; // first argument is a null object

    if (argc > 0)
    {
      for (int i=0; i<argc; i++)
      {
        args.get()[i + 1] = *argv[i];
      }
    }

    JsValueRef newInstance;
    if (JsConstructObject((JsValueRef)this, args.get(), argc + 1, &newInstance) != JsNoError)
    {
      return Local<Object>();
    }

    return Local<Object>::New((Object*)newInstance);
  }

  Local<Value> Function::Call(Handle<Object> recv, int argc, Handle<Value> argv []) 
  {
    std::unique_ptr<JsValueRef[]> args(new JsValueRef[argc + 1]);
    args.get()[0] = *recv;

    for (int i=0; i<argc; i++)
    {
      args.get()[i + 1] = *argv[i];
    }

    JsValueRef result;
    {
      TryCatch tryCatch;
      if (JsCallFunction((JsValueRef)this, args.get(), argc + 1, &result) != JsNoError)
      {
        tryCatch.CheckReportExternalException();
        return Local<Value>();
      }
    }

    return Local<Value>::New((Value*)result);
  }

  void Function::SetName(Handle<String> name)
  {
    // TODO
  }

  Function *Function::Cast(Value *obj) 
  { 
    if (!obj->IsFunction())
    {
      // TODO: report an error here!
      return nullptr; // TODO: What is the best behavior here? Should we return a pointer to undefined/null instead?
    }

    return static_cast<Function*>(obj);
  }
}
