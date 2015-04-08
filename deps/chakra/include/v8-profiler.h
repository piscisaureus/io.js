#include <v8.h>

namespace v8
{
  // NOT IMPLEMENTED
  class EXPORT CpuProfiler
  {
  public:
    //void SetSamplingInterval(int us);
    //void StartProfiling(Handle<String> title, bool record_samples = false);
    //CpuProfile* StopProfiling(Handle<String> title);
    void SetIdle(bool is_idle) {}
  };

  // NOT IMPLEMENTED
  class EXPORT HeapProfiler
  {
  public:
    typedef RetainedObjectInfo *(*WrapperInfoCallback)(uint16_t class_id, Handle<Value> wrapper);
    void SetWrapperClassInfoProvider(uint16_t class_id, WrapperInfoCallback callback) {}
  };

  // NOT IMPLEMENTED
  class EXPORT RetainedObjectInfo
  {
  public:
    virtual void Dispose() = 0;
    virtual bool IsEquivalent(RetainedObjectInfo *other) = 0;
    virtual intptr_t GetHash() = 0;
    virtual const char *GetLabel() = 0;
    virtual const char *GetGroupLabel() { return nullptr; }
    virtual intptr_t GetElementCount() { return 0; }
    virtual intptr_t GetSizeInBytes() { return 0; }
  };
}
