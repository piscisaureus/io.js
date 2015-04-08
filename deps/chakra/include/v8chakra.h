// Internal header for the v8-chakra bridge

#include <jsrt.h>

namespace v8
{
  struct ObjectTemplateData;

  extern __declspec(thread) JsRuntimeHandle runtime;
  extern __declspec(thread) bool g_EnableDebug;

  struct ObjectData
  {
  public:
    NamedPropertyGetterCallback namedPropertyGetter;
    NamedPropertySetterCallback namedPropertySetter;
    NamedPropertyQueryCallback namedPropertyQuery;
    NamedPropertyDeleterCallback namedPropertyDeleter;
    NamedPropertyEnumeratorCallback namedPropertyEnumerator;
    Persistent<Value> namedPropertyInterceptorData;
    IndexedPropertyGetterCallback indexedPropertyGetter;
    IndexedPropertySetterCallback indexedPropertySetter;
    IndexedPropertyQueryCallback indexedPropertyQuery;
    IndexedPropertyDeleterCallback indexedPropertyDeleter;
    IndexedPropertyEnumeratorCallback indexedPropertyEnumerator;
    Persistent<Value> indexedPropertyInterceptorData;
    int internalFieldCount;
    void **internalFields;

    ObjectData(ObjectTemplateData *templateData);
  };
}
