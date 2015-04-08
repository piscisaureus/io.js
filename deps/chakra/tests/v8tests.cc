#include "v8.h"
#include <assert.h>
#include "jsrtutils.h"
#include "jsrt.h"

using namespace v8;
using namespace jsrt;

bool testBoolean()
{
	HandleScope scope;
	TryCatch tryCatch;
	Handle<Boolean> falseHandle = Boolean::New(false);
	assert(falseHandle->Equals(False()));
	assert(!falseHandle->Equals(True()));
	assert(falseHandle->IsBoolean());
	assert(falseHandle->IsFalse());
	assert(!falseHandle->IsTrue());

	Handle<Boolean> trueHandle = Boolean::New(true);
	assert(!trueHandle->Equals(False()));
	assert(trueHandle->Equals(True()));
	assert(!trueHandle->IsFalse());
	assert(trueHandle->IsTrue());
	
	Handle<Boolean> secondTrueHandle = Boolean::New(true);
	assert(secondTrueHandle->Equals(trueHandle));

	assert(!tryCatch.HasCaught());

	return true;
}

bool testNumber()
{
	HandleScope scope;
	TryCatch tryCatch;
	Handle<Number> numHandle = Number::New(2.3);

	assert(numHandle->NumberValue() == 2.3);
	Handle<Number> numHandle2 = Number::New(4.5);
	assert(!numHandle->Equals(numHandle2));

	Handle<Number> numHandle3 = Number::New(2.3);
	assert(numHandle->Equals(numHandle3));
	assert(numHandle->IsNumber());

	assert(!tryCatch.HasCaught());

	return true;
}

bool testInt()
{
	HandleScope scope;
	TryCatch tryCatch;
	Handle<Integer> numHandle = Integer::New(5);

	assert(numHandle->IntegerValue() == 5);
	Handle<Integer> numHandle2 = Integer::New(1);
	assert(!numHandle->Equals(numHandle2));

	Handle<Integer> numHandle3 = Integer::New(5);
	
	assert(numHandle->Equals(numHandle3));
	Handle<Number> numHandle4 = Number::New(4.2);
	assert(!numHandle4->IsInt32());
	assert(!numHandle4->IsUint32());
	assert(numHandle->IsInt32());
	assert(numHandle->IsUint32());

	Handle<Integer> numHandle5 = Integer::New(-3);
	assert(numHandle5->IsInt32());
	assert(!numHandle5->IsUint32());

	assert(!tryCatch.HasCaught());

	return true;
}



bool testString()
{
	HandleScope scope;

	// create a string for unicode chars
	auto wcval = L"???????????????????????????";
	auto str = String::New(wcval);
	auto length = str->Length();
	auto utf8len = str->Utf8Length();

	assert(utf8len > length);
	assert(length == wcslen(wcval));

	// write the string as a UTF8 string to a buffer with a null termination
	char utf8buffer[500] = {0};
	// fill the buffer with chars so we can detect the null termination was placed correctly
	memset(utf8buffer, 'a', 500);
	str->WriteUtf8(utf8buffer);
	auto utf8bufferLength = strlen(utf8buffer);
	assert(utf8bufferLength == utf8len);

	// create a string from regular ascii chars
	auto mbval = "bla bla bla";
	auto asciiString = String::New(mbval);
	char buffer[500] = {0};
	// fill the buffer with chars so we can detect the null termination was placed correctly
	memset(buffer, 'a', 500);
	auto bytesWritten = asciiString->WriteAscii(buffer);
	length = asciiString->Length();
	assert(length == strlen(mbval));
	assert(bytesWritten == length + 1);

	auto bufferLength = strlen(buffer);
	assert(bufferLength == length);

	// fill the buffer with chars so we can detect the null termination was placed correctly
	memset(buffer, 'a', 500);
	bytesWritten = asciiString->WriteAscii(buffer, 0, -1, String::NO_NULL_TERMINATION);
	length = asciiString->Length();
	assert(bytesWritten == length);

	// check wchar writing
	uint16_t wbuffer[500];
	// fill the buffer with chars so we can detect the null termination was placed correctly
	memset(wbuffer, 'a', 500*sizeof(wchar_t));
	auto len = str->Write(wbuffer);
	wchar_t* wstr = reinterpret_cast<wchar_t*>(wbuffer);
	assert(wcslen(wstr) == str->Length());

	// test Write without null termination
	// fill the buffer with chars so we can detect the null termination was placed correctly
	memset(wbuffer, 'a', 500*sizeof(wchar_t));
	len = str->Write(wbuffer, 0, -1, String::NO_NULL_TERMINATION);
	assert(len == str->Length());

	// test partial string writing
	// fill the buffer with chars so we can detect the null termination was placed correctly
	memset(buffer, 'a', 500);
	bytesWritten = asciiString->WriteAscii(buffer, 3);
	length = asciiString->Length();
	assert(length - 3 == strlen(buffer));

	// test partial wchar string writing
	// fill the buffer with chars so we can detect the null termination was placed correctly
	memset(wbuffer, 'a', 500*sizeof(wchar_t));
	bytesWritten = str->Write(wbuffer, 3);
	length = str->Length();
	wstr = reinterpret_cast<wchar_t*>(wbuffer);
	assert(length - 3 == wcslen(wstr));

	// test small length write
	memset(buffer, 'a', 500);
	bytesWritten = asciiString->WriteAscii(buffer, 0, 3);
	assert(3 == strlen(buffer));

	// test empty string
	auto empty = String::Empty();
	assert(empty->Length() == 0);

	// test AsciiValue
	String::AsciiValue asciiValue(asciiString);
	assert(strcmp(mbval, *asciiValue) == 0);

	// test Value
	String::Value value(str);
	assert(wcscmp(wcval, (wchar_t*)*value) == 0);

	// test Value
	String::Utf8Value utf8value(str);
	assert(strcmp(utf8buffer, *utf8value) == 0);

	// test concat
	String::AsciiValue concatValue(String::Concat(String::New(L"a"), String::New(L"b")));
	assert(strcmp("ab", *concatValue) == 0);

	return true;
}

