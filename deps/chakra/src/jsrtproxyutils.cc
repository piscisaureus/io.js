#include "jsrtproxyutils.h"
#include <string>
#include <map>

namespace jsrt
{
  using namespace std;
  __declspec(thread) JsValueRef proxyConstructorRef = JS_INVALID_REFERENCE;

  static map<ProxyTraps, JsPropertyIdRef> s_proxyTrapsPropertyIdsCache;

  const wchar_t* PROXY = L"Proxy";
  // Trap names:
  const wchar_t* TRAP_GET = L"get";
  const wchar_t* TRAP_SET = L"set";
  const wchar_t* TRAP_DELETE_PROPERTY = L"deleteProperty";
  const wchar_t* TRAP_DEFINE_PROPERTY = L"defineProperty";
  const wchar_t* TRAP_ENUMERATE = L"enumerate";
  const wchar_t* TRAP_OWN_KEYS = L"ownKeys";
  const wchar_t* TRAP_HAS = L"has";
  const wchar_t* TRAP_HAS_OWN = L"hasOwn";
  const wchar_t* TRAP_GET_PROPERTY_NAMES = L"getPropertyNames";
  const wchar_t* TRAP_GET_OWN_PROPERTY_NAMES = L"getOwnPropertyNames";
  const wchar_t* TRAP_GET_PROPERTY_DESCRIPTOR = L"getPropertyDescriptor";
  const wchar_t* TRAP_GET_OWN_PROPERTY_DESCRIPTOR = L"getOwnPropertyDescriptor";
  const wchar_t* TRAP_GET_PROTOTYPE_OF = L"getPrototypeOf";
  const wchar_t* TRAP_FREEZE = L"freeze";
  const wchar_t* TRAP_SEAL = L"seal";
  const wchar_t* TRAP_PREVENT_EXTENSIONS = L"preventExtensions";
  const wchar_t* TRAP_IS_FROZEN = L"isFrozen";
  const wchar_t* TRAP_IS_SEALED = L"isSealed";
  const wchar_t* TRAP_IS_EXTENSIBLE = L"isExtensible";
  const wchar_t* TRAP_APPLY = L"apply";
  const wchar_t* TRAP_CONTRUCT = L"construct";

  JsErrorCode GetProxyConstructor(_Out_ JsValueRef *result)
  {
    if (proxyConstructorRef != JS_INVALID_REFERENCE)
    {
      *result = proxyConstructorRef;
      return JsNoError;
    }

    return GetPropertyOfGlobal(PROXY, result);
  }

  JsErrorCode GetProxyConfigPropId(ProxyTraps trap, JsPropertyIdRef *idRef)
  {
    // check if we have the property in the cache
    if (s_proxyTrapsPropertyIdsCache.find(trap) != s_proxyTrapsPropertyIdsCache.end())
    {
      *idRef = s_proxyTrapsPropertyIdsCache[trap];
      return JsNoError;
    }
    
    // get the property id and cache it
    JsErrorCode error;
    switch (trap) 
    {
    case GetTrap:
      error = JsGetPropertyIdFromName(TRAP_GET, idRef);
      break;
    case SetTrap:
      error = JsGetPropertyIdFromName(TRAP_SET, idRef);
      break;
    case DeletePropertyTrap:
      error = JsGetPropertyIdFromName(TRAP_DELETE_PROPERTY, idRef);
      break;
    case EnumerateTrap:
      error = JsGetPropertyIdFromName(TRAP_ENUMERATE, idRef);
      break;
    case OwnKeysTrap:
      error = JsGetPropertyIdFromName(TRAP_OWN_KEYS, idRef);
      break;
    case HasTrap:
      error = JsGetPropertyIdFromName(TRAP_HAS, idRef);
      break;
    case HasOwnTrap:
      error = JsGetPropertyIdFromName(TRAP_HAS_OWN, idRef);
      break;
    case DefinePropertyTrap:
      error = JsGetPropertyIdFromName(TRAP_DEFINE_PROPERTY, idRef);
      break;
    case GetPropertyNamesTrap:
      error = JsGetPropertyIdFromName(TRAP_GET_PROPERTY_NAMES, idRef);
      break;
    case GetOwnPropertyNamesTrap:
      error = JsGetPropertyIdFromName(TRAP_GET_OWN_PROPERTY_NAMES, idRef);
      break;
    case GetPropertyDescriptorTrap:
      error = JsGetPropertyIdFromName(TRAP_GET_PROPERTY_DESCRIPTOR, idRef);
      break;
    case GetOwnPropertyDescriptorTrap:
      error = JsGetPropertyIdFromName(TRAP_GET_OWN_PROPERTY_DESCRIPTOR, idRef);
      break;
    case GetPrototypeOfTrap:
      error = JsGetPropertyIdFromName(TRAP_GET_PROTOTYPE_OF, idRef);
      break;
    case FreezeTrap:
      error = JsGetPropertyIdFromName(TRAP_FREEZE, idRef);
      break;
    case SealTrap:
      error = JsGetPropertyIdFromName(TRAP_SEAL, idRef);
      break;
    case PreventExtensionsTrap:
      error = JsGetPropertyIdFromName(TRAP_PREVENT_EXTENSIONS, idRef);
      break;
    case IsFrozenTrap:
      error = JsGetPropertyIdFromName(TRAP_IS_FROZEN, idRef);
      break;
    case IsSealedTrap:
      error = JsGetPropertyIdFromName(TRAP_IS_SEALED, idRef);
      break;
    case isExtensibleTrap:
      error = JsGetPropertyIdFromName(TRAP_IS_EXTENSIBLE, idRef);
      break;
    case ApplyTrap:
      error = JsGetPropertyIdFromName(TRAP_APPLY, idRef);
      break;
    case ContructTrap:
      error = JsGetPropertyIdFromName(TRAP_CONTRUCT, idRef);
      break;
    default:
      return JsErrorInvalidArgument;
    }

    if (error != JsNoError) return error;

    s_proxyTrapsPropertyIdsCache[trap] = *idRef;
    return JsNoError;
  }

