#include "jsrtutils.h"
#include <functional>

namespace jsrt
{
  __declspec(thread) JsValueRef instanceOfRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef cloneObjectRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef getPropertyNamesRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef getOwnPropertyDescriptorRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef getEnumerableNamedPropertiesRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef getEnumerableIndexedPropertiesRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef createEnumerationIteratorRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef createPropertyDescriptorsEnumerationIteratorRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef getNamedOwnKeysRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef getIndexedOwnKeysRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef trueRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef falseRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef undefinedRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef zeroRef = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef isUintFunctionRef = JS_INVALID_REFERENCE;

  const wchar_t * DEFAULT_EXTENRAL_DATA_NAME = L"__external__";

  JsErrorCode GetProperty(
    _In_ JsValueRef ref, 
    _In_ JsValueRef propName,
    _Out_ JsValueRef *result)
  {
    JsPropertyIdRef idRef;
    JsErrorCode error;

    size_t strLength;
    const  wchar_t *strPtr;

    error = JsStringToPointer(propName, &strPtr, &strLength);
    if (error != JsNoError) return error;

    error = JsGetPropertyIdFromName(strPtr, &idRef);
    if (error != JsNoError) return error;

    error = JsGetProperty(ref, idRef, result);

    return error;
  }

  JsErrorCode GetProperty(
    _In_ JsValueRef ref, 
    _In_ const wchar_t *propertyName,
    _Out_ JsValueRef *result)
  {
    JsPropertyIdRef idRef;
    JsErrorCode error;

    error = JsGetPropertyIdFromName(propertyName, &idRef);

    if (error != JsNoError) return error;

    error = JsGetProperty(ref, idRef, result);

    return error;
  }

  JsErrorCode SetProperty(
    _In_ JsValueRef ref, 
    _In_ const wchar_t* propName,
    _In_ JsValueRef propValue)
  {
    JsPropertyIdRef idRef;
    JsErrorCode error;

    error = JsGetPropertyIdFromName(propName, &idRef);
    if (error != JsNoError) return error;

    error = JsSetProperty(ref, idRef, propValue, false);

    return error;
  }

  JsErrorCode SetProperty(
    _In_ JsValueRef ref, 
    _In_ JsValueRef propName,
    _In_ JsValueRef propValue)
  {
    JsPropertyIdRef idRef;
    JsErrorCode error;

    size_t strLength;
    const  wchar_t *strPtr;

    error = JsStringToPointer(propName, &strPtr, &strLength);
    if (error != JsNoError) return error;

    error = JsGetPropertyIdFromName(strPtr, &idRef);
    if (error != JsNoError) return error;

    error = JsSetProperty(ref, idRef, propValue, false);

    return error;
  }

  JsErrorCode DeleteProperty(
    _In_ JsValueRef ref, 
    _In_ JsValueRef propName,
    _Out_ JsValueRef* result)
  {
    JsPropertyIdRef idRef;
    JsErrorCode error;

    size_t strLength;
    const  wchar_t *strPtr;

    error = JsStringToPointer(propName, &strPtr, &strLength);
    if (error != JsNoError) return error;

    error = JsGetPropertyIdFromName(strPtr, &idRef);
    if (error != JsNoError) return error;

    error = JsDeleteProperty(ref, idRef, false, result);

    return error;
  }

  JsErrorCode CallProperty(
    _In_ JsValueRef ref, 
    _In_ const wchar_t *propertyName, 
    _In_reads_(argumentCount) JsValueRef *arguments,
    _In_ unsigned short argumentCount,
    _Out_ JsValueRef *result)
  {
    JsValueRef propertyRef;
    JsErrorCode error;

    error = GetProperty(ref, propertyName, &propertyRef);

    if (error != JsNoError) return error;

    error = JsCallFunction(propertyRef, arguments, argumentCount, result);

    return error;
  }

  JsErrorCode GetPropertyOfGlobal(_In_ const wchar_t *propertyName, 
    _Out_ JsValueRef *ref)
  {
    JsErrorCode error = JsNoError;
    JsValueRef globalRef;

    error = JsGetGlobalObject(&globalRef);

    if (error != JsNoError) return error;

    error = GetProperty(globalRef, propertyName, ref);

    return error;
  }

  JsErrorCode SetPropertyOfGlobal(
    _In_ const wchar_t *propertyName,
    _In_ JsValueRef ref)
  {
    JsErrorCode error = JsNoError;
    JsValueRef globalRef;

    error = JsGetGlobalObject(&globalRef);

    if (error != JsNoError) return error;

    JsPropertyIdRef propertyIdRef;
    error = JsGetPropertyIdFromName(propertyName, &propertyIdRef);
    if (error != JsNoError) return error;

    error = JsSetProperty(globalRef, propertyIdRef, ref, false);

    return error;
  }

  JsErrorCode GetGlobalType(
    _In_ const wchar_t* typeName,
    _Out_ JsRef *ref)
  {
    return GetPropertyOfGlobal(typeName, ref);
  }

  JsErrorCode GetPropertyOfGlobalType(
    _In_ const wchar_t* typeName,
    _In_ const wchar_t* propertyName,
    _Out_ JsRef *ref)
  {
    JsValueRef globalTypeRef;
    JsErrorCode error;

    error = GetGlobalType(typeName, &globalTypeRef);

    if (error != JsNoError) return error;

    error = GetProperty(globalTypeRef, propertyName, ref);

    return error;
  }

