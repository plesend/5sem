#include "test.h"

void TestCreate(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength, const char* filename) {
    cout << "=== 1. Test Create ===" << endl;

    HTHANDLE* ht = Create(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, filename);
    assert(ht != nullptr);

    assert(ht->Capacity == capacity);
    assert(ht->SecSnapshotInterval == secSnapshotInterval);
    assert(ht->MaxKeyLength == maxKeyLength);
    assert(ht->MaxPayloadLength == maxPayloadLength);
    assert(strlen(ht->FileName) > 0);

    assert(Close(ht));
    cout << "Test Create: PASSED" << endl << endl;
}

void TestOpen(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength, const char* filename) {
    cout << "=== 2. Test Open ===" << endl;

    HTHANDLE* ht = Create(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, filename);
    assert(ht != nullptr);
    assert(Close(ht));

    ht = Open(filename);
    assert(ht != nullptr);

    assert(ht->Capacity == capacity);
    assert(ht->SecSnapshotInterval == secSnapshotInterval);
    assert(ht->MaxKeyLength == maxKeyLength);
    assert(ht->MaxPayloadLength == maxPayloadLength);

    assert(Close(ht));
    cout << "Test Open: PASSED" << endl << endl;
}

void TestInsert(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength,
    const char* filename, const char* key, const char* value) {
    cout << "=== 3. Test Insert ===" << endl;

    HTHANDLE* ht = Create(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, filename);
    assert(ht != nullptr);

    Element element(key, strlen(key), value, strlen(value));
    assert(Insert(ht, &element));
    cout << "Insert operation successful" << endl;

    assert(Close(ht));
    cout << "Test Insert: PASSED" << endl << endl;
}

void TestGet(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength,
    const char* filename, const char* key, const char* value) {
    cout << "=== 3. Test Get ===" << endl;

    HTHANDLE* ht = Create(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, filename);
    assert(ht != nullptr);

    Element insertElement(key, strlen(key), value, strlen(value));
    assert(Insert(ht, &insertElement));

    Element searchElement(key, strlen(key));
    Element* result = Get(ht, &searchElement);

    assert(result != nullptr);
    assert(result->keylength == strlen(key));
    assert(result->payloadlength == strlen(value));
    assert(memcmp(result->key, key, result->keylength) == 0);
    assert(memcmp(result->payload, value, result->payloadlength) == 0);

    cout << "Get operation successful" << endl;

    delete result;
    assert(Close(ht));
    cout << "Test Get: PASSED" << endl << endl;
}

void TestGetNonExistent(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength,
    const char* filename, const char* key) {
    cout << "=== 9.3. Test Get Non-Existent ===" << endl;

    HTHANDLE* ht = Create(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, filename);
    assert(ht != nullptr);

    Element searchElement(key, strlen(key));
    Element* result = Get(ht, &searchElement);

    assert(result == nullptr);
    cout << "Get non-existent handled correctly" << endl;

    assert(Close(ht));
    cout << "Test Get Non-Existent: PASSED" << endl << endl;
}

void TestUpdate(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength,
    const char* filename, const char* key, const char* initialValue, const char* newValue) {
    cout << "=== 5. Test Update ===" << endl;

    HTHANDLE* ht = Create(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, filename);
    assert(ht != nullptr);

    Element insertElement(key, strlen(key), initialValue, strlen(initialValue));
    assert(Insert(ht, &insertElement));

    Element searchElement(key, strlen(key));
    assert(Update(ht, &searchElement, newValue, strlen(newValue)));

    Element* result = Get(ht, &searchElement);
    assert(result != nullptr);
    assert(memcmp(result->payload, newValue, result->payloadlength) == 0);

    cout << "Update operation successful" << endl;

    delete result;
    assert(Close(ht));
    cout << "Test Update: PASSED" << endl << endl;
}

void TestDelete(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength,
    const char* filename, const char* key, const char* value) {
    cout << "=== 6. Test Delete ===" << endl;

    HTHANDLE* ht = Create(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, filename);
    assert(ht != nullptr);

    Element insertElement(key, strlen(key), value, strlen(value));
    assert(Insert(ht, &insertElement));

    Element searchElement(key, strlen(key));
    assert(Delete(ht, &searchElement));

    Element* result = Get(ht, &searchElement);
    assert(result == nullptr);

    cout << "Delete operation successful" << endl;

    assert(Close(ht));
    cout << "Test Delete: PASSED" << endl << endl;
}

void TestDeleteNonExistent(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength,
    const char* filename, const char* key) {
    cout << "=== 9.2. Test Delete Non-Existent ===" << endl;

    HTHANDLE* ht = Create(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, filename);
    assert(ht != nullptr);

    Element searchElement(key, strlen(key));
    assert(!Delete(ht, &searchElement));
    cout << "Delete non-existent handled correctly: " << GetLastError(ht) << endl;

    assert(Close(ht));
    cout << "Test Delete Non-Existent: PASSED" << endl << endl;
}

void TestDuplicateInsert(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength,
    const char* filename, const char* key, const char* value1, const char* value2) {
    cout << "=== 9.1. Test Duplicate Insert ===" << endl;

    HTHANDLE* ht = Create(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, filename);
    assert(ht != nullptr);

    Element element1(key, strlen(key), value1, strlen(value1));
    Element element2(key, strlen(key), value2, strlen(value2));

    assert(Insert(ht, &element1));
    assert(!Insert(ht, &element2));
    cout << "Duplicate insert prevented: " << GetLastError(ht) << endl;

    assert(Close(ht));
    cout << "Test Duplicate Insert: PASSED" << endl << endl;
}

void TestSnap(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength,
    const char* filename, const char* key, const char* value) {
    cout << "=== 7. Test Snap ===" << endl;

    HTHANDLE* ht = Create(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, filename);
    assert(ht != nullptr);

    Element element(key, strlen(key), value, strlen(value));
    assert(Insert(ht, &element));

    assert(Snap(ht));
    cout << "Snap operation successful" << endl;

    assert(Close(ht));
    cout << "Test Snap: PASSED" << endl << endl;
}

void TestClose(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength, const char* filename) {
    cout << "=== 8. Test Close ===" << endl;

    HTHANDLE* ht = Create(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, filename);
    assert(ht != nullptr);

    assert(Close(ht));
    cout << "Close operation successful" << endl;

    cout << "Test Close: PASSED" << endl << endl;
}

void TestGetLastError(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength,
    const char* filename, const char* key) {
    cout << "=== 9.4. Test GetLastError ===" << endl;

    HTHANDLE* ht = Create(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, filename);
    assert(ht != nullptr);

    Element searchElement(key, strlen(key));
    Delete(ht, &searchElement);

    char* errorMsg = GetLastError(ht);
    assert(errorMsg != nullptr && strlen(errorMsg) > 0);
    cout << "Error message: " << errorMsg << endl;

    assert(Close(ht));
    cout << "Test GetLastError: PASSED" << endl << endl;
}

void TestPrint(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength,
    const char* filename, const char* key, const char* value) {
    cout << "=== 10. Test Print ===" << endl;

    HTHANDLE* ht = Create(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, filename);
    assert(ht != nullptr);

    Element element(key, strlen(key), value, strlen(value));
    assert(Insert(ht, &element));

    Element searchElement(key, strlen(key));
    Element* result = Get(ht, &searchElement);

    cout << "Print output:" << endl;
    print(result);

    delete result;
    assert(Close(ht));
    cout << "Test Print: PASSED" << endl << endl;
}
