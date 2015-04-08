#include <memory>
#include "v8.h"
#include "jsrt.h"
#include "jsrtutils.h"

namespace v8
{
  using std::unique_ptr;

  __declspec(thread) JsValueRef concatFunction = JS_INVALID_REFERENCE;
  __declspec(thread) JsValueRef newStringObjectFunction = JS_INVALID_REFERENCE;

  // Internal class for converting strings to Wide Chars and vice versa
  class Convert
  {
  public:
    static JsErrorCode ToChar(const wchar_t *str, const size_t length, char* buffer, const size_t size)
    {
      return InternalToChar(str, length, CP_ACP, buffer, size);
    }

    static JsErrorCode ToUTF8Char(const wchar_t *str, const size_t length, char* buffer, const size_t size, __out size_t *bytesWritten = nullptr, __out size_t *charsWrittern = nullptr)
    {
      return InternalToChar(str, length, CP_UTF8, buffer, size, bytesWritten, charsWrittern);
    }

    static JsErrorCode ToWChar(const char *str, const size_t length, wchar_t* buffer, const size_t size, __out size_t *charsWritten)
    {
      return InternalToWChar(str, length, CP_UTF8, buffer, size, charsWritten);
    }

    static JsErrorCode UTF8CharLength(const wchar_t *str, const size_t length, __out size_t *utf8Length)
    {
      return GetCharLength(str, length, CP_UTF8, utf8Length);
    }

    template <class SrcChar, class DstChar>
    static JsErrorCode CopyRaw(const SrcChar* src, size_t length, DstChar* dst, size_t size, __out size_t *charsWrittern = nullptr)
    {
      size_t count = min(length, size);
      InternalCopyRaw(src, dst, count);
      if (charsWrittern != nullptr)
      {
        *charsWrittern = count;
      }
      return JsNoError;
    }

  private:
    template <class SrcChar, class DstChar>
    static DstChar CastRaw(SrcChar ch)
    {
      return static_cast<DstChar>(ch);
    }

    template <>
    static wchar_t CastRaw<char, wchar_t>(char ch)
    {
      return static_cast<wchar_t>(static_cast<uint8_t>(ch));
    }

    template <class SrcChar, class DstChar>
    static void InternalCopyRaw(const SrcChar* src, DstChar* dst, size_t count)
    {
      const SrcChar* end = src + count;
      while (src < end)
      {
        *dst++ = CastRaw<SrcChar, DstChar>(*src++);
      }
    }

    template <>
    static void InternalCopyRaw<wchar_t, wchar_t>(const wchar_t* src, wchar_t* dst, size_t count)
    {
      wmemcpy_s(dst, count, src, count);
    }

    template <>
    static void InternalCopyRaw<char, char>(const char* src, char* dst, size_t count)
    {
      memcpy_s(dst, count, src, count);
    }

    static JsErrorCode GetCharLength(const wchar_t *str, const size_t length, const int code, __out size_t *utf8Length)
    {
      *utf8Length = 0;

      // Compute the size of the required buffer
      DWORD size = WideCharToMultiByte(code,
        0,
        str,
        static_cast<int>(length),
        NULL,
        0,
        NULL,
        NULL);
      if (size == 0) 
      {
        return JsErrorFatal;
      }

      *utf8Length = size;

      return JsNoError;
    }

