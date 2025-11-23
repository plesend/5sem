#pragma once
#include <Windows.h>
#include <iostream>

namespace HT    // HT API
{
	// API HT - программный интерфейс для доступа к НТ-хранилищу 
	//          НТ-хранилище предназначено для хранения данных в ОП в формате ключ/значение
	//          Персистестеность (сохранность) данных обеспечивается с помощью snapshot-механизма 
	//          create - создать  и открыть HT-хранилище для использования   
	//          open   - открыть HT-хранилище для использования
	//          insert - создать элемент данных
	//          remove - удалить элемент данных    
	//          get    - читать  элемент данных
	//          update - изменить элемент данных
	//          snap   - выполнить snapshot
	//          close  - выполнить snap и закрыть HT-хранилище для использования
	//          getLastError - получить сообщение о последней ошибке

	struct HTHANDLE    // блок управления HT
	{
		HTHANDLE();
		HTHANDLE(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength, const wchar_t* fileName);
		int     Capacity;               // емкость хранилища в количестве элементов 
		int     SecSnapshotInterval;    // периодичность сохранения в сек. 
		int     MaxKeyLength;           // максимальная длина ключа
		int     MaxPayloadLength;       // максимальная длина данных
		wchar_t FileName[512];          // имя файла 
		HANDLE  File;                   // File HANDLE != 0, если файл открыт
		HANDLE  FileMapping;            // Mapping File HANDLE != 0, если mapping создан  
		LPVOID  Addr;                   // Addr != NULL, если mapview выполнен  
		char    LastErrorMessage[512];  // сообщение об последней ошибке или 0x00  
		time_t  LastSnaptime;           // дата последнего snap'a (time())

		int Count;						// текущее количество элементов в хэш-таблице
		HANDLE SnapshotTimer;			// таймер для snapshot
	};

	struct Element   // элемент 
	{
		Element();
		Element(const void* key, int keyLength);                                             // for get
		Element(const void* key, int keyLength, const void* payload, int  payloadLength);    // for insert
		Element(const Element* oldElement, const void* newPayload, int  newPayloadLength);   // for update
		const void* key;                 // значение ключа 
		int         keyLength;           // размер ключа
		const void* payload;             // данные 
		int         payloadLength;       // размер данных
	};

	int GetElementSize
	(
		int   maxKeyLength,                // максимальный размер ключа
		int   maxPayloadLength	  // максимальный размер данных
	);



	HTHANDLE* Create   //  создать HT             
	(
		int	  capacity,					   // емкость хранилища
		int   secSnapshotInterval,		   // периодичность сохранения в сек.
		int   maxKeyLength,                // максимальный размер ключа
		int   maxPayloadLength,            // максимальный размер данных
		const wchar_t* fileName           // имя файла 
	); 	// != NULL успешное завершение  

	HTHANDLE* Open     //  открыть HT             
	(
		const wchar_t* fileName         // имя файла 
	); 	// != NULL успешное завершение  

	BOOL Snap         // выполнить Snapshot
	(
		HTHANDLE* htHandle           // управление HT (File, FileMapping)
	);

	BOOL Close        // snap и закрыть HT  и  очистить htHandle
	(
		const HTHANDLE* htHandle           // управление HT (File, FileMapping)
	);	//  == TRUE успешное завершение   


	BOOL Insert      // добавить элемент в хранилище
	(
		HTHANDLE* htHandle,            // управление HT
		const Element* element              // элемент
	);	//  == TRUE успешное завершение 


	BOOL Delete      // удалить элемент в хранилище
	(
		HTHANDLE* htHandle,            // управление HT (ключ)
		const Element* element              // элемент 
	);	//  == TRUE успешное завершение 

	Element* Get     //  читать элемент в хранилище
	(
		HTHANDLE* htHandle,            // управление HT
		const Element* element              // элемент 
	); 	//  != NULL успешное завершение 


	BOOL Update     //  изменить элемент в хранилище
	(
		HTHANDLE* htHandle,            // управление HT
		const Element* oldElement,          // старый элемент (ключ, размер ключа)
		const void* newPayload,          // новые данные  
		int             newPayloadLength     // размер новых данных
	); 	//  != NULL успешное завершение 

	const char* GetLastError  // получить сообщение о последней ошибке
	(
		const HTHANDLE* htHandle                         // управление HT
	);

	void print                               // распечатать элемент 
	(
		const Element* element              // элемент 
	);

	int hashFunction(const char* key, int capacity);
	int nextHash(int currentHash, const char* key, int capacity);

	int findFreeIndex(const HTHANDLE* htHandle, const Element* element);
	BOOL writeToMemory(const HTHANDLE* const htHandle, const Element* const element, int index);
	int incrementCount(HTHANDLE* const htHandle);

	int findIndex(const HTHANDLE* htHandle, const Element* element);
	Element* readFromMemory(const HTHANDLE* const htHandle, Element* const element, int index);
	BOOL clearMemoryByIndex(const HTHANDLE* const htHandle, int index);
	int decrementCount(HTHANDLE* const htHandle);

	void CALLBACK snapAsync(LPVOID prm, DWORD, DWORD);
	const char* WriteLastError(HTHANDLE* const htHandle, const char* msg);

	HTHANDLE* OpenHTFromFile(const wchar_t* fileName);
	HTHANDLE* OpenHTFromMapName(const wchar_t* fileName);
};
