#pragma once

#include "jsrt.h"
#include <functional>

#define IfComFailError(v) \
  { \
  hr = (v) ; \
  if (FAILED(hr)) \
    { \
    goto error; \
    } \
  }

namespace jsrt
{
  JsErrorCode GetProperty(
    _In_ JsValueRef ref, 
    _In_ JsValueRef propName,
    _Out_ JsValueRef *result);

  JsErrorCode GetProperty(
    _In_ JsValueRef ref, 
    _In_ const wchar_t *propertyName,
    _Out_ JsValueRef *result);

  JsErrorCode SetProperty(
    _In_ JsValueRef ref, 
    _In_ JsValueRef propName,
    _In_ JsValueRef propValue);

  JsErrorCode SetProperty(
    _In_ JsValueRef ref, 
    _In_ const wchar_t* propName,
    _In_ JsValueRef propValue);

  JsErrorCode DeleteIndexedProperty(
    _In_ JsValueRef object, 
    _In_ int index);

  JsErrorCode DeleteProperty(
    _In_ JsValueRef ref, 
    _In_ JsValueRef propName,
    _Out_ JsValueRef* result);

  JsErrorCode GetOwnPropertyDescriptor(
    _In_ JsValueRef ref, 
    _In_ JsValueRef prop,
    _Out_ JsValueRef* result);

  JsErrorCode IsValueInArray(
    _In_ JsValueRef arrayRef, 
    _In_ JsValueRef valueRef,
    _Out_ bool* result);

  JsErrorCode IsValueInArray(
    _In_ JsValueRef arrayRef, 
    _In_ JsValueRef valueRef,
    _In_ std::function<JsErrorCode (JsValueRef,JsValueRef, bool*)> comperator,
    _Out_ bool* result);

  JsErrorCode IsCaseInsensitiveStringValueInArray(
    _In_ JsValueRef arrayRef, 
    _In_ JsValueRef valueRef,
    _Out_ bool* result);

  JsErrorCode IsZero(
    _In_ JsValueRef value,
    _Out_ bool *result);

  JsErrorCode IsUndefined(
    _In_ JsValueRef value,
    _Out_ bool *result);

  JsErrorCode HasOwnProperty(
    _In_ JsValueRef object, 
    _In_ JsValueRef prop,
    _Out_ JsValueRef *result);

  JsErrorCode HasProperty(
    _In_ JsValueRef object, 
    _In_ JsValueRef prop,
    _Out_ bool *result);

  JsErrorCode HasIndexedProperty(
    _In_ JsValueRef object, 
    _In_ int index,
    _Out_ bool *result);

  JsErrorCode GetEnumerableNamedProperties(
    _In_ JsValueRef object, 
    _Out_ JsValueRef *result);

  JsErrorCode GetEnumerableIndexedProperties(
    _In_ JsValueRef object, 
    _Out_ JsValueRef *result);

  JsErrorCode GetIndexedOwnKeys(
    _In_ JsValueRef object, 
    _Out_ JsValueRef *result);

  JsErrorCode GetNamedOwnKeys(
    _In_ JsValueRef object, 
    _Out_ JsValueRef *result);

  JsErrorCode CreateEnumerationIterator(
    _In_ JsValueRef enumeration, 
    _Out_ JsValueRef *result);

  JsErrorCode CreatePropertyDescriptorsEnumerationIterator(
    _In_ JsValueRef enumeration, 
    _Out_ JsValueRef *result);

  JsErrorCode ConcatArray(
    _In_ JsValueRef first, 
    _In_ JsValueRef second, 
    _Out_ JsValueRef *result
    );

  JsErrorCode CallProperty(
    _In_ JsValueRef ref, 
    _In_ const wchar_t *propertyName, 
    _In_reads_(argumentCount) JsValueRef *arguments,
    _In_ unsigned short argumentCount,
    _Out_ JsValueRef *result);

  JsErrorCode GetGlobalType(
    _In_ const wchar_t* typeName,
    _Out_ JsRef *ref); 

  JsErrorCode GetProtoypeOfGlobalType(
    _In_ const wchar_t* typeName,
    _Out_ JsRef *ref);

  JsErrorCode GetPropertyOfGlobalType(
    _In_ const wchar_t* typeName,
    _In_ const wchar_t* propertyName,
    _Out_ JsRef *ref);

  JsErrorCode GetPropertyOfGlobal(
    _In_ const wchar_t *propertyName, 
    _Out_ JsValueRef *ref);


  JsErrorCode SetPropertyOfGlobal(
    _In_ const wchar_t *propertyName, 
    _In_ JsValueRef ref);

  JsValueRef GetUndefined();

  JsValueRef GetTrue();

  JsValueRef GetFalse();

  // NOT IMPELEMENTED
  JsErrorCode SetException(
    _In_ const wchar_t* message, 
    _In_ const wchar_t* stackTrace);

  JsErrorCode GetArrayLength(
    _In_ JsValueRef arrayRef, 
    _Out_ unsigned int *arraySize);

  JsErrorCode InstanceOf(
    _In_ JsValueRef first, 
    _In_ JsValueRef second,
    _Out_ bool *result);

  JsErrorCode InstanceOfGlobalType(
    _In_ JsValueRef first,
    _In_ const wchar_t* typeName,
    _Out_ bool *result);