    static JsErrorCode InternalToChar(const wchar_t *str, const size_t length, const int code, char* buffer, size_t bufferSize, __out size_t *bytesWritten = nullptr, __out size_t *charsWritten = nullptr)
    {			
      if (bytesWritten != nullptr)
      {
        *bytesWritten = 0;
      }

      // Compute the size of the required buffer
      size_t size;
      auto getCharLengthResult = GetCharLength(str, length, code, &size);

      if (getCharLengthResult != JsNoError) 
      {
        return getCharLengthResult;
      }

      if (bufferSize == -1)
      {
        bufferSize = size;
      }

      size_t strLengthToWrite = length;
      // buffer size is insuffecient
      if (bufferSize < size)
      {
        // in the horrible situation in which the buffer size isn't sufficient we mimic the behavior of v8 and find the maximal
        // number of characters that we can fit in the given buffer
        // in order to do so, the only thing that we can do is try and remove characters from the string and call WideCharToMultiByte each time
        // unfortenately, WideCharToMultiByte will fail if the buffer size is insufficient, so this is the only way to do this
        size_t requiredBufferSize = size;
        for (strLengthToWrite = length-1; strLengthToWrite > 0;  strLengthToWrite--)
        {
          auto getCharLengthResult = GetCharLength(str, strLengthToWrite, code, &requiredBufferSize);

          if (getCharLengthResult != JsNoError) 
          {
            return getCharLengthResult;
          }

          if (requiredBufferSize <= bufferSize)
          {
            break;
          }
        }
      }

      // if nothing could be written, just leave
      if (strLengthToWrite == 0)
      {
        if (bytesWritten != nullptr)
        {
          *bytesWritten = 0;
        }

        if (charsWritten != nullptr)
        {
          *charsWritten = 0;
        }

        return JsNoError;
      }

      // Do the actual conversion
      DWORD result = WideCharToMultiByte(code,
        0,
        str,
        static_cast<int>(strLengthToWrite),
        buffer,
        static_cast<int>(bufferSize),
        NULL,
        NULL);

      if (result == 0) 
      {
        DWORD conversionError = GetLastError();
        if (conversionError != ERROR_INSUFFICIENT_BUFFER)
        {
          // This should never happen.
          return JsErrorFatal;
        }
      }

      if (bytesWritten != nullptr)
      {
        *bytesWritten = result;
      }

      if (charsWritten != nullptr)
      {
        *charsWritten = strLengthToWrite;
      }

      return JsNoError;
    }

    static JsErrorCode InternalToWChar(const char *str, const size_t length, const int code, wchar_t* buffer, size_t size, __out size_t *charsWritten) 
    { 
      // 
      // Special case of empty input string 
      // 
      if (str == nullptr || length == 0) 
      {
        return JsNoError;
      }

      // 
      // Get length (in wchar_t's) of resulting UTF-16 string 
      // 
      const int utf16Length = ::MultiByteToWideChar( 
        code,				// convert the given code 
        0,    // flags 
        str,				// source string 
        static_cast<int>(length),				// length (in chars) of source string 
        NULL,               // unused - no conversion done in this step 
        0                   // request size of destination buffer, in wchar_t's 
        );

      if (utf16Length == 0) 
      { 
        // Error 
        return JsErrorFatal;				
      } 

      if (size == -1)
      {
        size = utf16Length;
      }

      // 
      // Do the conversion from code to UTF-16 
      // 
      *charsWritten = ::MultiByteToWideChar( 
        code,			// convert from code 
        0,              // validation was done in previous call,  
        // so speed up things with default flags 
        str,            // source string 
        static_cast<int>(length),			// length (in chars) of source string 
        buffer,			// destination buffer 
        static_cast<int>(size)			// size of destination buffer, in wchar_t's 
        );

      if (*charsWritten == 0)
      { 
        // Error
        return JsErrorFatal;
      }

      // 
      // Return resulting UTF-16 string 
      // 
      return JsNoError; 
    } 
  };

  String::AsciiValue::AsciiValue(Handle<v8::Value> obj) : _str(nullptr), _length(0)
  {
    Handle<String> str = obj->ToString();
    if (str.IsEmpty())
    {
      return;
    }

    _length = str->Utf8Length();
    _str = new char[_length + 1];
    str->WriteUtf8(_str);
  }

  String::AsciiValue::~AsciiValue()
  {
    if (_str != nullptr) delete _str;
  }

  String::Utf8Value::Utf8Value(Handle<v8::Value> obj) : _str(nullptr), _length(0)
  {
    Handle<String> str = obj->ToString();
    if (str.IsEmpty())
    {
      return;
    }

    _length = str->Utf8Length();
    _str = new char[_length + 1];
    str->WriteUtf8(_str);
  }

  String::Utf8Value::~Utf8Value()
  {
    if (_str != nullptr) delete _str;
  }

  String::Value::Value(Handle<v8::Value> obj) : _str(nullptr), _length(0)
  {
    Handle<String> str = obj->ToString();
    if (str.IsEmpty())
    {
      return;
    }

    _length = str->Length();
    _str = new uint16_t[_length + 1];
    str->Write(_str);
  }

  String::Value::~Value()
  {
    if (_str != nullptr) delete _str;
  }

  int String::Length() const 
  {
    int length = 0;

    if (JsGetStringLength((JsValueRef)this, &length) != JsNoError) 
    {
      return 0;
    }

    return length; 
  }

