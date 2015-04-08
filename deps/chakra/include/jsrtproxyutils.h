#pragma once

#include "jsrt.h"
#include "jsrtutils.h"
#include <map>

namespace jsrt
{
  // Proxy Traps types, please see:
  // http://wiki.ecmascript.org/doku.php?id=harmony:direct_proxies
  // for more details regarding harmony proxies
  enum ProxyTraps
  {
    GetTrap,
    SetTrap,
    DeletePropertyTrap,
    EnumerateTrap,
    OwnKeysTrap,
    HasTrap,
    HasOwnTrap,
    DefinePropertyTrap,
    GetPropertyNamesTrap,
    GetOwnPropertyNamesTrap,
    GetPropertyDescriptorTrap,
    GetOwnPropertyDescriptorTrap,
    GetPrototypeOfTrap,
    FreezeTrap,
    SealTrap,
    PreventExtensionsTrap,
    IsFrozenTrap,
    IsSealedTrap,
    isExtensibleTrap,
    ApplyTrap,
    ContructTrap
  };

  JsErrorCode CreateProxy(
    _In_ JsValueRef target,
    _In_ const std::map<ProxyTraps, JsNativeFunction>& conf,
    _Out_ JsValueRef *result
    );

  JsErrorCode TryParseUInt32(
    _In_ JsValueRef strRef, 
    _Out_ bool* isUInt32, 
    _Out_ unsigned int *uint32Value);
}