  JsErrorCode GetProtoypeOfGlobalType(
    _In_ const wchar_t* typeName,
    _Out_ JsRef *ref)
  {
    JsValueRef globalTypeRef;
    JsErrorCode error;

    error = GetGlobalType(typeName, &globalTypeRef);

    if (error != JsNoError) return error;

    error = JsGetPrototype(globalTypeRef, ref);

    return error;
  }

  JsValueRef GetUndefined()
  {
    if (undefinedRef == JS_INVALID_REFERENCE)
    { 
      if (JsGetUndefinedValue(&undefinedRef) != JsNoError)
      {
        return JS_INVALID_REFERENCE;
      }
    }

    return undefinedRef;
  }

  JsValueRef GetTrue()
  {
    if (trueRef == JS_INVALID_REFERENCE)
    {
      if (JsGetTrueValue(&trueRef) != JsNoError)
      {
        return JS_INVALID_REFERENCE;
      }
    }

    return trueRef;
  }

  JsValueRef GetFalse()
  {
    if (falseRef == JS_INVALID_REFERENCE)
    {
      if (JsGetFalseValue(&falseRef) != JsNoError)
      {
        return JS_INVALID_REFERENCE;
      }
    }

    return falseRef;
  }

  JsErrorCode GetArrayLength(_In_ JsValueRef arrayRef, 
    _Out_ unsigned int *arraySize)
  {
    JsErrorCode error;

    JsPropertyIdRef arrayLengthPropertyIdRef;
    error = JsGetPropertyIdFromName(L"length", &arrayLengthPropertyIdRef);
    if (error != JsNoError) return error;

    JsValueRef lengthRef;

    error = JsGetProperty(arrayRef, arrayLengthPropertyIdRef, &lengthRef);
    if (error != JsNoError) return error;

    double sizeInDouble;
    error = JsNumberToDouble(lengthRef, &sizeInDouble);
    *arraySize = static_cast<unsigned int>(sizeInDouble);

    return error;
  }

  JsErrorCode InstanceOf(
    _In_ JsValueRef first, 
    _In_ JsValueRef second,
    _Out_ bool *result)
  {
    JsErrorCode error;
    if (instanceOfRef == JS_INVALID_REFERENCE)
    {
      error = JsRunScript(L"(function() {\n\
                           var isInstanceOf = function(a,b) {\n\
                           return (a instanceof b);\n\
                           };\n\
                           Object.defineProperty(this, '__isInstanceOf__', { enumerable: false, configurable: false, writable: false, value: isInstanceOf });\n\
                           return this.__isInstanceOf__;\n\
                           }).call(this);",
                           JS_SOURCE_CONTEXT_NONE, 
                           L"jsrtUtils::InstanceOf", 
                           &instanceOfRef);
      if (error != JsNoError) return error;
    }

    JsValueRef args[] = {nullptr, first, second};
    JsValueRef resultRef;
    error = JsCallFunction(instanceOfRef, args, _countof(args), &resultRef);

    if (error != JsNoError) return error;

    error = JsBooleanToBool(resultRef, result);

    return error;
  }

  JsErrorCode InstanceOfGlobalType(
    _In_ JsValueRef first, 
    _In_ const wchar_t* typeName,
    _Out_ bool *result)
  {
    JsValueRef typeRef;
    JsErrorCode error;
    error = jsrt::GetPropertyOfGlobal(typeName, &typeRef);
    if (error != JsNoError) return error;

    error = jsrt::InstanceOf(first, typeRef, result);
    return error;
  }

  JsErrorCode CloneObject(
    _In_ JsValueRef source, 
    _In_ JsValueRef target,
    _In_ bool clonePrototype)
  {
    JsErrorCode error;
    if (cloneObjectRef == JS_INVALID_REFERENCE)
    {
      error = JsRunScript(
        L"(function() {\n\
         var cloneObject = function(source, target) {\n\
         Object.getOwnPropertyNames(source).forEach(function(key) {\n\
         try {\n\
         var desc = Object.getOwnPropertyDescriptor(source, key);\n\
         if (desc.value === source) desc.value = target;\n\
         Object.defineProperty(target, key, desc);\n\
         } catch (e) {\n\
         // Catch sealed properties errors\n\
         }\n\
         });\n\
         };\n\
         Object.defineProperty(this, '__cloneObject__', { enumerable: false, configurable: false, writable: false, value: cloneObject });\n\
         return this.__cloneObject__;\n\
         }).call(this)",
         JS_SOURCE_CONTEXT_NONE,
         L"jsrtUtils::cloneObject",
         &cloneObjectRef);

      if (error != JsNoError) return error;
    }

    JsValueRef args[] = {nullptr, source, target};
    JsValueRef resultRef;
    error = JsCallFunction(cloneObjectRef, args, _countof(args), &resultRef);
    if (error != JsNoError) return error;

    if (clonePrototype)
    {
      JsValueRef prototypeRef;
      JsErrorCode error =  JsGetPrototype(source, &prototypeRef);
      if (error != JsNoError) return error;
      error = JsSetPrototype(target, prototypeRef);
    }

    return error;
  }

