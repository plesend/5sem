#pragma once
#include "ОССП_Лабораторная_работа_10_HT.h"
#include <iostream>
#include <cassert>

using namespace std;
using namespace HT;

void TestCreate
(
	int   capacity,
	int   secSnapshotInterval,
	int   maxKeyLength,
	int   maxPayloadLength,
	const char* filename
);

void TestOpen
(
	int   capacity,
	int   secSnapshotInterval,
	int   maxKeyLength,
	int   maxPayloadLength,
	const char* filename
);

void TestInsert
(
	int   capacity,
	int   secSnapshotInterval,
	int   maxKeyLength,
	int   maxPayloadLength,
	const char* filename,
	const char* key,
	const char* value
);

void TestGet
(
	int   capacity,
	int   secSnapshotInterval,
	int   maxKeyLength,
	int   maxPayloadLength,
	const char* filename,
	const char* key,
	const char* value
);

void TestGetNonExistent
(
	int   capacity,
	int   secSnapshotInterval,
	int   maxKeyLength,
	int   maxPayloadLength,
	const char* filename,
	const char* key
);

void TestUpdate
(
	int   capacity,
	int   secSnapshotInterval,
	int   maxKeyLength,
	int   maxPayloadLength,
	const char* filename,
	const char* key,
	const char* initialValue,
	const char* newValue
);

void TestDelete
(
	int   capacity,
	int   secSnapshotInterval,
	int   maxKeyLength,
	int   maxPayloadLength,
	const char* filename,
	const char* key,
	const char* value
);

void TestDeleteNonExistent
(
	int   capacity,
	int   secSnapshotInterval,
	int   maxKeyLength,
	int   maxPayloadLength,
	const char* filename,
	const char* key
);

void TestDuplicateInsert
(
	int   capacity,
	int   secSnapshotInterval,
	int   maxKeyLength,
	int   maxPayloadLength,
	const char* filename,
	const char* key,
	const char* value1,
	const char* value2
);

void TestSnap
(
	int   capacity,
	int   secSnapshotInterval,
	int   maxKeyLength,
	int   maxPayloadLength,
	const char* filename,
	const char* key,
	const char* value
);

void TestClose
(
	int   capacity,
	int   secSnapshotInterval,
	int   maxKeyLength,
	int   maxPayloadLength,
	const char* filename
);

void TestGetLastError
(
	int   capacity,
	int   secSnapshotInterval,
	int   maxKeyLength,
	int   maxPayloadLength,
	const char* filename,
	const char* key
);

void TestPrint
(
	int   capacity,
	int   secSnapshotInterval,
	int   maxKeyLength,
	int   maxPayloadLength,
	const char* filename,
	const char* key,
	const char* value
);
