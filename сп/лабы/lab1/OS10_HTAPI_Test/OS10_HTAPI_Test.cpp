#include "Header.h"
#include <cassert>
#include <iostream>

using namespace std;
using namespace HT;

void TestFunctionCreate()
{
	HT::HTHANDLE* ht = HT::Create(1000, 3, 10, 256, "HTAPITEST.ht", 0);
	assert(ht != 0);
	cout << "TestFunctionCreate: Pass\n";
	Close(ht);
}
void TestFunctionOpen()
{
	HT::HTHANDLE* ht = Open("HTAPITEST.ht");
	assert(ht != 0);
	cout << "TestFunctionOpen: Pass\n";
	Close(ht);
}
void TestFunctionInsert()
{
	HT::HTHANDLE* ht = HT::Create(1000, 3, 10, 256, "HTAPITEST.ht", 0);
	Insert(ht, new HT::Element("key123", 7, "payload1", 8));
	Insert(ht, new HT::Element("key124", 7, "payload2", 8));
	assert(ht->elementCount == 2);

	cout << "TestFunctionInsert: Pass\n";
	Close(ht);
}
void TestFunctionGet()
{
	HT::HTHANDLE* ht = HT::Create(1000, 3, 10, 256, "HTAPITEST.ht", 0);
	HT::Insert(ht, new HT::Element("key123", 7, "payload1", 8));

	HT::Element* e = HT::Get(ht, new HT::Element("key123", 7));
	assert(e != nullptr);
	cout << "TestFunctionGet: Pass\n";

	HT::Close(ht);
}
void TestFunctionUpdate()
{
	HT::HTHANDLE* ht = HT::Create(1000, 3, 10, 256, "HTAPITEST.ht", 0);
	HT::Insert(ht, new HT::Element("key123", 7, "payload1", 8));

	HT::Element* e = HT::Get(ht, new HT::Element("key123", 7));
	assert(e != nullptr);

	bool updated = HT::Update(ht, e, "newpayload", 10);
	assert(updated);

	HT::Element* e2 = HT::Get(ht, new HT::Element("key123", 7));
	assert(e2 != nullptr);
	cout << "TestFunctionUpdate: Pass\n";

	HT::Close(ht);
}
void TestFunctionDelete()
{
	HT::HTHANDLE* ht = HT::Create(1000, 3, 10, 256, "HTAPITEST.ht", 0);
	HT::Insert(ht, new HT::Element("key123", 7, "payload1", 8));

	HT::Element* e = HT::Get(ht, new HT::Element("key123", 7));
	assert(e != nullptr);

	bool deleted = HT::Delete(ht, e);
	assert(deleted);

	HT::Element* e2 = HT::Get(ht, new HT::Element("key123", 7));
	assert(e2 == nullptr);
	cout << "TestFunctionDelete: Pass\n";

	HT::Close(ht);
}
void TestFunctionClose()
{
	HT::HTHANDLE* ht = HT::Create(1000, 3, 10, 256, "HTAPITEST.ht", 0);
	assert(ht != nullptr);

	bool closed = HT::Close(ht);
	assert(closed);
	cout << "TestFunctionClose: Pass\n";
}

int main()
{
	TestFunctionCreate();
	TestFunctionOpen();
	TestFunctionInsert();

	TestFunctionGet();

	TestFunctionUpdate();
	TestFunctionDelete();
	TestFunctionClose();

	cout << "tests";
	
}