  JsErrorCode HasOwnProperty(
    _In_ JsValueRef object, 
    _In_ JsValueRef prop,
    _Out_ JsValueRef *result)
  {
    JsValueRef resultRef;
    JsValueRef args[] = {object, prop};
    JsErrorCode error = jsrt::CallProperty(object, L"hasOwnProperty", args, _countof(args), &resultRef);
    
    if (error != JsNoError) return error;

    return error;
  }

  JsErrorCode IsValueInArray(
    _In_ JsValueRef arrayRef, 
    _In_ JsValueRef valueRef,
    _In_ std::function<JsErrorCode (JsValueRef,JsValueRef, bool*)> comperator,
    _Out_ bool* result)
  {
    JsErrorCode error;
    unsigned int length;
    *result = false;
    error = GetArrayLength(arrayRef, &length);
    if (error != JsNoError) return error;

    for (unsigned int index = 0; index < length; index++)
    {
      JsValueRef indexValue;
      error = JsIntToNumber(index, &indexValue);
      if (error != JsNoError) return error;

      JsValueRef itemRef;
      error = JsGetIndexedProperty(arrayRef, indexValue, &itemRef);
      if (error != JsNoError) return error;

      if (comperator != nullptr)
      {
        error = comperator(valueRef, itemRef, result);
      }
      else 
      {
        error = JsEquals(itemRef, valueRef, result);
      }
      
      if (error != JsNoError) return error;

      if (*result)
        return JsNoError;
    }

    return JsNoError;
  }

  JsErrorCode IsValueInArray(
    _In_ JsValueRef arrayRef, 
    _In_ JsValueRef valueRef,
    _Out_ bool* result)
  {
    return IsValueInArray(arrayRef, valueRef, nullptr, result);
  }

  JsErrorCode IsCaseInsensitiveStringValueInArray(
    _In_ JsValueRef arrayRef, 
    _In_ JsValueRef valueRef,
    _Out_ bool* result)
  {
    return IsValueInArray(arrayRef, valueRef, [=](JsValueRef first, JsValueRef second, bool* areEqual) -> JsErrorCode {
      JsValueType type;
      *areEqual = false;

      JsErrorCode error = JsGetValueType(first, &type);
      if (error != JsNoError) return error;
      if (type != JsString) return JsNoError;

      error = JsGetValueType(second, &type);
      if (error != JsNoError) return error;
      if (type != JsString) return JsNoError;

      const wchar_t* firstPtr;
      size_t firstLength;

      error = JsStringToPointer(first, &firstPtr, &firstLength);
      if (error != JsNoError) return error;

      const wchar_t* secondPtr;
      size_t secondLength;

      error = JsStringToPointer(second, &secondPtr, &secondLength);
      if (error != JsNoError) return error;

      size_t maxCount = min(firstLength, secondLength);
      *areEqual = (_wcsnicmp(firstPtr, secondPtr, maxCount) == 0);
      return JsNoError;
    },
    result);
  }

  JsErrorCode GetOwnPropertyDescriptor(
    _In_ JsValueRef ref, 
    _In_ JsValueRef prop,
    _Out_ JsValueRef* result)
  {
    JsErrorCode error;
    if (getOwnPropertyDescriptorRef == JS_INVALID_REFERENCE)
    {
      error = GetPropertyOfGlobalType(L"Object", L"getOwnPropertyDescriptor", &getOwnPropertyDescriptorRef);
      if (error != JsNoError) return error;
    }

    JsValueRef args[] = {nullptr, ref, prop};
    error = JsCallFunction(getOwnPropertyDescriptorRef, args, _countof(args), result);
    return error;
  }

  JsErrorCode IsZero(
    _In_ JsValueRef value,
    _Out_ bool *result)
  {
    JsErrorCode error;
    if (zeroRef == JS_INVALID_REFERENCE)
    {
      error = JsIntToNumber(0, &zeroRef);
      if (error != JsNoError) return error;
    }

    error = JsEquals(value, zeroRef, result);
    return error;
  }

  JsErrorCode IsUndefined(
    _In_ JsValueRef value,
    _Out_ bool *result)
  {
    JsErrorCode error;
    if (undefinedRef == JS_INVALID_REFERENCE)
    {
      error = JsGetUndefinedValue(&undefinedRef);
      if (error != JsNoError) return error;
    }

    error = JsEquals(value, undefinedRef, result);
    return error;
  }

  JsErrorCode DefineIsUintFunction()
  {
    if (isUintFunctionRef != JS_INVALID_REFERENCE)
    {
      return JsNoError;
    }

    JsErrorCode error = JsRunScript(
        L"(function() {\n\
         var isUintRegex = /^(0|[1-9]\\d*)$/; \n\
         var isUint = function(value) { \n\
          var result = isUintRegex.test(value); \n\
          isUintRegex.lastIndex = 0; \n\
          return result; \n\
         }; \n\
         Object.defineProperty(this, '__isUint__', { enumerable: false, configurable: false, writable: false, value: isUint });\n\
         return this.__isUint__;\n\
         }).call(this)",
         JS_SOURCE_CONTEXT_NONE,
         L"jsrtUtils::isUint",
         &isUintFunctionRef);
    
    return error;
  }

