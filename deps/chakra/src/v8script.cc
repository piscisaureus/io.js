#include "v8.h"
#include "jsrtutils.h"

#include <memory>

namespace v8
{
  __declspec(thread) JsSourceContext currentContext;

  Local<Script> Script::Compile(Handle<String> source, ScriptOrigin* origin)
  {
    return Compile(source, origin->ResourceName());
  }

  // Compiled script object, bound to the context that was active when this function was called. When run it will always use this context.
  Local<Script> Script::Compile(Handle<String> source, Handle<String> file_name)
  { 
    // TODO how to bound this script to a context?
    HandleScope scoope;

    const wchar_t* filename;
    if (jsrt::ToString(*file_name, &filename) != JsNoError)
    {
      return Local<Script>();
    }

    const wchar_t *script;
    if (jsrt::ToString(*source, &script) != JsNoError)
    {
      return Local<Script>();
    }

    JsValueRef scriptFunction;
    if (JsParseScript(script, currentContext++, filename, &scriptFunction) != JsNoError)
    {
      return Local<Script>();
    }

    return Local<Script>::New((Script *)scriptFunction);
  }

  Local<Value> Script::Run() 
  {
    JsValueRef result;
    if (JsCallFunction(this, nullptr, 0, &result) != JsNoError)
    {
      return Local<Value>();
    }

    return Local<Value>::New((Value *) result);
  }

  Local<UnboundScript> Script::GetUnboundScript()
  {
    //TODO: Chakra doesn't support unbound script
    return Local<UnboundScript>((UnboundScript*)this);
  }

  Local<Script> UnboundScript::BindToCurrentContext()
  {
    //TODO: Chakra doesn't support unbound script
    return Local<Script>((Script*)this);
  }

  Local<UnboundScript> ScriptCompiler::CompileUnbound(Isolate* isolate, Source* source, CompileOptions options)
  {
    //TODO: Chakra doesn't support unbound script
    return Compile(isolate, source, options)->GetUnboundScript();
  }

  Local<Script> ScriptCompiler::Compile(Isolate* isolate, Source* source, CompileOptions options)
  {
    //TODO: Chakra doesn't support unbound script
    return Script::Compile(source->source_string, source->resource_name);
  }

}