  JsErrorCode CloneObject(
    _In_ JsValueRef source, 
    _In_ JsValueRef target,
    _In_ bool cloneProtoype = false);

  JsErrorCode ConcatArray(
    _In_ JsValueRef first, 
    _In_ JsValueRef second, 
    _Out_ JsValueRef *result
    );

  JsErrorCode GetPropertyNames(
    _In_ JsValueRef object, 
    _Out_ JsValueRef *namesArray);

  JsErrorCode AddExternalData(
    _In_ JsValueRef ref,
    _In_ const wchar_t *externalDataName,
    _In_ void *data,
    _In_ JsFinalizeCallback onObjectFinalize);

  JsErrorCode AddExternalData(
    _In_ JsValueRef ref,
    _In_ JsPropertyIdRef externalDataPropertyId,
    _In_ void *data,
    _In_ JsFinalizeCallback onObjectFinalize);

  JsErrorCode AddExternalData(
    _In_ JsValueRef ref,
    _In_ void *data,
    _In_ JsFinalizeCallback onObjectFinalize);

  JsErrorCode GetExternalDataDefaultName(
    _Out_ const wchar_t **defaultName);

  JsErrorCode HasExternalData(
    _In_ JsValueRef ref,
    _In_ const wchar_t *externalDataName,
    _Out_ bool *result);

  JsErrorCode HasExternalData(
    _In_ JsValueRef ref, 
    _Out_ bool *result);

  JsErrorCode HasExternalData(
    _In_ JsValueRef ref,
    JsPropertyIdRef propertyId,
    _Out_ bool *result);

  JsErrorCode GetExternalData(
    _In_ JsValueRef ref,
    _In_ JsPropertyIdRef idRef,
    _Out_ void **data);

  JsErrorCode GetExternalData(
    _In_ JsValueRef ref,
    _In_ const wchar_t *externalDataName,
    _Out_ void **data);

  JsErrorCode GetExternalData(
    _In_ JsValueRef ref,
    _Out_ void **data);

  JsErrorCode CreateFunctionWithExternalData(
    _In_ JsNativeFunction, 
    void* data,
    _In_ JsFinalizeCallback onObjectFinalize,
    _Out_ JsValueRef *function);

  const wchar_t* DebugToString(_In_ JsValueRef ref);

  JsErrorCode ToString(
    _In_ JsValueRef ref,
    _Out_ const wchar_t** str,
    _In_ bool alreadyString = false);

  JsErrorCode GetExceptionMessage(_Out_ const wchar_t** str);

  // the possible values for the property descriptor options
  enum PropertyDescriptorOptionValues
  {
    True,
    False,
    None
  };

  JsErrorCode CreatePropertyDescriptor(
    _In_ PropertyDescriptorOptionValues writable,
    _In_ PropertyDescriptorOptionValues enumerable,
    _In_ PropertyDescriptorOptionValues configurable,
    _In_ JsValueRef value,
    _In_ JsValueRef getter,
    _In_ JsValueRef setter,
    _Out_ JsValueRef *descriptor);

  JsErrorCode DefineProperty(
    _In_ JsValueRef object,
    _In_ const wchar_t * propertyName,
    _In_ PropertyDescriptorOptionValues writable,
    _In_ PropertyDescriptorOptionValues enumerable,
    _In_ PropertyDescriptorOptionValues configurable,
    _In_ JsValueRef value,
    _In_ JsValueRef getter,
    _In_ JsValueRef setter);

  JsErrorCode DefineProperty(
    _In_ JsValueRef object,
    _In_ JsPropertyIdRef propertyIdRef,
    _In_ PropertyDescriptorOptionValues writable,
    _In_ PropertyDescriptorOptionValues enumerable,
    _In_ PropertyDescriptorOptionValues configurable,
    _In_ JsValueRef value,
    _In_ JsValueRef getter,
    _In_ JsValueRef setter);

  JsErrorCode GetPropertyIdFromJsString(
    _In_ JsValueRef stringRef, 
    _Out_ JsPropertyIdRef *idRef);

  JsErrorCode GetPropertyIdFromValue(
    _In_ JsValueRef valueRef, 
    _Out_ JsPropertyIdRef *idRef);

  JsErrorCode GetObjectConstructor(
    _In_ JsValueRef objectRef,
    _Out_ JsValueRef *constructorRef);

  JsErrorCode SetIndexedProperty(
    _In_ JsValueRef object,
    _In_ int index, 
    _In_ JsValueRef value);

  JsErrorCode GetIndexedProperty(
    _In_ JsValueRef object,
    _In_ int index, 
    _In_ JsValueRef *value);

  JsErrorCode IsOfGlobalType(
    _In_ JsValueRef objectRef,
    _In_ const wchar_t *typeName,
    _Out_ bool *result);

  JsErrorCode SetConstructorName(
    _In_ JsValueRef objectRef,
    _In_ JsValueRef nameStringRef);

  JsErrorCode SetConstructorName(
    _In_ JsValueRef objectRef,
    _In_ const wchar_t * name);

  // used for debugging
  JsErrorCode StringifyObject(
    _In_ JsValueRef object,
    _Out_ const wchar_t **stringifiedObject);

  JsErrorCode GetConstructorName(
    _In_ JsValueRef objectRef,
    _Out_ const wchar_t **name);


}