  JsErrorCode GetEnumerableNamedProperties(
    _In_ JsValueRef object, 
    _Out_ JsValueRef *result)
  {
    JsErrorCode error;
    if (getEnumerableNamedPropertiesRef == JS_INVALID_REFERENCE)
    {
      error = DefineIsUintFunction();
      if (error != JsNoError) return error;

      error = JsRunScript(
        L"(function() {\n\
         var getEnumerableNamedProperties = function(obj, isUint) {\n\
           var props = [];\n\
           for (var key in obj)\
           {\
           if (!isUint(key))\n\
               props.push(key); \n\
           }\n\
           return props;\n\
         };\n\
         Object.defineProperty(this, '__getEnumerableNamedProperties__', { enumerable: false, configurable: false, writable: false, value: getEnumerableNamedProperties });\n\
         return this.__getEnumerableNamedProperties__;\n\
         }).call(this)",
         JS_SOURCE_CONTEXT_NONE,
         L"jsrtUtils::getEnumerableNamedProperties",
         &getEnumerableNamedPropertiesRef);
    }

    JsValueRef args[] = {nullptr, object, isUintFunctionRef};
    error = JsCallFunction(getEnumerableNamedPropertiesRef, args, _countof(args), result);
    return error;
  }

 JsErrorCode GetEnumerableIndexedProperties(
    _In_ JsValueRef object, 
    _Out_ JsValueRef *result)
  {
    JsErrorCode error;
    if (getEnumerableIndexedPropertiesRef == JS_INVALID_REFERENCE)
    {
      error = DefineIsUintFunction();
      if (error != JsNoError) return error;

      error = JsRunScript(
        L"(function() {\n\
         var getEnumerableIndexedProperties = function(obj, isUint) {\n\
           var props = [];\n\
           for (var key in obj)\n\
           {\n\
             if (isUint(key))\n\
               props.push(key); \n\
           }\n\
           return props;\n\
         };\n\
         Object.defineProperty(this, '__getEnumerableIndexedProperties__', { enumerable: false, configurable: false, writable: false, value: getEnumerableIndexedProperties });\n\
         return this.__getEnumerableIndexedProperties__;\n\
         }).call(this)",
         JS_SOURCE_CONTEXT_NONE,
         L"jsrtUtils::getEnumerableIndexedProperties",
         &getEnumerableIndexedPropertiesRef);
    }

    JsValueRef args[] = {nullptr, object, isUintFunctionRef};
    error = JsCallFunction(getEnumerableIndexedPropertiesRef, args, _countof(args), result);
    return error;
  }

 JsErrorCode GetIndexedOwnKeys(
    _In_ JsValueRef object, 
    _Out_ JsValueRef *result)
 {
   JsErrorCode error;
    if (getIndexedOwnKeysRef == JS_INVALID_REFERENCE)
    {
      error = DefineIsUintFunction();
      if (error != JsNoError) return error;

      error = JsRunScript(
        L"(function() {\n\
         var getIndexedOwnKeys = function(obj, isUint) {\n\
           var props = [];\n\
           Object.keys(obj).forEach(function(item) {\n\
             if (isUint(item))\n\
               props.push(item); \n\
           });\n\
           return props;\n\
         };\n\
         Object.defineProperty(this, '__getIndexedOwnKeys__', { enumerable: false, configurable: false, writable: false, value: getIndexedOwnKeys });\n\
         return this.__getIndexedOwnKeys__;\n\
         }).call(this)",
         JS_SOURCE_CONTEXT_NONE,
         L"jsrtUtils::getIndexedOwnKeys",
         &getIndexedOwnKeysRef);
    }

    JsValueRef args[] = {nullptr, object, isUintFunctionRef};
    error = JsCallFunction(getIndexedOwnKeysRef, args, _countof(args), result);
    return error;
 }

 JsErrorCode GetNamedOwnKeys(
    _In_ JsValueRef object, 
    _Out_ JsValueRef *result)
 {
   JsErrorCode error;
   if (getNamedOwnKeysRef == JS_INVALID_REFERENCE)
   {
     error = DefineIsUintFunction();
     if (error != JsNoError) return error;

     error = JsRunScript(
       L"(function() {\n\
        var getNamedOwnKeys = function(obj, isUint) {\n\
          var props = [];\n\
          Object.keys(obj).forEach(function(item) {\n\
          {\n\
            if (!isUint(item))\n\
              props.push(item); \n\
          }\n\
          return props;\n\
        };\n\
        Object.defineProperty(this, '__getNamedOwnKeys__', { enumerable: false, configurable: false, writable: false, value: getNamedOwnKeys });\n\
        return this.__getNamedOwnKeys__;\n\
        }).call(this)",
        JS_SOURCE_CONTEXT_NONE,
        L"jsrtUtils::getNamedOwnKeys",
        &getNamedOwnKeysRef);
    }

    JsValueRef args[] = {nullptr, object, isUintFunctionRef};
    error = JsCallFunction(getNamedOwnKeysRef, args, _countof(args), result);
    return error;
 }

 JsErrorCode ConcatArray(
   _In_ JsValueRef first, 
   _In_ JsValueRef second, 
   _Out_ JsValueRef *result
   )
 {
   JsValueRef args[] = {first, second};

   return CallProperty(first, L"concat", args, _countof(args), result);
 }

