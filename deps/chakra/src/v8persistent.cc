#include <jsrt.h>
#include <v8.h>
#include "jsrtutils.h"

namespace v8
{
  namespace internal
  {
    struct WeakReferenceCallbackWrapper
    {
      void *parameters;
      WeakCallbackData<Value, void>::Callback callback;
    };

    static void CALLBACK WeakReferenceCallbackWrapperCallback(_In_ JsRef ref, _In_opt_ void *data)
    {
      WeakReferenceCallbackWrapper *callbackWrapper = reinterpret_cast<WeakReferenceCallbackWrapper*>(data);
      WeakCallbackData<Value, void> callbackData(Isolate::GetCurrent(), (Value*)ref, callbackWrapper->parameters);
      callbackWrapper->callback(callbackData);
    }

    void ClearObjectWeakReferenceCallback(JsValueRef object)
    {
      JsSetObjectBeforeCollectCallback(object, nullptr, nullptr);
    }

    void SetObjectWeakReferenceCallback(JsValueRef object, WeakCallbackData<Value, void>::Callback callback, void* parameters, std::shared_ptr<WeakReferenceCallbackWrapper>* weakWrapper)
    {
      if (callback == nullptr || object == JS_INVALID_REFERENCE)
      {
        return;
      }

      if (!*weakWrapper)
      {
        *weakWrapper = std::make_shared<WeakReferenceCallbackWrapper>();
      }

      WeakReferenceCallbackWrapper *callbackWrapper = (*weakWrapper).get();
      callbackWrapper->parameters = parameters;
      callbackWrapper->callback = callback;

      JsSetObjectBeforeCollectCallback(object, callbackWrapper, WeakReferenceCallbackWrapperCallback);
    }
  }
}
