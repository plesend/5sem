#pragma once
#include <Windows.h>
#include <mutex>

namespace HT    // HT API
{
	

	struct HTHANDLE    // ���� ���������� HT
	{
		HTHANDLE();
		HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512], int elementCount);
		int     Capacity;
		int		elementCount;             // ������� ��������� � ���������� ��������� 
		int     SecSnapshotInterval;    // ������������� ���������� � ���. 
		int     MaxKeyLength;           // ������������ ����� �����
		int     MaxPayloadLength;       // ������������ ����� ������
		char    FileName[512];          // ��� ����� 
		HANDLE  File;                   // File HANDLE != 0, ���� ���� ������
		HANDLE  FileMapping;            // Mapping File HANDLE != 0, ���� mapping ������  
		LPVOID  Addr;                   // Addr != NULL, ���� mapview ��������  
		char    LastErrorMessage[512];  // ��������� �� ��������� ������ ��� 0x00  
		time_t  lastsnaptime;           // ���� ���������� snap'a (time()) 
		BYTE* DataAddr;
	};

	struct Element   // ������� 
	{
		Element();
		Element(const void* key, int keylength);                                             // for Get
		Element(const void* key, int keylength, const void* payload, int  payloadlength);    // for Insert
		Element(Element* oldelement, const void* newpayload, int  newpayloadlength);         // for update
		const void* key;                 // �������� ����� 
		int             keylength;           // ������ �����
		const void* payload;             // ������ 
		int             payloadlength;       // ������ ������
	};

	HTHANDLE* Create   //  ������� HT             
	(
		int	  Capacity,					   // ������� ���������
		int   SecSnapshotInterval,		   // ������������� ���������� � ���.
		int   MaxKeyLength,                // ������������ ������ �����
		int   MaxPayloadLength,            // ������������ ������ ������
		const char  FileName[512],
		int elementCount          // ��� ����� 
	); 	// != NULL �������� ����������  

	HTHANDLE* Open     //  ������� HT             
	(
		const char    FileName[512]         // ��� ����� 
	); 	// != NULL �������� ����������  

	BOOL Snap         // ��������� Snapshot
	(
	 HTHANDLE* hthandle           // ���������� HT (File, FileMapping)
	);


	BOOL Close        // Snap � ������� HT  �  �������� HTHANDLE
	(
		HTHANDLE* hthandle           // ���������� HT (File, FileMapping)
	);	//  == TRUE �������� ����������   


	BOOL Insert      // �������� ������� � ���������
	(
		HTHANDLE* hthandle,            // ���������� HT
		const Element* element              // �������
	);	//  == TRUE �������� ���������� 


	BOOL Delete      // ������� ������� � ���������
	(
		HTHANDLE* hthandle,            // ���������� HT (����)
		const Element* element              // ������� 
	);	//  == TRUE �������� ���������� 

	Element* Get     //  ������ ������� � ���������
	(
		 HTHANDLE* hthandle,            // ���������� HT
		const Element* element              // ������� 
	); 	//  != NULL �������� ���������� 


	BOOL Update     //  ������� ������� � ���������
	(
		HTHANDLE* hthandle,            // ���������� HT
		const Element* oldelement,          // ������ ������� (����, ������ �����)
		const void* newpayload,          // ����� ������  
		int             newpayloadlength     // ������ ����� ������
	); 	//  != NULL �������� ���������� 

	//char* GetLastError  // �������� ��������� � ��������� ������
	//(
	//	HTHANDLE* ht                         // ���������� HT
	//);

	void print                               // ����������� ������� 
	(
		const Element* element              // ������� 
	);


};