 JsErrorCode CreateEnumerationIterator(
    _In_ JsValueRef enumeration, 
    _Out_ JsValueRef *result)
 {
    JsErrorCode error;
    if (createEnumerationIteratorRef == JS_INVALID_REFERENCE)
    {
      error = JsRunScript(
        L"(function() {\n\
         var createEnumerationIterator = function(props) {\n\
           var i = 0;\n\
           return {\n\
            next: function() {\n\
                if (i === props.length)\n\
                    return { done: true }\n\
                return { value : props[i++] };\n\
            }\n\
          };\n\
         };\n\
         Object.defineProperty(this, '__createEnumerationIterator__', { enumerable: false, configurable: false, writable: false, value: createEnumerationIterator });\n\
         return this.__createEnumerationIterator__;\n\
         }).call(this)",
         JS_SOURCE_CONTEXT_NONE,
         L"jsrtUtils::createEnumerationIterator",
         &createEnumerationIteratorRef);
    }

    JsValueRef args[] = {nullptr, enumeration};
    error = JsCallFunction(createEnumerationIteratorRef, args, _countof(args), result);
    return error;
 }

 JsErrorCode CreatePropertyDescriptorsEnumerationIterator(
    _In_ JsValueRef enumeration, 
    _Out_ JsValueRef *result)
 {
    JsErrorCode error;
    if (createPropertyDescriptorsEnumerationIteratorRef == JS_INVALID_REFERENCE)
    {
      error = JsRunScript(
        L"(function() {\n\
         var createPropertyDescriptorsEnumerationIterator = function(props) {\n\
           var i = 0;\n\
           return {\n\
            next: function() {\n\
                if (i === props.length)\n\
                    return { done: true }\n\
                    return { name : props[i++], enumerable : true };\n\
            }\n\
          };\n\
         };\n\
         Object.defineProperty(this, '__createPropertyDescriptorsEnumerationIterator__', { enumerable: false, configurable: false, writable: false, value: createPropertyDescriptorsEnumerationIterator });\n\
         return this.__createEnumerationIterator__;\n\
         }).call(this)",
         JS_SOURCE_CONTEXT_NONE,
         L"jsrtUtils::createEnumerationIterator",
         &createPropertyDescriptorsEnumerationIteratorRef);
    }

    JsValueRef args[] = {nullptr, enumeration};
    error = JsCallFunction(createPropertyDescriptorsEnumerationIteratorRef, args, _countof(args), result);
    return error;
 }

  JsErrorCode GetPropertyNames(
    _In_ JsValueRef object, 
    _Out_ JsValueRef *namesArray)
  {
    JsErrorCode error;
    if (getPropertyNamesRef == JS_INVALID_REFERENCE)
    {
      error = JsRunScript(L"(function() {\n\
                           var getPropertyNames = function(a) {\n\
                           var names = []; \n\
                           for(var propertyName in a) { \n\
                           names.push(propertyName); \n\
                           } \n\
                           return names;\n\
                           };\n\
                           Object.defineProperty(this, '__getPropertyNames__', { enumerable: false, configurable: false, writable: false, value: getPropertyNames });\n\
                           return this.__getPropertyNames__;\n\
                           }).call(this);",
                           JS_SOURCE_CONTEXT_NONE, 
                           L"jsrtUtils::GetPropertyNames", 
                           &getPropertyNamesRef);
      if (error != JsNoError) return error;
    }

    JsValueRef args[] = {nullptr, object};
    error = JsCallFunction(getPropertyNamesRef, args, _countof(args), namesArray);

    return error;
  }

  JsErrorCode GetExternalDataDefaultName(
    _Out_ const wchar_t **defaultName)
  {
    *defaultName = DEFAULT_EXTENRAL_DATA_NAME;
    return JsNoError;
  }

  JsErrorCode AddExternalData(
    _In_ JsValueRef ref,
    _In_ JsPropertyIdRef externalDataPropertyId,
    _In_ void *data,
    _In_ JsFinalizeCallback onObjectFinalize)
  {
    JsErrorCode error;

    JsValueRef externalObjectRef;
    error = JsCreateExternalObject(data, onObjectFinalize, &externalObjectRef);
    if (error != JsNoError) return error;

    error = DefineProperty(ref, externalDataPropertyId,PropertyDescriptorOptionValues::False, PropertyDescriptorOptionValues::False, 
      PropertyDescriptorOptionValues::False, externalObjectRef, JS_INVALID_REFERENCE, JS_INVALID_REFERENCE);

    return error;
  }

  JsErrorCode AddExternalData(_In_ JsValueRef ref,
    _In_ const wchar_t *externalDataName,
    _In_ void *data,
    _In_ JsFinalizeCallback onObjectFinalize)
  {
    JsErrorCode error;
    JsPropertyIdRef idRef;
    error = JsGetPropertyIdFromName(externalDataName , &idRef);
    if (error != JsNoError) return error;

    return AddExternalData(ref, idRef, data, onObjectFinalize);
  }

  JsErrorCode AddExternalData(
    _In_ JsValueRef ref,
    _In_ void *data,
    _In_ JsFinalizeCallback onObjectFinalize)
  {
    JsErrorCode error;
    JsPropertyIdRef defaultExternalDataPropertyIdRef;
    error = JsGetPropertyIdFromName(DEFAULT_EXTENRAL_DATA_NAME, &defaultExternalDataPropertyIdRef);
    if (error != JsNoError) return error;


    return AddExternalData(ref, defaultExternalDataPropertyIdRef, data, onObjectFinalize);
  }

