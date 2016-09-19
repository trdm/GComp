
#ifndef STORAGE_H
#define STORAGE_H


#include "Declarations.h"
#include "StreamInfo.h"

#import "zlibeng.dll" no_namespace

void SetIOPre(LPCTSTR MsgPrefix, LPCTSTR FileName);

class CStorage
{
private:
	static IzlibEnginePtr iLibEngine;
public:
	static void InitZLibEngine();
	static void ReleaseZLibEngine();

protected:
	CList<IStorage*, IStorage*&> Storages;
	bool IsRootStorage;
	void Push(IStorage* pStorage);
	IStorage* Pop();

	//Хэш-таблица файлов и объектов, выгруженных в текущей сессии. 
	//Нужна для последующей очистки каталога от старых файлов
	CMapStringToPtr* ExtractedFiles;
	//Доп. хэш-таблица объектов. В основном используется чтобы избежать повторной выгрузки описаний
	CMapPtrToPtr*    ExtractedObjects;

public:
	IStream* pStream;
	GCString StreamName;
	CStreamInfo StreamInfo;

private:
	void SetParentStorage(CStorage& Storage);

public:
	CStorage();
	CStorage(CStorage& Storage);
	//В основном этот конструктор вызывается с обычной строкой, поэтому для SubStorage
	//не будем делать ссылочный тип - пусть конструктор каждый раз вызывается
	CStorage(CStorage& Storage, GCString SubStorage);
	~CStorage();

public:
	GCString FullPath();

	bool OpenCompound(GCString& CompoundFileName);
	bool Open(CStringArray* StorageName);
	bool Open(GCString& StorageName);
	bool CreateCompound(GCString& CompoundFileName);
	bool Create(CStringArray* StorageName);
	bool Create(GCString& StorageName);
	bool Delete(GCString& StorageName);
	void Close(bool CloseAll = false);

	bool OpenStream(GCString& StreamName);
	bool CreateStream(GCString& StreamName);
	void CloseStream();

	bool StorageExist(GCString& StorageName);
	bool StreamExist(GCString& StreamName);
	long GetStreamSize(GCString& StreamName);

	void Rewind();

	void GetCleanStream();
	bool CopyToFile(FILE* File, unsigned long bytes_to_copy);
	bool CopyToFile(GCString& FileName, bool OnlyRestOfSream = false);
	bool CopyToFile(GCString& StreamName, GCString& FileName);
	bool Extract(GCString& DestDir, bool WithContainerContents = true);
	bool CopyStringToFile(GCString& String, GCString& FileName);

	bool StreamFromFile(GCString& StreamName, GCString& FileName);
	bool StreamFromString(GCString StreamName, GCString& String);

	unsigned long AppendFile(IStream* pStream, FILE* File);
	unsigned long AppendFile(IStream* pStream, GCString& FileName);
	void AddZeroByte();

	bool FromFile(GCString& FileName);
	bool FromFile(FILE* File, LPCSTR FileName = NULL);
	bool StorageFromDir(GCString& StorageName, GCString& SrcDir);

	//Сравнивает текущий поток с указанным файлом. Сравнение начинается с 
	//текущей позиции (как в потоке, так и в файле). Если указан bytes_to_compare > 0, 
	//то сравниваем только указанное количество байт. Если не указан - сравниваем до 
	//конца потока или файла.
	int Compare(FILE* File, unsigned long bytes_to_compare);
	int Compare(GCString& FileName, unsigned long bytes_to_compare);
	int CompareWithString(GCString& str2);
	//Операторы сравнения потока целиком (перед сравнением происходит перепозиционирование)
	int FullCompare(GCString& FileName);

	IStorage* GetStorage();

	void ParseStream(CMMSObject* Object, GCString& StreamName, bool ParseWitnNoIDs);

	void AddToExtractedList(GCString& FullFName, void* object);
	bool ObjectExtracted(void* object);
	int PruneDirectory(GCString& Path, bool NoEmptyFolders);

private:
	void ReleaseMaps();
	void WriteSizeToStream(IStream* pStream);
	bool ExtractPictureGallery(GCString& FileName);
	
	bool GalleryImported; //флаг - чтобы не импортировать Gallery 3 раза
	bool ImportPictureGallery(GCString& StreamName, GCString& FileName);

	void RemoveDirectory(GCString& Path);
};



LPCSTR GetStgErrorString(HRESULT err);

ULARGE_INTEGER GetStreamSize(IStream* pStream);

IStorage* OpenStorage(IStorage* pParentStorage, wchar_t* wStorName);
IStorage* OpenStorage(IStorage* pParentStorage, GCString& StorName);

IStream* OpenStream(IStorage* pParentStorage, wchar_t* wStreamName);
IStream* OpenStream(IStorage* pParentStorage, GCString& StreamName);

bool StringDifferFromFile(GCString& String, GCString& FileName);
bool CopyStringToFile(GCString& String, GCString& FileName);

#endif