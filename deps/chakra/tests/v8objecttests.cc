#include "v8.h"
#include "jsrt.h"
#include <cassert>

using namespace v8;

void testGetterSetter();
void testHiddenData();
void testInternalFields();
void testExternalArrayData();
void testGetPropertyNames();

extern void testObject()
{
	HandleScope scope;
	// test properties:
	Local<Object> obj = Object::New();
	assert(obj->IsObject());
	// test set/get property
	assert(!obj.IsEmpty());
	Local<String> propertyName = String::NewSymbol("one");
	assert(!obj->Has(propertyName));
	Local<Number> val = Number::New(2);
	assert(obj->Set(propertyName, val));
	assert(obj->Has(propertyName));
	Local<Value> valInObj = obj->Get(propertyName);
	assert(valInObj->Equals(val));

	obj->Delete(propertyName);
	assert(!obj->Has(propertyName));
	
	// test get/set indexed property
	for (unsigned int i=0; i<5 ;i++)
	{
		val = Number::New(i);
		assert(obj->Set(i, val));
	}

	for (unsigned int i=0; i<5 ;i++)
	{
		val = Number::New(i);
		Local<Value> retVal = obj->Get(i);
		assert(retVal->Equals(val));
	}

	Local<Value> constructor = obj->GetConstructor();
	assert(constructor->IsFunction());

	Local<Value> protoype = obj->GetPrototype();
	assert(!protoype.IsEmpty());

	testGetterSetter();
	testHiddenData();
	testInternalFields();
	testExternalArrayData();
	testGetPropertyNames();
}

const int GetterValue = 325;
const wchar_t* propertyName = L"getterSetterTest";
const char* propertyNameAscii = "getterSetterTest";
const int DataValue = 32;
const int SetterValue = 444433;

Handle<Value> Getter(Local<String> property, const AccessorInfo &info)
{
	String::AsciiValue ascii(property);
	const char *getterPropNameAscii = *ascii;
	assert(strcmp(propertyNameAscii, getterPropNameAscii) == 0);
	
	int dataNumVal = info.Data()->Int32Value();
	assert(dataNumVal == DataValue);
	return Number::New(GetterValue);
}

void Setter(Local<String> property, Local<Value> value, const AccessorInfo &info)
{
	String::AsciiValue ascii(property);
	const char *getterPropNameAscii = *ascii;
	assert(strcmp(propertyNameAscii, getterPropNameAscii) == 0);
	
	int dataNumVal = info.Data()->Int32Value();
	assert(dataNumVal == DataValue);

	int val = value->Int32Value();
	assert(val == SetterValue);
}

void testGetterSetter()
{
	Local<Object> obj = Object::New();
	assert(obj->IsObject());
	// test set/get property
	assert(!obj.IsEmpty());
	Local<Value> dataToPass = Number::New(DataValue);
	obj->SetAccessor(String::NewSymbol(propertyNameAscii), Getter, Setter,dataToPass);
	
	// test getter
	Handle<Value> retVal = obj->Get(String::NewSymbol(propertyNameAscii));
	assert(retVal->IsNumber());
	int val = retVal->Int32Value();
	assert(val == GetterValue);

	// test setter
	assert( obj->Set(String::NewSymbol(propertyNameAscii), Integer::New(SetterValue)) );
}

void testGetPropertyNames()
{
	Local<Object> obj = Object::New();
	obj->Set(String::NewSymbol(L"first"),True());
	obj->Set(String::NewSymbol(L"second"),True());
	obj->Set(String::NewSymbol(L"third"),True());

	Local<Array> propertyNames = obj->GetPropertyNames();

	bool gotFirst = false;
	bool gotSecond = false;
	bool gotThird = false;

	unsigned int length = propertyNames->Length();
	assert(length > 0);
	for (int i=0; i<length; i++)
	{
		Local<Value> val = propertyNames->Get(i);

		assert(val->IsString());
		
		String::Value strVal(val);
		
		if (wcscmp(L"first", (wchar_t*)*strVal) == 0)
		{
			gotFirst =  true;
		}

		if (wcscmp(L"second", (wchar_t*)*strVal) == 0)
		{
			gotSecond =  true;
		}

		if (wcscmp(L"third", (wchar_t*)*strVal) == 0)
		{
			gotThird =  true;
		}
	}

	assert(gotFirst);
	assert(gotSecond);
	assert(gotThird);
}

void testHiddenData()
{
	Local<Object> obj = Object::New();
	assert(obj->IsObject());
	
	assert(!obj.IsEmpty());
	assert(obj->SetHiddenValue(String::NewSymbol("hiddenDataTest"), String::New(L"stringTest")));
	assert(!obj->Has(String::NewSymbol("hiddenDataTest")));
	Local<Value> hiddenVal = obj->GetHiddenValue(String::NewSymbol("hiddenDataTest"));
	assert(!hiddenVal.IsEmpty());
	assert(hiddenVal->IsString());
	String::AsciiValue asciiVal(hiddenVal);
	const char* asciiValPtr = *asciiVal;
	assert(strcmp(asciiValPtr,"stringTest") == 0);
}

void testInternalFields()
{
	// TODO: fix the tests to use object templates..
	// this test was written for the first version of the internal fields that were implemented
	// now internal fields are initialized in object templates

	/*Local<Object> obj = Object::New();
	assert(obj->IsObject());
	assert(!obj.IsEmpty());
	assert(obj->InternalFieldCount() == 0);
	int data1 = 234;
	obj->SetPointerInInternalField(0,&data1);
	assert(obj->InternalFieldCount() == 1);
	int data2 = 111;
	obj->SetPointerInInternalField(1,&data2);
	assert(obj->InternalFieldCount() == 2);

	int *retData1 = (int*)obj->GetPointerFromInternalField(0);
	assert(*retData1 == data1);

	int *retData2 = (int*)obj->GetPointerFromInternalField(1);
	assert(*retData2 == data2);	

	assert(obj->GetPointerFromInternalField(3) == nullptr);
	assert(obj->GetPointerFromInternalField(-2) == nullptr);*/
	
}

void testExternalArrayData()
{
	//Local<Object> obj = Object::New(true);
	//assert(obj->IsObject());
	//assert(!obj.IsEmpty());


	//int externalData[10];
	//for (int i=0; i<10; i++)
	//{
	//	externalData[i] = i;
	//}

	//obj->SetIndexedPropertiesToExternalArrayData(externalData, kExternalIntArray, 10);

	//assert(obj->GetIndexedPropertiesExternalArrayData() == externalData);
	//assert(obj->GetIndexedPropertiesExternalArrayDataLength() == 10);
	//assert(obj->GetIndexedPropertiesExternalArrayDataType() == kExternalIntArray);

	//for (int i=0; i<10; i++)
	//{
	//	assert(obj->Get(i)->Int32Value() == i);
	//}

	//assert(obj->Set(5,Number::New(321)));
	//assert(externalData[5] == 321);
	//Local<Value> retVal = obj->Get(11);
	//
	//assert(retVal->Equals(Undefined()));
	//assert(obj->Set(14, Number::New(1)));
	//retVal = obj->Get(14);
	//assert(retVal->Equals(Undefined()));
}