  JsErrorCode HasExternalData(
    _In_ JsValueRef ref,
    JsPropertyIdRef propertyId,
    _Out_ bool *result)
  {
    return JsHasProperty(ref, propertyId, result);
  }

  JsErrorCode HasExternalData(
    _In_ JsValueRef ref,
    _In_ const wchar_t *externalDataName,
    _Out_ bool *result)
  {
    JsErrorCode error;
    JsPropertyIdRef idRef;
    error = JsGetPropertyIdFromName(externalDataName , &idRef);
    if (error != JsNoError) return error;

    return HasExternalData(ref, idRef, result);
  }

  JsErrorCode HasExternalData(
    _In_ JsValueRef ref, 
    _Out_ bool *result)
  {
    JsErrorCode error;
    JsPropertyIdRef defaultExternalDataPropertyIdRef;
    error = JsGetPropertyIdFromName(DEFAULT_EXTENRAL_DATA_NAME, &defaultExternalDataPropertyIdRef);
    if (error != JsNoError) return error;

    return HasExternalData(ref, defaultExternalDataPropertyIdRef, result);
  }

  JsErrorCode GetExternalData(
    _In_ JsValueRef ref,
    _In_ JsPropertyIdRef idRef,
    _Out_ void **data)
  {
    JsErrorCode error;
    bool hasProperty;
    error = HasExternalData(ref, idRef, &hasProperty);
    if (error != JsNoError) return error;

    if (!hasProperty)
    {
      *data = nullptr;
      return JsNoError;
    }

    JsValueRef externalObject;
    error = JsGetProperty(ref, idRef, &externalObject);
    if (error != JsNoError) return error;

    error = JsGetExternalData(externalObject, data);
    return error;
  }

  JsErrorCode GetExternalData(_In_ JsValueRef ref,
    _In_ const wchar_t *externalDataName,
    _Out_ void **data)
  {
    JsErrorCode error;
    JsPropertyIdRef propertyIdRef;
    error = JsGetPropertyIdFromName(externalDataName, &propertyIdRef);
    if (error != JsNoError) return error;

    error = GetExternalData(ref, propertyIdRef, data);
    return error;
  }

  JsErrorCode GetExternalData(
    _In_ JsValueRef ref,
    _Out_ void **data)
  {
    JsErrorCode error;
    JsPropertyIdRef defaultExternalDataPropertyIdRef;
    error = JsGetPropertyIdFromName(DEFAULT_EXTENRAL_DATA_NAME, &defaultExternalDataPropertyIdRef);
    if (error != JsNoError) return error;

    error = GetExternalData(ref, defaultExternalDataPropertyIdRef, data);
    return error;
  }

  JsErrorCode CreateFunctionWithExternalData(
    _In_ JsNativeFunction nativeFunction, 
    void* data,
    _In_ JsFinalizeCallback onObjectFinalize,
    _Out_ JsValueRef *function)
  {
    JsErrorCode error;
    error = JsCreateFunction(nativeFunction, nullptr, function);
    if (error != JsNoError) return error;

    error = AddExternalData(*function, data, onObjectFinalize);
    return error;
  }

  const wchar_t* DebugToString(_In_ JsValueRef ref)
  {
    const wchar_t* str = nullptr;
    ToString(ref, &str);

    return str;
  }

  JsErrorCode ToString(_In_ JsValueRef ref,
    _Out_ const wchar_t** str,
    _In_ bool alreadyString)
  {
    // just a dummy here
    size_t size; 
    JsRef strRef;
    JsErrorCode error;

    // call convert only if needed
    if (alreadyString)
    {
      strRef = ref;
    }
    else
    {
      error = JsConvertValueToString(ref, &strRef);
      if (error != JsNoError) return error;
    }

    error = JsStringToPointer(strRef, str, &size);
    return error;
  }

  JsErrorCode GetExceptionMessage(_Out_ const wchar_t** str)
  {
    JsErrorCode error;
    bool hasException;
    error = JsHasException(&hasException);
    if (error != JsNoError) return error; 

    if (!hasException)
    {
      str = nullptr;
      return JsNoError;
    }

    JsValueRef exceptionRef;
    error = JsGetAndClearException(&exceptionRef);

    if (error != JsNoError) return error;

    return ToString(exceptionRef, str);
  }

