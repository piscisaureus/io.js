#pragma once
#include <v8.h>

namespace v8
{
  // NOT IMPLEMENTED
  class EXPORT Debug
  {
  public:
    class ClientData
    {
    };

    class Message
    {
    public:
      virtual ~Message() {}
      virtual Handle<String> GetJSON() const = 0;
      virtual Isolate* GetIsolate() const = 0;
    };

    typedef void (*DebugMessageDispatchHandler)();
    typedef void(*MessageHandler)(const Message& message);

    static void DebugBreak(Isolate *isolate = NULL) {}
    static void SetDebugMessageDispatchHandler(DebugMessageDispatchHandler handler, bool provide_locker = false) {}
    static bool EnableAgent(const char *name = NULL, int port = 0, bool wait_for_connection = false);
    static void Dispose();
    static void DisableAgent() {}
    static bool IsAgentEnabled();
    static void ProcessDebugMessages() {}
    static Local<Context> GetDebugContext() { return Context::GetCurrent(); }
    static void SetMessageHandler(MessageHandler handler) {}
    static void SendCommand(Isolate* isolate,
      const uint16_t* command, int length,
      ClientData* client_data = NULL) {}
  };
}