  int String::Utf8Length() const 
  { 
    const wchar_t* str;
    size_t stringLength;
    if (JsStringToPointer((JsValueRef)this, &str, &stringLength) != JsNoError)
    {
      // error
      return 0;
    }

    size_t utf8Length;
    auto result = Convert::UTF8CharLength(str, stringLength, &utf8Length);
    if (result != JsNoError)
    {
      return 0;
    }

    return static_cast<int>(utf8Length);
  }

  template <class CharType>
  static int WriteRaw(JsValueRef ref, CharType* buffer, int start, int length, int options)
  {
    const wchar_t* str;
    if (length == 0)
    {
      // bail out if we are required to write no chars
      return 0;
    }

    size_t stringLength;
    if (JsStringToPointer(ref, &str, &stringLength) != JsNoError)
    {
      // error
      return 0;
    }

    if (stringLength == 0)
    {
      if (!(options & String::NO_NULL_TERMINATION))
      {
        // include the null terminate
        buffer[0] = L'\0';
      }
      return 0;
    }

    if (start < 0 || start > static_cast<int>(stringLength))
    {
      // illegal bail out
      return 0;
    }

    // in case length was not provided we want to copy the whole string
    int count = length >= 0 ? length : static_cast<int>(stringLength);
    count = min(count, static_cast<int>(stringLength) - start);

    if (length < 0)
    {
      // If length was not provided, assume enough space to hold content and null terminator.
      length = count + 1;
    }

    Convert::CopyRaw(str + start, count, buffer, length);

    if (count < length && !(options & String::NO_NULL_TERMINATION))
    {
      // include the null terminate
      buffer[count++] = L'\0';
    }

    return count;
  }

  int String::Write(uint16_t *buffer, int start, int length, int options) const 
  {
    return WriteRaw((JsValueRef)this, reinterpret_cast<wchar_t*>(buffer), start, length, options);
  }

  int String::WriteAscii(char *buffer, int start, int length, int options) const 
  {
    if (length == 0)
    {
      // bail out if we are required to write no chars
      return 0;
    }

    if (start < 0)
    {
      // illegal bail out
      return 0;
    }

    const wchar_t* str;
    size_t stringLength;
    if (JsStringToPointer((JsValueRef)this, &str, &stringLength) != JsNoError)
    {
      // error
      return 0;
    }

    if (stringLength == 0)
    {
      if (!(options & String::NO_NULL_TERMINATION))
      {
        buffer[0] = '\0';
      }
      // bail out if string is empty
      return 0;
    }

    if (length < 0)
    {
      // in case length was not provided we want to copy the whole string
      length = static_cast<int>(stringLength);
    }
    unsigned int count = (length + start) < static_cast<int>(stringLength) ? length : static_cast<int>(stringLength) - start;

    wchar_t* mutableStr = const_cast<wchar_t*>(str);

    if (start > 0)
    {
      // pointer arithmatic to start the copying from the 'start' parameter
      mutableStr += start;
    }

    JsErrorCode convertResult = Convert::ToChar(mutableStr, count, buffer, count);
    if (convertResult != JsNoError)
    {
      return 0;
    }

    if (!(options & String::NO_NULL_TERMINATION))
    {
      buffer[count++] = '\0';
    }

    return count; 
  }

  int String::WriteOneByte(uint8_t* buffer, int start, int length, int options) const
  {
    return WriteRaw((JsValueRef)this, reinterpret_cast<char*>(buffer), start, length, options);
  }

  int String::WriteUtf8(char *buffer, int length, int *nchars_ref, int options) const 
  { 
    if (length == 0)
    {
      // bail out if we are required to write no chars
      return 0;
    }

    const wchar_t* str;
    size_t stringLength;
    if (JsStringToPointer((JsValueRef)this, &str, &stringLength) != JsNoError)
    {
      // error
      return 0;
    }

    if (stringLength == 0)
    {
      // bail out if string is empty

      if (!(options & String::NO_NULL_TERMINATION))
      {
        buffer[0] = '\0';
      }

      return 0;
    }

    int originalLength = length;
    if (length < 0)
    {
      // in case length was not provided we want to copy the whole string
      length = static_cast<int>(stringLength);
    }
    unsigned int count = length < static_cast<int>(stringLength) ? length : static_cast<unsigned int>(stringLength);

    size_t charsCount = 0;
    size_t size = 0;
    // we pass -1 as the buffer size to imply that the buffer is big enough
    JsErrorCode convertResult = Convert::ToUTF8Char(str, count, buffer, originalLength, &size, &charsCount);

    if (convertResult != JsNoError)
    {
      return 0;
    }

    if (!(options & String::NO_NULL_TERMINATION))
    {
      buffer[size++] = '\0';
      count++;
      // TODO: @saary - should we increase this?
      //charsCount++;
    }

    if (nchars_ref != nullptr)
    {
      *nchars_ref = static_cast<int>(charsCount);
    }

    return static_cast<int>(size); 
  }

