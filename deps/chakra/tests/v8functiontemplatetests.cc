#include "v8.h"
#include "jsrt.h"
#include "jsrtutils.h"
#include <cassert>

using namespace v8;

void testCallFunctionTemplate();
void testNewInstanceTemplate();
void testAddPrototypeProperty();
void testAddObjectMemberProperty();

extern void testFunctionTemplate()
{
	testCallFunctionTemplate();
	testNewInstanceTemplate();
	testAddPrototypeProperty();
	testAddObjectMemberProperty();
}

Handle<Value> CallCallback(const Arguments &args)
{
	HandleScope scope;

	assert(args.Length() == 2);
	if (args[0]->IsNumber())
	{
		return Handle<Value>(*Number::New(args[0].As<Number>()->NumberValue() + args[1].As<Number>()->NumberValue()));
	}
	else
	{
		return Handle<Value>(*String::Concat(args[0].As<String>(), args[1].As<String>()));
	}
}

Handle<Value> NewInstanceCallback(const Arguments &args)
{
	HandleScope scope;
	args.This()->Set(String::NewSymbol(L"version"), args[0]);
	assert(args.Length() == 1);

	return Undefined();
}

void testCallFunctionTemplate(Local<FunctionTemplate> ft)
{
	HandleScope scope;

	Local<Function> function = ft->GetFunction();

	Handle<Value> args[] = { Integer::New(1), Integer::New(2) };
	auto result = function->Call(Context::GetCurrent()->Global(), _countof(args), args);
	assert(result->IsNumber());

	Local<Number> number = result.As<Number>();
	assert(number->Equals(Integer::New(3)));

	Handle<Value> args2[] = { String::New(L"1"), String::New(L"2") };
	result = function->Call(Context::GetCurrent()->Global(), _countof(args2), args2);
	assert(result->IsString());

	Local<String> str = result.As<String>();
	assert(str->Equals(String::New(L"12")));
}

void testCallFunctionTemplate()
{
	HandleScope scope;
	Local<FunctionTemplate> ft = FunctionTemplate::New(CallCallback);

	testCallFunctionTemplate(ft);
}

void testNewInstanceTemplate()
{
	HandleScope scope;

	Local<FunctionTemplate> ft = FunctionTemplate::New(NewInstanceCallback);
	Local<Function> function = ft->GetFunction();

	Handle<Value> args[] = { Integer::New(1) };
	auto result = function->NewInstance(_countof(args), args);
	assert(result->IsObject());

	Local<Object> obj = result.As<Object>();
	assert(obj->Get(String::NewSymbol(L"version"))->StrictEquals(Integer::New(1)));

	Handle<Value> args2[] = { String::New(L"1") };
	result = function->NewInstance(_countof(args2), args2);
	assert(result->IsObject());

	obj = result.As<Object>();
	assert(obj->Get(String::NewSymbol(L"version"))->StrictEquals(String::New(L"1")));
}

void testAddPrototypeProperty()
{
	HandleScope scope;

	Local<FunctionTemplate> ft = FunctionTemplate::New(NewInstanceCallback);
	Local<Function> function = ft->GetFunction();

	ft->PrototypeTemplate()->Set(String::NewSymbol("prop"), String::New(L"prop"));
	ft->PrototypeTemplate()->Set(String::NewSymbol("func"), FunctionTemplate::New(CallCallback));

	Handle<Value> args[] = { Integer::New(1) };
	auto result = function->NewInstance(_countof(args), args);
	assert(result->IsObject());

	Local<Object> obj = result.As<Object>();
	bool isInstanceOf;
	jsrt::InstanceOf(*obj, *ft, &isInstanceOf);
	assert(isInstanceOf);

	assert(obj->Get(String::NewSymbol(L"version"))->StrictEquals(Integer::New(1)));
	assert(obj->Get(String::NewSymbol(L"prop"))->StrictEquals(String::New(L"prop")));

	Local<FunctionTemplate> funcTemplate = Local<FunctionTemplate>((FunctionTemplate*)*obj->Get(String::NewSymbol(L"func")));
	testCallFunctionTemplate(funcTemplate);

	Local<Function> func = obj->Get(String::NewSymbol(L"func")).As<Function>();
	Handle<Value> args2[] = { Integer::New(1), Integer::New(2) };
	auto result2 = func->Call(obj, _countof(args2), args2);
	assert(result2->IsNumber());

	Local<Number> number = result2.As<Number>();
	assert(number->Equals(Integer::New(3)));
}

void testAddObjectMemberProperty()
{
	HandleScope scope;

	Local<FunctionTemplate> ft = FunctionTemplate::New();
	Local<Function> function = ft->GetFunction();

	ft->InstanceTemplate()->Set(String::NewSymbol(L"version"), Integer::New(1));
	ft->PrototypeTemplate()->Set(String::NewSymbol("prop"), String::New(L"prop"));

	Handle<Value> args[] = { Integer::New(1) };
	auto result = function->NewInstance(_countof(args), args);
	assert(result->IsObject());

	Local<Object> obj = result.As<Object>();
	bool isInstanceOf;
	jsrt::InstanceOf(*obj, *ft, &isInstanceOf);
	assert(isInstanceOf);

	assert(obj->Get(String::NewSymbol(L"version"))->StrictEquals(Integer::New(1)));
	assert(obj->Get(String::NewSymbol(L"prop"))->StrictEquals(String::New(L"prop")));
}