  JsErrorCode CreatePropertyDescriptor(
    _In_ PropertyDescriptorOptionValues writable,
    _In_ PropertyDescriptorOptionValues enumerable,
    _In_ PropertyDescriptorOptionValues configurable,
    _In_ JsValueRef value,
    _In_ JsValueRef getter,
    _In_ JsValueRef setter,
    _Out_ JsValueRef *descriptor)
  {
    JsErrorCode error;
    error = JsCreateObject(descriptor);
    if (error != JsNoError) return error;


    // set writable
    if (writable != PropertyDescriptorOptionValues::None)
    {
      JsPropertyIdRef writablePropertyIdRef;
      error = JsGetPropertyIdFromName(L"writable",&writablePropertyIdRef);
      if (error != JsNoError) return error;
      JsValueRef writableRef;
      error = JsBoolToBoolean((writable == PropertyDescriptorOptionValues::True), &writableRef);
      if (error != JsNoError) return error;
      error = JsSetProperty(*descriptor, writablePropertyIdRef, writableRef, false);
      if (error != JsNoError) return error;
    }

    // set enumerable
    if (enumerable != PropertyDescriptorOptionValues::None)
    {
      JsPropertyIdRef enumerablePropertyIdRef;
      error = JsGetPropertyIdFromName(L"enumerable",&enumerablePropertyIdRef);
      if (error != JsNoError) return error;
      JsValueRef enumerableRef;
      error = JsBoolToBoolean((enumerable  == PropertyDescriptorOptionValues::True), &enumerableRef);
      if (error != JsNoError) return error;
      error = JsSetProperty(*descriptor, enumerablePropertyIdRef, enumerableRef, false);
      if (error != JsNoError) return error;
    }

    // set configurable
    if (configurable != PropertyDescriptorOptionValues::None)
    {
      JsPropertyIdRef configurablePropertyIdRef;
      error = JsGetPropertyIdFromName(L"configurable",&configurablePropertyIdRef);
      if (error != JsNoError) return error;
      JsValueRef configurableRef;
      error = JsBoolToBoolean((configurable == PropertyDescriptorOptionValues::True), &configurableRef);
      if (error != JsNoError) return error;
      error = JsSetProperty(*descriptor, configurablePropertyIdRef, configurableRef, false);
      if (error != JsNoError) return error;
    }

    // set value
    if (value != JS_INVALID_REFERENCE)
    {
      JsPropertyIdRef valuePropertyIdRef;
      error = JsGetPropertyIdFromName(L"value",&valuePropertyIdRef);
      if (error != JsNoError) return error;
      error = JsSetProperty(*descriptor, valuePropertyIdRef, value, false);
      if (error != JsNoError) return error;
    }

    // set getter if needed
    if (getter != JS_INVALID_REFERENCE)
    {
      JsPropertyIdRef getterPropertyIdRef;
      error = JsGetPropertyIdFromName(L"get",&getterPropertyIdRef);
      if (error != JsNoError) return error;
      error = JsSetProperty(*descriptor, getterPropertyIdRef, getter, false);
      if (error != JsNoError) return error;
    }

    // set setter if needed
    if (setter != JS_INVALID_REFERENCE)
    {
      JsPropertyIdRef setterPropertyIdRef;
      error = JsGetPropertyIdFromName(L"set",&setterPropertyIdRef);
      if (error != JsNoError) return error;
      error = JsSetProperty(*descriptor, setterPropertyIdRef, setter, false);
      if (error != JsNoError) return error;
    }

    return JsNoError;
  }

  JsErrorCode DefineProperty(_In_ JsValueRef object,
    _In_ JsPropertyIdRef propertyIdRef,
    _In_ PropertyDescriptorOptionValues writable,
    _In_ PropertyDescriptorOptionValues enumerable,
    _In_ PropertyDescriptorOptionValues configurable,
    _In_ JsValueRef value,
    _In_ JsValueRef getter,
    _In_ JsValueRef setter)
  {
    JsValueRef descriptor;
    JsErrorCode error;
    error = CreatePropertyDescriptor(writable, enumerable, configurable, value, getter, setter, &descriptor);
    if (error != JsNoError) return error;

    bool result;
    error = JsDefineProperty(object, propertyIdRef, descriptor, &result);

    if (error == JsNoError && !result) return JsErrorInvalidArgument;
    return error;
  }

  JsErrorCode DefineProperty(_In_ JsValueRef object,
    _In_ const wchar_t * propertyName,
    _In_ PropertyDescriptorOptionValues writable,
    _In_ PropertyDescriptorOptionValues enumerable,
    _In_ PropertyDescriptorOptionValues configurable,
    _In_ JsValueRef value,
    _In_ JsValueRef getter,
    _In_ JsValueRef setter)
  {
    JsErrorCode error;
    JsPropertyIdRef propertyIdRef;

    error = JsGetPropertyIdFromName(propertyName, &propertyIdRef);
    if (error != JsNoError) return error;

    error = DefineProperty(object, propertyIdRef, writable, enumerable, configurable, value, getter, setter);
    return error;
  }

  // TODO: cache the property ids in a hash table?
  JsErrorCode GetPropertyIdFromJsString(_In_ JsValueRef stringRef, 
    _Out_ JsPropertyIdRef *idRef)
  {
    JsErrorCode error;
    const wchar_t *propertyName;
    size_t propertyNameSize;

    error = JsStringToPointer(stringRef, &propertyName, &propertyNameSize);
    if (error != JsNoError) return error;

    error = JsGetPropertyIdFromName(propertyName, idRef);
    return error;
  }

  JsErrorCode GetPropertyIdFromValue(_In_ JsValueRef valueRef, 
    _Out_ JsPropertyIdRef *idRef)
  {
    JsErrorCode error;
    JsValueRef stringRef;
    error = JsConvertValueToString(valueRef, &stringRef);

    if (error != JsNoError) return error;

    error = GetPropertyIdFromJsString(stringRef, idRef);
    return error;
  }

  JsErrorCode GetObjectConstructor(_In_ JsValueRef objectRef,
    _Out_ JsValueRef *constructorRef)
  {
    JsErrorCode error;
    JsPropertyIdRef constructorPropertyIdRef;
    error = JsGetPropertyIdFromName(L"constructor", &constructorPropertyIdRef);
    if (error != JsNoError) return error;

    error = JsGetProperty(objectRef, constructorPropertyIdRef, constructorRef);
    return error;
  }