  Local<String> String::Empty(Isolate* isolate)
  {
    return String::New(L"", 0);
  }

  String* String::Cast(v8::Value *obj) 
  { 
    if (!obj->IsString())
    {
      return nullptr; // TODO: What is the best behavior here? Should we return a pointer to undefined/null instead?
    }

    return static_cast<String*>(obj);
  }

  template <class ToWide>
  Local<String> String::New(const ToWide& toWide, const char *data, int length)
  {
    if (length < 0)
    {
      length = static_cast<int>(strlen(data));
    }

    if (length == 0)
    {
      return Empty();
    }

    unique_ptr<wchar_t[]> str(new wchar_t[length]);
    size_t charsWritten;
    if (toWide(data, length, str.get(), length, &charsWritten) != JsNoError)
    {
      return Local<String>();
    }

    return New(str.get(), static_cast<int>(charsWritten));
  }

  Local<String> String::New(const wchar_t *data, int length)
  {
    if (length < 0)
    {
      length = static_cast<int>(wcslen(data));
    }

    JsValueRef strRef;
    if (JsPointerToString(data, length, &strRef) != JsNoError)
    {
      return Local<String>();
    }

    return Local<String>::New((String*)strRef);
  }

  Local<String> String::New(const uint16_t *data, int length) 
  { 
    return New((wchar_t*)data, length);
  }

  Local<String> String::NewSymbol(const char *data, int length) 
  { 
    return New(Convert::ToWChar, data, length);
  }

  Local<String> String::NewSymbol(const wchar_t *data, int length) 
  { 
    return New(data, length); 
  }

  bool LazyInitConcat()
  {
    if (concatFunction == JS_INVALID_REFERENCE)
    {
      JsRef stringProto;
      if (jsrt::GetPropertyOfGlobalType(L"String", L"prototype", &stringProto) != JsNoError)
      {
        return false;
      }

      if (jsrt::GetProperty(stringProto, L"concat", &concatFunction) != JsNoError)
      {
        return false;
      }
    }

    return true;
  }

  Local<String> String::Concat(Handle<String> left, Handle<String> right) 
  { 
    if (LazyInitConcat())
    {
      JsValueRef args[] = { *left, *right };
      JsValueRef result;

      JsErrorCode concatResult = JsCallFunction(concatFunction, args, _countof(args), &result);
      if (concatResult != JsNoError)
      {
        return Local<String>(); 
      }

      return Local<String>::New((String*)result);
    }

    return Local<String>(); 
  }

  Local<String> String::NewExternal(Isolate* isolate, ExternalStringResource* resource)
  {
    if (resource->data() != nullptr)
    {
      auto newStr = New(resource->data(), static_cast<int>(resource->length()));
      delete resource;

      return newStr;
    }

    // otherwise the resource is empty just delete it and return an empty string
    delete resource;
    return Empty();
  }

  Local<String> String::NewExternal(Isolate* isolate, ExternalAsciiStringResource *resource)
  {
    if (resource->data() != nullptr)
    {
      auto newStr = New(Convert::ToWChar, resource->data(), static_cast<int>(resource->length()));
      delete resource;

      return newStr;
    }

    // otherwise the resource is empty just delete it and return an empty string
    delete resource;
    return Empty();
  }

  Local<String> String::NewFromUtf8(
    Isolate* isolate,
    const char* data,
    NewStringType type,
    int length)
  {
    return New(Convert::ToWChar, data, length);
  }

  Local<String> String::NewFromOneByte(
    Isolate* isolate,
    const uint8_t* data,
    NewStringType type,
    int length)
  {
    return New(Convert::CopyRaw<char, wchar_t>, reinterpret_cast<const char*>(data), length);
  }

  Local<String> String::NewFromTwoByte(
    Isolate* isolate,
    const uint16_t* data,
    NewStringType type,
    int length)
  {
    return New(data, length);
  }

}