class SimpleExternal : public String::ExternalAsciiStringResource
{
 public:
  SimpleExternal(const char *src, size_t src_len)
      : buffer_(src),
        buf_len_(src_len) {
  }

  ~SimpleExternal() {
  }

  const char *data() const {
      return buffer_;
  }

  size_t length() const {
      return buf_len_;
  }

 private:
  const char *buffer_;
  size_t buf_len_;
};


bool testExternalString()
{
	HandleScope scope;

	char str[] = "this is an external string";
	auto simpleExternal = new SimpleExternal(str, _countof(str));
	Handle<String> extString  = String::NewExternal(simpleExternal);
	assert(extString->Length() == _countof(str));

	return true;
}


bool testScript()
{
	HandleScope scope;

	char str[] = "(function() { this.one = function() { return 1;}; return one(); }).call(this)";
	auto simpleExternal = new SimpleExternal(str, _countof(str));
	Handle<String> source  = String::NewExternal(simpleExternal);
	
	auto script = Script::New(source, String::New(L"one.js"));

	auto result = script->Run();
	auto uno = Integer::New(1);
	Context::GetCurrent()->Global()->Set(String::NewSymbol("uno"), uno);
	assert(uno->Equals(result));

	script = Script::New(String::New(L"one()"), String::New(L"call_one.js"));
	result = script->Run();
	assert(uno->Equals(result));

	script = Script::New(String::New(L"(function() { return uno; })()"), String::New(L"get_uno.js"));
	result = script->Run();
	assert(uno->Equals(result));

	return true;
}

bool testDate()
{
	HandleScope scope;
	double dateVal = 1371749582999;
	const wchar_t *dateString = L"Thu, 20 Jun 2013 17:33:02 GMT";
	Local<Value> date = Date::New(dateVal);
	assert(date->IsDate());
	JsValueRef stringRef;
	JsValueRef dateRef = (JsValueRef)*date;
	JsErrorCode error = CallProperty(dateRef, L"toGMTString", &dateRef, 1, &stringRef);
	assert(error == JsNoError);
	const wchar_t *str;
	size_t size;
	assert(JsStringToPointer(stringRef, &str, &size) == JsNoError);

	assert(lstrcmpW(str, dateString) == 0);

	return true;
}

bool testArray()
{
	HandleScope scope;
	int arrayLen = 5;
	Local<Array> arr = Array::New(arrayLen);
	assert(arr->IsArray());
	assert(arrayLen == arr->Length());
	return true;
	// TODO: add tests once object is implemented
}

extern void testObject();
extern void testFunction();
extern void testFunctionTemplate();

extern bool v8TestsRunTests()
{

	V8::Initialize();
	Persistent<Context> context = Context::New();
	Context::Scope currentScope(context);
	
	testDate();
	testBoolean();
	testNumber();
	testString();
	testArray();
	testInt();
	testExternalString();
	testScript();
	testObject();
	testFunction();
	// TODO: fix this...
	//testFunctionTemplate();
	return true;
}



