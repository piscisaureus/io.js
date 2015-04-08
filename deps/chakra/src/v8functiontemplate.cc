#include "v8.h"
#include "v8chakra.h"
#include "jsrt.h"
#include "jsrtutils.h"

using namespace jsrt;

namespace v8
{
  struct FunctionTemplateData
  {
    FunctionCallback callback;
    Persistent<Value> data; 
    Persistent<Signature> signature;
    Persistent<ObjectTemplate> instanceTemplate;
    Persistent<ObjectTemplate> prototypeTemplate;
    Persistent<Object> prototype;

    FunctionTemplateData(FunctionCallback aCallback, Handle<Value> aData, Handle<Signature> aSignature) :
      callback(aCallback), 
      data(Persistent<Value>(aData)), 
      signature(Persistent<Signature>(aSignature)),
      instanceTemplate(),
      prototypeTemplate(),
      prototype()
    {
      HandleScope scope;
      instanceTemplate = Persistent<ObjectTemplate>(ObjectTemplate::New(Isolate::GetCurrent()));
      prototypeTemplate = Persistent<ObjectTemplate>(ObjectTemplate::New(Isolate::GetCurrent()));
    }

    static void CALLBACK FinalizeCallback(_In_opt_ void *data)
    {
      if (data != nullptr)
      {
        FunctionTemplateData* templateData = reinterpret_cast<FunctionTemplateData*>(data);
        templateData->data.Dispose();
        templateData->signature.Dispose();
        templateData->instanceTemplate.Dispose();
        templateData->prototypeTemplate.Dispose();
        templateData->prototype.Dispose();
        delete templateData;
      }
    }

    bool CheckSignature(Object* thisPointer, JsValueRef *arguments, unsigned short argumentCount)
    {
      if (signature.IsEmpty())
      {
        return true;
      }
      Local<FunctionTemplate> receiver = reinterpret_cast<FunctionTemplate*>(*signature);

      // REVIEW: Current FunctionTemplate::HasInstance check seems too weak. One might manipulate
      // __proto__ and pass that check. Should HasInstance validate the instance is created through
      // that FunctionTemplate?
      return receiver->HasInstance(thisPointer);
    }

    static JsValueRef CALLBACK FunctionInvoked(_In_ JsValueRef callee, _In_ bool isConstructCall, _In_ JsValueRef *arguments, _In_ unsigned short argumentCount, void *callbackState)
    {
      HandleScope scope;

      void *externalData;
      if (GetExternalData(callee, &externalData) != JsNoError)
      {
        return JS_INVALID_REFERENCE;
      }

      FunctionTemplateData *templateData = reinterpret_cast<FunctionTemplateData*>(externalData);

      Local<Object> thisPointer;
      ++arguments; // skip the this argument

      if (isConstructCall)
      {
        thisPointer = templateData->instanceTemplate->NewInstance(templateData->prototype);
        if (thisPointer.IsEmpty())
        {
          return JS_INVALID_REFERENCE;
        }
      }
      else
      {
        thisPointer = Local<Object>::New((Object*)arguments[-1]);
      }

      if (templateData->callback != nullptr)
      {
        if (!templateData->CheckSignature(*thisPointer, arguments, argumentCount))
        {
          wchar_t txt[] = L"Illegal invocation";
          JsValueRef msg, err;
          if (JsPointerToString(txt, _countof(txt) - 1, &msg) == JsNoError
            && JsCreateTypeError(msg, &err) == JsNoError)
          {
            JsSetException(err);
          }

          return JS_INVALID_REFERENCE;
        }

        FunctionCallbackInfo<Value> args((Value**)arguments, argumentCount - 1, thisPointer, isConstructCall, Local<Function>::New((Function*)callee), templateData->data);

        templateData->callback(args);
        Handle<Value> result = args.GetReturnValue().Get();

        // if this is a regualr function call return the result, otherwise this is a constructor call return the new instance
        if (!isConstructCall)
        {
          return *result; 
        }
        else if (!result.IsEmpty())
        {
          if (!result->Equals(Undefined()) && !result->Equals(Null()))
          {
            return *result;
          }
        }
      }

      // no callback is attach just return the new instance
      return *thisPointer;
    }
  };

