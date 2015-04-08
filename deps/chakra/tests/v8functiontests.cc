#include "v8.h"
#include "jsrt.h"
#include <cassert>

using namespace v8;

void testCallFunction();
void testNewInstance();

extern void testFunction()
{
	testCallFunction();
	testNewInstance();
}

void testCallFunction()
{
	HandleScope scope;

	Local<Script> createFunction = Script::New(String::New(L"(function() { return function(a,b) { return a+b; }; }).call(this)"), String::New(L"test1"));
	Local<Function> function = createFunction->Run().As<Function>();

	Handle<Value> args[] = { Integer::New(1), Integer::New(2) };
	auto result = function->Call(Local<Object>(), _countof(args), args);
	assert(result->IsNumber());

	Local<Number> number = result.As<Number>();
	assert(number->Equals(Integer::New(3)));

	Handle<Value> args2[] = { String::New(L"1"), String::New(L"2") };
	result = function->Call(Context::GetCurrent()->Global(), _countof(args2), args2);
	assert(result->IsString());

	Local<String> str = result.As<String>();
	assert(str->Equals(String::New(L"12")));
}

void testNewInstance()
{
	HandleScope scope;

	Local<Script> createFunction = Script::New(String::New(L"(function() { return function(a) { return this.test = a; }; }).call(this)"), String::New(L"test2"));
	Local<Function> function = createFunction->Run().As<Function>();

	Handle<Value> args[] = { Integer::New(1), Integer::New(2) };
	auto result = function->NewInstance(_countof(args), args);
	assert(result->IsObject());

	Local<Object> obj = result.As<Object>();
	assert(obj->Get(String::NewSymbol(L"test"))->StrictEquals(Integer::New(1)));

	Handle<Value> args2[] = { String::New(L"1") };
	result = function->NewInstance(_countof(args2), args2);
	assert(result->IsObject());

	obj = result.As<Object>();
	assert(obj->Get(String::NewSymbol(L"test"))->StrictEquals(String::New(L"1")));
}
