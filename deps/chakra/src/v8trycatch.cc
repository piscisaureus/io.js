
#include "v8.h"
#include "jsrtutils.h"
#include <cassert>

namespace v8
{
  __declspec(thread) TryCatch *current = nullptr;

  TryCatch::TryCatch() :
    prev(current),
    error(JS_INVALID_REFERENCE),
    rethrow(false),
    verbose(false)
  {
    current = this;
  }

  TryCatch::~TryCatch()
  {
    if (!rethrow)
    {
      GetAndClearException();
    }

    current = prev;
  }

  bool TryCatch::HasCaught() const
  {
    if (error == JS_INVALID_REFERENCE)
    {
      const_cast<TryCatch*>(this)->GetAndClearException();
    }

    if (error != JS_INVALID_REFERENCE)
    {
      return true;
    }

    bool hasException;
    if (JsHasException(&hasException) != JsNoError)
    {
      // TODO: report error
      assert(false);
      return false;
    }

    return hasException;
  }

  bool TryCatch::HasTerminated() const
  {
    //TODO
    return false;
  }

  void TryCatch::GetAndClearException()
  {
    bool hasException;
    if (JsHasException(&hasException) != JsNoError)
    {
      // TODO: report error
      assert(false);
      return;
    }

    if (hasException)
    {
      JsValueRef exceptionRef;
      if (JsGetAndClearException(&exceptionRef) != JsNoError)
      {
        // TODO: report error
        assert(false);
        return;
      }
      error = exceptionRef;
    }
  }

  Handle<Value> TryCatch::ReThrow()
  {
    if (error == JS_INVALID_REFERENCE)
    {
      GetAndClearException();
    }

    if (error == JS_INVALID_REFERENCE)
    {
      return Local<Value>();
    }

    if (JsSetException(error) != JsNoError)
    {
      return Handle<Value>();
    }
    rethrow = true;

    return Local<Value>::New((Value *)error);
  }

  Local<Value> TryCatch::Exception() const
  {
    if (error == JS_INVALID_REFERENCE)
    {
      const_cast<TryCatch*>(this)->GetAndClearException();
    }

    if (error == JS_INVALID_REFERENCE)
    {
      return Local<Value>();
    }

    return Local<Value>::New((Value *) error);
  }

  Local<Value> TryCatch::StackTrace() const
  {
    if (error == JS_INVALID_REFERENCE)
    {
      const_cast<TryCatch*>(this)->GetAndClearException();
    }

    if (error == JS_INVALID_REFERENCE)
    {
      return Local<Value>();
    }

    JsPropertyIdRef stack;
    if (JsGetPropertyIdFromName(L"stack", &stack) != JsNoError)
    {
      return Local<Value>();
    }

    JsValueRef trace;
    if (JsGetProperty(error, stack, &trace) != JsNoError)
    {
      return Local<Value>();
    }

    return Local<Value>::New((Value *) trace);
  }

  Local<v8::Message> TryCatch::Message() const
  {
    // return an empty ref for now, so no nulls/empty messages will be printed
    // should be changed once we understand how to retreive the info for each errror message
    return Local<v8::Message>();
  }

  void TryCatch::SetVerbose(bool value)
  {
    this->verbose = value;
  }

  void TryCatch::CheckReportExternalException()
  {
    // This is only used by Function::Call. If caller explictly uses a TryCatch and SetVerbose,
    // we'll report the external exception message.
    if (prev != nullptr && prev->verbose)
    {
      Isolate::GetCurrent()->InternalInvokeMessageListeners(Message(), Exception());
    }
    else
    {
      rethrow = true; // Otherwise leave the exception as is
    }
  }
}