  Local<FunctionTemplate> FunctionTemplate::New(
    Isolate* isolate,
    FunctionCallback callback,
    v8::Handle<Value> data,
    v8::Handle<Signature> signature,
    int length)
  {
    FunctionTemplateData* templateData = new FunctionTemplateData(callback, data, signature);
    JsValueRef function;

    if (CreateFunctionWithExternalData(FunctionTemplateData::FunctionInvoked, templateData, FunctionTemplateData::FinalizeCallback, &function) != JsNoError)
    {
      delete templateData;
      return Local<FunctionTemplate>();
    }

    return Local<FunctionTemplate>::New((FunctionTemplate*)function);
  }

  Local<Function> FunctionTemplate::GetFunction() 
  { 
    void *externalData;
    if (GetExternalData(this, &externalData) != JsNoError)
    {
      return Local<Function>();
    }

    FunctionTemplateData *functionTemplateData = reinterpret_cast<FunctionTemplateData*>(externalData);

    if (functionTemplateData->prototype.IsEmpty())
    {
      functionTemplateData->prototype = Persistent<Object>(functionTemplateData->prototypeTemplate->NewInstance());

      if (functionTemplateData->prototype.IsEmpty())
      {
        return Local<Function>();
      }

      ((Function *)this)->Set(String::New(L"prototype"), functionTemplateData->prototype);
    }

    return Local<Function>::New((Function *)this);
  }

  Local<ObjectTemplate> FunctionTemplate::InstanceTemplate() 
  {
    void *externalData;
    if (GetExternalData(this, &externalData) != JsNoError)
    {
      return Local<ObjectTemplate>();
    }

    FunctionTemplateData *functionTemplateData = reinterpret_cast<FunctionTemplateData*>(externalData);

    return Local<ObjectTemplate>::New(*functionTemplateData->instanceTemplate);
  }

  Local<ObjectTemplate> FunctionTemplate::PrototypeTemplate()
  { 
    void *externalData;
    if (GetExternalData(this, &externalData) != JsNoError)
    {
      return Local<ObjectTemplate>();
    }

    FunctionTemplateData *functionTemplateData = reinterpret_cast<FunctionTemplateData*>(externalData);

    if (functionTemplateData->prototypeTemplate.IsEmpty())
    {
      functionTemplateData->prototypeTemplate = Persistent<ObjectTemplate>(ObjectTemplate::New(Isolate::GetCurrent()));
    }

    // The V8 specs are silent on what's supposed to happen here if the function
    // has been created. If you try and modify the prototype template, what's supposed
    // to happen given that the prototype object must have already been created?

    return Local<ObjectTemplate>::New(*functionTemplateData->prototypeTemplate);
  }

  void FunctionTemplate::Set(Handle<String> name, Handle<Data> value, PropertyAttribute attributes)
  {
    void *externalData;
    if (GetExternalData(this, &externalData) != JsNoError)
    {
      return;
    }

    FunctionTemplateData *functionTemplateData = reinterpret_cast<FunctionTemplateData*>(externalData);

    ((Function *)this)->Set(name, value, attributes);
  }

  void FunctionTemplate::SetClassName(Handle<String> name)
  {
    void *externalData;
    if (GetExternalData(this, &externalData) != JsNoError)
    {
      return;
    }

    FunctionTemplateData *functionTemplateData = reinterpret_cast<FunctionTemplateData*>(externalData);
    functionTemplateData->instanceTemplate->SetClassName(name);
  }

  void FunctionTemplate::SetHiddenPrototype(bool value)
  {
    //TODO
  }

  bool FunctionTemplate::HasInstance(Handle<Value> object) 
  { 
    void *externalData;
    if (GetExternalData(this, &externalData) != JsNoError)
    {
      return false;
    }

    FunctionTemplateData *functionTemplateData = reinterpret_cast<FunctionTemplateData*>(externalData);

    JsValueRef prototype;
    if (JsGetPrototype(*object, &prototype) != JsNoError)
    {
      return false;
    }

    return *functionTemplateData->prototype == prototype;
  }
}