  JsErrorCode SetIndexedProperty(
    _In_ JsValueRef object,
    _In_ int index, 
    _In_ JsValueRef value)
  {
    JsErrorCode error;
    JsValueRef indexRef;
    error = JsIntToNumber(index, &indexRef);
    if (error != JsNoError) return error;

    error = JsSetIndexedProperty(object, indexRef, value);
    return error;
  }

  JsErrorCode GetIndexedProperty(
    _In_ JsValueRef object,
    _In_ int index, 
    _In_ JsValueRef *value)
  {
    JsErrorCode error;
    JsValueRef indexRef;
    error = JsIntToNumber(index, &indexRef);
    if (error != JsNoError) return error;

    error = JsGetIndexedProperty(object, indexRef, value);
    return error;
  }

  JsErrorCode DeleteIndexedProperty(
    _In_ JsValueRef object, 
    _In_ int index)
  {
    JsErrorCode error;
    JsValueRef indexRef;
    error = JsIntToNumber(index, &indexRef);
    if (error != JsNoError) return error;

    error = JsDeleteIndexedProperty(object, indexRef);
    return error;
  }

  JsErrorCode HasProperty(
    _In_ JsValueRef object, 
    _In_ JsValueRef prop,
    _Out_ bool *result)
  {
    JsPropertyIdRef idRef;
    JsErrorCode error;

    size_t strLength;
    const  wchar_t *strPtr;

    error = JsStringToPointer(prop, &strPtr, &strLength);
    if (error != JsNoError) return error;

    error = JsGetPropertyIdFromName(strPtr, &idRef);
    if (error != JsNoError) return error;

    error = JsHasProperty(object, idRef, result);

    return error;
  }

  JsErrorCode HasIndexedProperty(
    _In_ JsValueRef object, 
    _In_ int index, 
    _Out_ bool *result)
  {
    JsErrorCode error;
    JsValueRef indexRef;
    error = JsIntToNumber(index, &indexRef);
    if (error != JsNoError) return error;

    error = JsHasIndexedProperty(object, indexRef, result);
    return error;
  }

  JsErrorCode IsOfGlobalType(
    _In_ JsValueRef objectRef,
    _In_ const wchar_t *typeName,
    _Out_ bool *result
    )
  {
    // the JS equivlant to what we do is: this.constructor.prototype == object.prototype 
    JsErrorCode error;
    JsValueRef valRef;

    error = GetPropertyOfGlobal(typeName, &valRef);
    if (error != JsNoError) return error;

    error = InstanceOf(objectRef, valRef, result);

    return error;
  }

  JsErrorCode SetConstructorName(
    _In_ JsValueRef objectRef,
    _In_ const wchar_t * name)
  {
    JsValueRef stringRef;
    JsErrorCode error = JsPointerToString(name, wcslen(name), &stringRef);
    if (error != JsNoError) return error;

    return SetConstructorName(objectRef, stringRef);
  }

  JsErrorCode SetConstructorName(
    _In_ JsValueRef objectRef,
    _In_ JsValueRef nameStringRef)
  {
    JsErrorCode error;
    JsValueRef constructorObj = JS_INVALID_REFERENCE;

    error = GetProperty(objectRef,L"constructor",&constructorObj);
    if (error != JsNoError) return error;

    if (constructorObj == JS_INVALID_REFERENCE)
    {
      return JsErrorInvalidArgument;
    }

    JsPropertyIdRef namePropertyIdRef;
    error = JsGetPropertyIdFromName(L"name",&namePropertyIdRef);
    if (error != JsNoError) return error;

    error = JsSetProperty(constructorObj, namePropertyIdRef, nameStringRef,false);
    return error;
  }

  // used for debugging

  JsErrorCode StringifyObject(
    _In_ JsValueRef object,
    _Out_ const wchar_t **stringifiedObject)
  {
    JsValueRef jsonObj = JS_INVALID_REFERENCE;
    JsErrorCode error;
    error = GetPropertyOfGlobal(L"JSON",&jsonObj);
    if (error != JsNoError) return error;

    JsValueRef args[] = {jsonObj, object};
    JsValueRef jsonResult;
    error = jsrt::CallProperty(jsonObj, L"stringify", args, _countof(args), &jsonResult);
    if (error != JsNoError) return error;

    size_t stringLength;
    return JsStringToPointer(jsonResult, stringifiedObject, &stringLength);
  }

  JsErrorCode GetConstructorName(
    _In_ JsValueRef objectRef,
    _Out_ const wchar_t **name)
  {
    JsErrorCode error;
    JsValueRef constructorObj = JS_INVALID_REFERENCE;

    error = GetProperty(objectRef,L"constructor",&constructorObj);
    if (error != JsNoError) return error;

    if (constructorObj == JS_INVALID_REFERENCE)
    {
      return JsErrorInvalidArgument;
    }

    JsValueRef stringRef = JS_INVALID_REFERENCE;
    error = GetProperty(constructorObj,L"name",&stringRef);
    if (error != JsNoError) return error;
    if (stringRef == JS_INVALID_REFERENCE)
    {
      return JsErrorInvalidArgument;
    }

    size_t dummyLength;
    error = JsStringToPointer(stringRef, name, &dummyLength);

    return error;
  }
}