  JsErrorCode SetPropertyOnTrapConfig(ProxyTraps trap, JsNativeFunction callback, _In_ JsValueRef configObj)
  {
    if (callback == nullptr) 
    {
      return JsNoError;
    }

    JsErrorCode error;

    JsValueRef func;
    error = JsCreateFunction(callback, nullptr, &func);
    if (error != JsNoError) return error;
    
    JsPropertyIdRef prop;
    error = GetProxyConfigPropId(trap, &prop);
    if (error != JsNoError) return error;

    error = JsSetProperty(configObj, prop, func, false);
    return error;
  }

  JsErrorCode CreateProxyTrapConfig(const std::map<ProxyTraps, JsNativeFunction>& config, _Out_ JsValueRef *confObj)
  {
    JsErrorCode error = JsNoError;
    
    error = JsCreateObject(confObj);

    if (error != JsNoError) return error;

    // Set the properties of the proxy configuration object according to the given map of proxy traps and function handlers
    // For each proxy trap - set the given handler using the appropriate javascript name
    for (std::map<ProxyTraps, JsNativeFunction>::const_iterator it = config.begin(); it != config.end(); it++)
    {
      error = SetPropertyOnTrapConfig(it->first, it->second, *confObj);
      if (error != JsNoError) return error;
    }
    
    return JsNoError;
  }

  JsErrorCode CreateProxy(
    _In_ JsValueRef target,
    const std::map<ProxyTraps, JsNativeFunction>& config,
    _Out_ JsValueRef *result
    )
  {
    JsErrorCode error;

    JsValueRef proxyConfigObj;
    error = CreateProxyTrapConfig(config, &proxyConfigObj);

    if (error != JsNoError) return error;

    JsValueRef proxyConstructorRef;
    error = GetProxyConstructor(&proxyConstructorRef);

    if (error != JsNoError) return error;

    JsValueRef args[] =  { nullptr, target, proxyConfigObj };

    error = JsConstructObject(proxyConstructorRef, args, _countof(args), result);

    return error;
  }

  JsErrorCode TryParseUInt32(JsValueRef strRef, bool* isUInt32, unsigned int *uint32Value)
  {
    // since we are using proxies here, the value will always be of type string
    // the fastest thing that we can do here is try and 
    // javascript max array size: 2^32-1 = 4,294,967,295 = 4.29: http://stackoverflow.com/questions/6154989/maximum-size-of-an-array-in-javascript

    JsErrorCode error;

    *isUInt32 = false;

    // check that every character in the str is a digit, and that the string does not start with a zero, unless it is a zero
    const wchar_t *strPtr;
    size_t strLength;

    error = JsStringToPointer(strRef, &strPtr, &strLength);

    if (error != JsNoError) return error;
    
    // empty string
    if (strLength == 0) return JsNoError;

    // deal with the case in which zero is the first letter, in which we will accept it only if the string reperesents zero itself
    if (strPtr[0] == L'0')
    {
      if (strLength == 1) 
      {
        *uint32Value = 0;
        *isUInt32 = true;
      }
      
      return JsNoError;
    }

    // iterate over the charecters, allow only digits:
    for (size_t i=0; i < strLength; i++)
    {
      if (strPtr[i] < L'0' || strPtr[i] > L'9')
      {
        return JsNoError;
      }
    }

    *isUInt32 = true;
    // use std:stoull as it the most comprehenisve way to convert string to int
    // there is some performance issue here, so we migth optimiaze this code using the reuslts in here: 
    // string to int conversion - naive approach is the fastest: http://tinodidriksen.com/2010/02/16/cpp-convert-string-to-int-speed/

    wchar_t* strEnd = const_cast<wchar_t*>(strPtr) + strLength;

    unsigned long longVal = std::wcstoul(strPtr, &strEnd, 10);

    if (longVal == ULONG_MAX)
    {
      // check if errno is set:
      if (errno == ERANGE)
      {
        *isUInt32 = false;
        return JsNoError;
      }
    }
    
    *uint32Value = longVal;
    return JsNoError;
  }
}
