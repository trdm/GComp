
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <sys\stat.h>
#include "DLL\GComp.h"
#include "Storage.h"
#include "CommonFunctions.h"
#include "StreamInfo.h"
#include "MMS.h"
#include "Lexer.h"


void SetIOPre(LPCTSTR MsgPrefix, LPCTSTR FileName)
{
	GCString preActMsg;
	preActMsg =  "PRE_";
	preActMsg += MsgPrefix;
	
	Msg(1, "%s: %s", preActMsg, FileName);
	
	DWORD  dwAttrs  = GetFileAttributes(FileName);
	if ((dwAttrs & FILE_ATTRIBUTE_READONLY)) 
	{ 
		dwAttrs &= ~FILE_ATTRIBUTE_READONLY;
		BOOL res = SetFileAttributes(FileName, dwAttrs);
	}	
}


static char _StgErrorString[256];
LPCSTR GetStgErrorString(HRESULT err)
{
	switch( err )
	{
		case STG_E_ACCESSDENIED:
			return "Access denied";
		case STG_E_FILENOTFOUND:
			return "The element with the specified name does not exist";
		case STG_E_FILEALREADYEXISTS:
			return "File already exist";
		case STG_E_INVALIDPOINTER:
			return "The pointer specified for the element was invalid";
		case STG_E_INSUFFICIENTMEMORY:
			return "Insufficient memory";
		case STG_E_INVALIDNAME:
			return "Invalid name";
		case STG_E_TOOMANYOPENFILES:
			return "Too many open files";
		case -2147287008:
			return "Share violation";
	}
	sprintf(_StgErrorString, "Unknown error - %l", (long)err);
	return _StgErrorString;
}


////////////////////////////////////////////////////////////////////
IzlibEnginePtr CStorage::iLibEngine;

void CStorage::InitZLibEngine()
{
	CoInitialize(NULL);
	if( iLibEngine.CreateInstance(L"V77.zlibEngine") != S_OK )
	{
		//Msg(0, "Error CreateInstance V77.zlibEngine");		
		if( iLibEngine.CreateInstance(L"V75.zlibEngine") != S_OK )
		{
			Msg(0, "Error CreateInstance V75.zlibEngine");
			throw NULL;
		}
	}
}

void CStorage::ReleaseZLibEngine()
{
	iLibEngine.Release();
	CoUninitialize();
}

CStorage::CStorage()
{
	IsRootStorage = true;
	pStream = NULL;
	ExtractedFiles = new CMapStringToPtr;
	ExtractedObjects = new CMapPtrToPtr;
}

void CStorage::SetParentStorage(CStorage& Storage)
{
	IsRootStorage = false;
	IStorage* pStorage = Storage.GetStorage();
	Push(pStorage);
	ExtractedFiles = Storage.ExtractedFiles;
	ExtractedObjects = Storage.ExtractedObjects;
}

CStorage::CStorage(CStorage& Storage)
{
	SetParentStorage(Storage);
}

CStorage::CStorage(CStorage& Storage, GCString SubStorage)
{
	SetParentStorage(Storage);
	Open(SubStorage);
}

CStorage::~CStorage()
{
	Close(true);
	ReleaseMaps();
}

void CStorage::ReleaseMaps()
{
	if( IsRootStorage )
	{
		if( ExtractedFiles   != NULL ) delete ExtractedFiles;
		if( ExtractedObjects != NULL ) delete ExtractedObjects;
		ExtractedFiles   = NULL;
		ExtractedObjects = NULL;
	}
}


GCString CStorage::FullPath()
{
	GCString str;
	POSITION pos = Storages.GetHeadPosition();
	while( pos != NULL )
	{
		STATSTG  stat;
		IStorage* Storage = Storages.GetAt(pos);
		GCString stor;
		Storage->Stat(&stat, STATFLAG_DEFAULT);
		stor.Format("\\%S", stat.pwcsName);
		str += stor;
		Storages.GetNext(pos);
	}
	return str;
}

void CStorage::Push(IStorage* pStorage)
{
	Storages.AddTail(pStorage);
	pStream = NULL;
}

IStorage* CStorage::Pop()
{
	if( !IsRootStorage && Storages.GetCount() <= 1 )
		return NULL;

	POSITION pos = Storages.GetTailPosition();
	if( pos == NULL ) 
		return NULL;

	IStorage* pStorage = Storages.GetAt(pos);
	Storages.RemoveTail();
	return pStorage;
}

IStorage* CStorage::GetStorage()
{
	POSITION pos = Storages.GetTailPosition();
	if( pos == NULL ) 
		return NULL;
	return Storages.GetAt(pos);
}


bool CStorage::CreateCompound(GCString& CompoundFileName)
{
	if( !IsRootStorage ) //Создавать compound можно только из корневого объекта
		return false;
	Close(true);

	IStorage* pRootStorage;
	HRESULT hr;
	wchar_t* wCompoundName = new wchar_t[CompoundFileName.GetLength()+1];
	
	swprintf(wCompoundName , L"%S", (LPCSTR)CompoundFileName);
	hr = StgCreateDocfile(wCompoundName, 
		STGM_DIRECT | STGM_READWRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0, &pRootStorage);
	delete[] wCompoundName;
	
	if( hr != S_OK )
	{
		Msg(0, "ERR: can't create storage '%s' - %s", 
			(LPCSTR)CompoundFileName, GetStgErrorString(hr));
		throw NULL;
	}

	Push(pRootStorage);

	return true;
}

bool CStorage::OpenCompound(GCString& CompoundFileName)
{
	if( !IsRootStorage ) //Открывать compound можно только из корневого объекта
		return false;
	Close(true);

	IStorage* pRootStorage;
	wchar_t* wCompoundFileName = new wchar_t[CompoundFileName.GetLength()+1];

	swprintf(wCompoundFileName, L"%S", CompoundFileName);
	HRESULT hr = StgOpenStorage(wCompoundFileName , NULL, 
		STGM_SHARE_EXCLUSIVE | STGM_DIRECT | STGM_READWRITE
		//STGM_READ | STGM_SHARE_DENY_WRITE
		, NULL, 0, 
		&pRootStorage);

	delete[] wCompoundFileName;

	if( S_OK != hr )
	{
		Msg(0, "ERR: Can not open storage '%s' - %s", 
			(LPCSTR)CompoundFileName,  GetStgErrorString(hr));
		throw NULL;
	}

	Push(pRootStorage);

	return true;
}

bool CStorage::Open(CStringArray* StorageNames)
{
	int i, max_i = StorageNames->GetSize() - 1;
	for( i = 0; i <= max_i; i++ )
	{
		if( !Open( GCString(StorageNames->GetAt(i)) ) )
			return false;
	}

	return true;
}

bool CStorage::Open(GCString& StorageName)
{
	IStorage* Parent = GetStorage();
	if( Parent == NULL ) return false;

	if( StorageName.Find("\\") < 0 )
	{
		IStorage* pStorage = OpenStorage(Parent, StorageName);
		if( pStorage == NULL ) return false;
		Push(pStorage);
	}
	else
	{
		CStringArray* name_parts = SplitPath(StorageName);
		bool res = Open(name_parts);
		delete name_parts;
		return res;
	}

	return true;
}

bool CStorage::Create(CStringArray* StorageNames)
{
	int i, max_i = StorageNames->GetSize() - 1;
	for( i = 0; i <= max_i; i++ )
	{
		if( !Create(GCString(StorageNames->GetAt(i))) ) return false;
	}
	return true;
}

bool CStorage::Create(GCString& StorageName)
{
	IStorage* Parent = GetStorage();
	if( Parent == NULL ) return false;

	if( StorageName.Find("\\") >= 0 )
	{
		CStringArray* name_parts = SplitPath(StorageName);
		bool res = Create(name_parts);
		delete name_parts;
		return res;
	}

	if( Open(StorageName) ) return true;

	IStorage* pStorage;
	HRESULT hr;
	wchar_t* wStorageName = new wchar_t[StorageName.GetLength()+1];
	swprintf(wStorageName, L"%S", (LPCSTR)StorageName);

	hr = GetStorage()->CreateStorage(wStorageName,
		STGM_DIRECT | STGM_READWRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0,0, 
		&pStorage);
	delete[] wStorageName;

	if( hr != S_OK )
	{
		Msg(0, "ERR: Cant create storage '%s' in '%s' - %s", (LPCSTR)StorageName, (LPCSTR)FullPath(), GetStgErrorString(hr));
		return false;
	}

	Push(pStorage);
	return true;
}

bool CStorage::CreateStream(GCString& StreamName)
{
	CloseStream();

	if( StreamName.Find("\\") >= 0 )
	{
		CStringArray* name_parts = SplitPath(StreamName);
		this->StreamName = name_parts->GetAt(name_parts->GetSize()-1);

		name_parts->SetSize(name_parts->GetSize()-1);
		bool res = Create(name_parts);
		delete name_parts;
		if( !res ) return false;
	}
	else
		this->StreamName = StreamName;

	HRESULT hr;
	wchar_t* wStreamName = new wchar_t[this->StreamName.GetLength()+1];
	swprintf(wStreamName, L"%S", (LPCSTR)(this->StreamName));


	hr = GetStorage()->CreateStream(wStreamName, 
		STGM_DIRECT | STGM_READWRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
		0,0, &pStream);
	delete[] wStreamName;
	
	if( S_OK != hr )
	{
		Msg(0, "ERR: Can't create stream '%s' - %s", (LPCSTR)StreamName, GetStgErrorString(hr));
		throw NULL;
	}

	StreamInfo.UpdateInfo((char*)(LPCSTR)(this->StreamName));
	return true;
}

void CStorage::Close(bool CloseAll)
{
	CloseStream();

	IStorage* pStorage;
	while( NULL != (pStorage = Pop()) )
	{
		//pStorage->Commit(STGC_DEFAULT);
		pStorage->Release();
		if( !CloseAll ) return;
	}
}

bool CStorage::Delete(GCString& StorageName)
{
	wchar_t* wStorageName = new wchar_t[StorageName.GetLength()+1];
	HRESULT hr;
	
	swprintf(wStorageName, L"%S", (LPCSTR)StorageName);
	hr = GetStorage()->DestroyElement(wStorageName);
	delete[] wStorageName;

	if( S_OK != hr )
	{
		Msg(0, "ERR: Can't delete element '%s' - %s", (LPCSTR)StorageName, GetStgErrorString(hr));
		throw NULL;
	}
	return true;
}


void CStorage::CloseStream()
{
	if( pStream != NULL )
	{
		pStream->Release();
		pStream = NULL;
	}
}

bool CStorage::OpenStream(GCString& StreamName)
{
	CloseStream();

	IStorage* Parent = GetStorage();
	if( Parent == NULL ) return false;

	if( StreamName.Find("\\") == -1 )
	{
		pStream = ::OpenStream(Parent, StreamName);
		this->StreamName = StreamName;
	}
	else
	{
		CStringArray* name_parts = SplitPath(StreamName);
		GCString RealName = name_parts->GetAt(name_parts->GetSize()-1);
		name_parts->SetSize(name_parts->GetSize()-1);
		if( !Open(name_parts) ) return false;
		delete name_parts;
		pStream = ::OpenStream(GetStorage(), RealName);
		this->StreamName = RealName;
	}

	if( pStream == NULL ) return false;
	
	StreamInfo.UpdateInfo((char*)(LPCSTR)(this->StreamName));
	StreamInfo.ReadSize(pStream);

	return true;
}

bool CStorage::StorageExist(GCString& StorageName)
{
	IStorage* pStorage = ::OpenStorage(GetStorage(), StorageName);
	if( pStorage != NULL )
	{
		pStorage->Release();
		return true;
	}
	return false;
}

bool CStorage::StreamExist(GCString& StreamName)
{
	IStream* pStream = ::OpenStream(GetStorage(), StreamName);
	if( pStream != NULL )
	{
		pStream->Release();
		return true;
	}
	return false;
}

long CStorage::GetStreamSize(GCString& StreamName)
{
	long size;
	if( !OpenStream(StreamName) ) return -1L;
	size = ::GetStreamSize(pStream).LowPart;
	CloseStream();
	return size;
}

void CStorage::Rewind()
{
	LARGE_INTEGER SeekZero = {0, 0};
	pStream->Seek(SeekZero, 0, NULL);
}

void CStorage::GetCleanStream()
{
	if( StreamInfo.Packed )
	{
		IStream* pUnpackedStream;
		CreateStreamOnHGlobal(NULL, TRUE, &pUnpackedStream);

		iLibEngine->pkDecompress(pStream, pUnpackedStream);

		pStream->Release();
		pStream = pUnpackedStream;
	}

	if( StreamInfo.HaveSize )
	{
		StreamInfo.ReadSize(pStream); //размер читаем до обработки, чтобы определить длину длины в заголовке
		
		LARGE_INTEGER Offset = {0, 0};
		Offset.LowPart = StreamInfo.ContentOffset;
		pStream->Seek(Offset, STREAM_SEEK_SET, NULL);
		
		IStream* CleanStream;
		CreateStreamOnHGlobal(NULL, TRUE, &CleanStream);
		
		ULARGE_INTEGER BytesToCopy = {0, 0};
		BytesToCopy.LowPart = StreamInfo.FullSize - StreamInfo.SizeBytes - StreamInfo.SizeOffset;
		pStream->CopyTo(CleanStream, BytesToCopy, NULL, NULL);

		Offset.LowPart = 0;
		CleanStream->Seek(Offset, 0, NULL);

		pStream->Release();
		pStream = CleanStream;
	}
	else
	{
		StreamInfo.ReadSize(pStream);
	}

}

bool CStorage::CopyToFile(FILE* File, unsigned long bytes_to_copy)
{
	if( File == NULL ) return false;

	BYTE buff[BUFFER_SIZE];
	unsigned long BytesToRead = BUFFER_SIZE, BytesRead = 0;
	DWORD BytesWritten = 0;

	if( bytes_to_copy <= 0 ) Rewind();

	do
	{
		if( bytes_to_copy > 0 && BytesToRead > bytes_to_copy )
			BytesToRead = bytes_to_copy;

		pStream->Read(buff, BytesToRead, &BytesRead);

		if( BytesRead > 0 )
		{
			BytesWritten = fwrite(buff, 1, BytesRead, File);
			if( BytesWritten < BytesRead ) return false;
		}
		
		if( bytes_to_copy > 0 )
		{
			if( bytes_to_copy <= BytesRead ) break;
			bytes_to_copy -= BytesRead;
		}

	} while( BUFFER_SIZE == BytesRead );
	return true;
}

bool CStorage::CopyToFile(GCString& FileName, bool OnlyRestOfSream)
{
	if( pStream == NULL ) return false;

	FILE *File;
	char* MsgPrefix = "";
	unsigned long bytes_to_copy = 0;

	//Разберёмся с картинками
	if( StreamInfo.Picture )
	{
		StreamInfo.DeterminePictureType(pStream);
		FileName += StreamInfo.PictureType;
	}
	else if( StreamInfo.PictureGallery )
	{
		return ExtractPictureGallery(FileName);
	}

	if( OnlyRestOfSream )
	{
		LARGE_INTEGER Offset = {0, 0};
		ULARGE_INTEGER Pos = {0, 0};
		pStream->Seek(Offset, STREAM_SEEK_CUR, &Pos);
		bytes_to_copy = StreamInfo.FullSize - Pos.LowPart;
		if( bytes_to_copy == 0 ) return true;
	}

	AddToExtractedList(FileName, NULL);

	if( FileExist(FileName) )
	{
		if( bytes_to_copy == 0 )
		{
			if( FullCompare(FileName) == 0 ) return true;
		}
		else if( Compare(FileName, bytes_to_copy) == 0 )
		{
			return true;
		}
		MsgPrefix = "UPD";
	}
	else
	{
		MsgPrefix = "NEW";
	}
	SetIOPre(MsgPrefix, FileName);
	File = fopen(FileName, "wb");
	if( !CopyToFile(File, bytes_to_copy) )
	{
		Msg(0, "Error writing to file '%s' - %s", FileName, strerror(errno));
		throw NULL;
	}

	fclose(File);

	Msg(1, "%s: %s", MsgPrefix, FileName);
	return true;
}

bool CStorage::CopyToFile(GCString& StreamName, GCString& FileName)
{
	if( !OpenStream(StreamName) ) return false;

	GetCleanStream();

	if( StreamInfo.PictureGallery )
	{
		return ExtractPictureGallery(FileName);
	}

	if( StreamInfo.HaveSize )
	{
		if( (StreamInfo.Size != (StreamInfo.FullSize - StreamInfo.SizeBytes - StreamInfo.SizeOffset)) )
			Msg(0, "WARNING: Checksum %ib != real size %ib. Stream '%s\\%s'. File '%s'", 
				(int)StreamInfo.Size, 
				(int)(StreamInfo.FullSize - StreamInfo.SizeBytes - StreamInfo.SizeOffset),
				FullPath(), StreamName,
				FileName
				);
	}

	return CopyToFile(FileName);
}

bool CStorage::Extract(GCString& DestDir, bool WithContainerContents)
{
	IEnumSTATSTG *ppenum;
	STATSTG stat;
	unsigned long uCount;
	GCString StreamName, FileName, cc("Container.Contents");

	CreateDirectory(DestDir);

	GetStorage()->EnumElements(0, NULL, 0, &ppenum);
	while ( S_OK == ppenum->Next(1, &stat, &uCount) )
	{
		StreamName.Format("%S", stat.pwcsName);
		FileName.Format("%s\\%s", DestDir, StreamName);

		if( (stat.type & STGTY_STORAGE) != 0 )
		{
			if( Open(StreamName) )
			{
				Extract(FileName, WithContainerContents);
				Close();
			}
		}
		else
		{
			if( !WithContainerContents && StreamName == cc ) continue;
			CopyToFile(StreamName, FileName);
		}
	}
	ppenum->Release();

	return true;
}

unsigned long CStorage::AppendFile(IStream* pStream, FILE* File)
{
	char buff[BUFFER_SIZE];
	size_t BytesRead;
	unsigned long TotalBytesRead = 0;

	LARGE_INTEGER offset;
	ULARGE_INTEGER pos = ::GetStreamSize(pStream);
	offset.QuadPart = pos.QuadPart;
	pStream->Seek(offset, STREAM_SEEK_SET, &pos);

	do
	{
		BytesRead = fread(buff, 1, sizeof(buff), File);
		pStream->Write(buff, BytesRead, NULL);
		TotalBytesRead += BytesRead;
	} while( BytesRead == sizeof(buff) );

	return TotalBytesRead;
}

unsigned long CStorage::AppendFile(IStream* pStream, GCString& FileName)
{
	FILE* File = fopen(FileName, "rb");
	if( File == NULL ) return 0;

	unsigned long bytes_appended = AppendFile(pStream, File);
	fclose(File);
	return bytes_appended;
}

bool CStorage::FromFile(FILE* File, LPCSTR FileName)
{
	IStream* pStreamText;

	if( StreamInfo.Packed )
		CreateStreamOnHGlobal(NULL, TRUE, &pStreamText);
	else
		pStreamText = pStream;

	if( StreamInfo.HaveSize )
	{
		StreamInfo.ReadFileSize(File); //Определим, сколько байт 0xFF и размер размера
		WriteSizeToStream(pStreamText);
	}

	rewind(File);
	AppendFile(pStreamText, File);

	if( StreamInfo.Packed )
	{
		LARGE_INTEGER SeekZero = {0, 0};
		pStreamText->Seek(SeekZero, STREAM_SEEK_SET, NULL);

		iLibEngine->pkCompress(pStreamText, pStream);

		pStreamText->Release();
	}

	if( FileName != NULL ) Msg(2, "CPY: '%s'", FileName);

	return true;
}

bool CStorage::FromFile(GCString& FileName)
{
	FILE* File;
	File = fopen((LPCSTR)FileName, "rb");
	if( File == NULL ) return false;

	FromFile(File, FileName);
	fclose(File);
	return true;
}

bool CStorage::StreamFromFile(GCString& StreamName, GCString& FileName)
{
	if( !FileExist(FileName) ) return false;

	StreamInfo.UpdateInfo(StreamName);
	if( StreamInfo.Picture )
	{
		StreamInfo.DeterminePictureType(FileName);
		if( !StreamInfo.PictureType.IsEmpty() )
		{
			int offset = StreamName.GetLength() - StreamInfo.PictureType.GetLength();
			StreamName.SetAt(offset, '\0'); //уберём расширение
		}
	}
	else if( StreamInfo.PictureGallery )
	{
		return ImportPictureGallery(StreamName, FileName);
	}

	GCString PictureType = StreamInfo.PictureType; //Сохраним расширение
	if( !CreateStream(StreamName) ) return false;
	StreamInfo.PictureType = PictureType;

	return FromFile(FileName);
}

void CStorage::AddZeroByte()
{
	BYTE byte = 0;
	if( pStream == NULL ) return;
	pStream->Write(&byte, 1, NULL);
}

bool CStorage::StreamFromString(GCString StreamName, GCString& String)
{
	if( !CreateStream(StreamName) ) return false;

	StreamInfo.ReadSize(String);
	
	IStream* pStreamText;
	if( StreamInfo.Packed )
		CreateStreamOnHGlobal(NULL, TRUE, &pStreamText);
	else
		pStreamText = pStream;

	WriteSizeToStream(pStream);
	pStream->Write((LPCSTR)String, String.GetLength(), NULL);

	if( StreamInfo.Packed )
	{
		LARGE_INTEGER SeekZero = {0, 0};
		pStream->Seek(SeekZero, STREAM_SEEK_SET, NULL);

		iLibEngine->pkCompress(pStreamText, pStream);
		
		pStreamText->Release();
	}

	return true;
}

bool CStorage::StorageFromDir(GCString& StorageName, GCString& SrcDir)
{
	GCString FileName;
	struct _finddata_t find_data;
	long hFind;

	if( !DirectoryExist(SrcDir) ) return false;

	if( !Create(StorageName) )
	{
		Msg(0, "ERR: Can't create storage %s", StorageName);
		return false;
	}

	GalleryImported = false;
	hFind = _findfirst(SrcDir+"\\*", &find_data);
	if( hFind != -1 )
	{
		do
		{
			if( IsIgnoredFile(find_data.name) ) continue;
			
			FileName.Format("%s\\%s", SrcDir, find_data.name);

			if( (find_data.attrib & _A_SUBDIR) != 0 )
			{
				StorageFromDir(GCString(find_data.name), FileName);
				Close();
			}
			else
				StreamFromFile(GCString(find_data.name), FileName);
		} while( _findnext(hFind, &find_data) == 0 );
		_findclose(hFind);
	}

	return true;
}

void CStorage::WriteSizeToStream(IStream* pStream)
{
	BYTE byte_ff = 0xFF;

	if( !StreamInfo.HaveSize ) return;

	if( StreamInfo.Picture )
	{
		//Если формат неизвестен - поток был выгружен As Is. Размер писать не нужно
		if( StreamInfo.PictureType.IsEmpty() ) return;

		unsigned char signature[] = {0x6c, 0x74, 0x00, 0x00};
		pStream->Write(signature, sizeof(signature), NULL);
	}
	else
	{
		//Начальные байты 0xFF
		for( unsigned int i = 0; i < StreamInfo.SizeOffset; i++ ) 
			pStream->Write(&byte_ff, 1, NULL);
	}


	switch( StreamInfo.SizeBytes )
	{
		BYTE bSize;
		WORD wSize;
		DWORD dwSize;
		case 1:
			bSize = (BYTE)StreamInfo.Size;
			pStream->Write(&bSize, StreamInfo.SizeBytes, NULL);
			break;
		case 2:
			wSize = (WORD)StreamInfo.Size;
			pStream->Write(&wSize, StreamInfo.SizeBytes, NULL);
			break;
		case 4:
			dwSize = StreamInfo.Size;
			pStream->Write(&dwSize, StreamInfo.SizeBytes, NULL);
			break;
		default:
			Msg(0, "ERR: Don't know how to write size of stream (%i bytes)", (int)StreamInfo.SizeBytes);
			throw NULL;
			break;
	}
}


//Если файл короче потока, возвращает -1
//Если файл длиннее потока, возвращает 1
//Если размеры одинаковы, то возвращается результат memcmp()
int CStorage::Compare(FILE* File, unsigned long bytes_to_compare)
{
	//save stream position
	LARGE_INTEGER seek_offset = {0, 0};
	ULARGE_INTEGER pos;
	pStream->Seek(seek_offset, STREAM_SEEK_CUR, &pos);

	void* buff_Stream[BUFFER_SIZE];
	void* buff_File[BUFFER_SIZE];
	DWORD BytesReadF = 0, BytesReadS = 0;
	int cmp = 0;

	do
	{
		BytesReadF = fread(buff_File, 1, BUFFER_SIZE, File);
		pStream->Read(buff_Stream, BUFFER_SIZE, &BytesReadS);

		if( bytes_to_compare > 0 )
		{
			if( BytesReadF > bytes_to_compare ) BytesReadF = bytes_to_compare;
			if( BytesReadS > bytes_to_compare ) BytesReadS = bytes_to_compare;
		}
		
		if( BytesReadF < BytesReadS ) 
		{
			cmp = -1;
			break;
		}
		if( BytesReadF > BytesReadS ) 
		{
			cmp = 1;
			break;
		}

		cmp = memcmp(buff_File, buff_Stream, BytesReadF);
		if( cmp != 0 ) break;

		if( bytes_to_compare > 0 )
		{
			if( bytes_to_compare <= BytesReadF ) break;
			bytes_to_compare -= BytesReadF;
		}
	} while( BUFFER_SIZE == BytesReadF );
	
	//restore stream position
	seek_offset.QuadPart = pos.QuadPart;
	pStream->Seek(seek_offset, STREAM_SEEK_SET, NULL);

	return cmp;
}

int CStorage::Compare(GCString& FileName, unsigned long bytes_to_compare)
{
	if( !FileExist(FileName) ) return -1;

	FILE* File = fopen(FileName, "rb");
	if (File == NULL ) 
	{ 
		Msg(0, "ERR: Невозможно открыть файл '%s' - %s", FileName, strerror(errno));
		throw NULL;
	}

	int cmp = Compare(File, bytes_to_compare);

	fclose(File);

	return cmp;
}

int CStorage::CompareWithString(GCString& str2)
{
	//save stream position
	LARGE_INTEGER seek_offset = {0, 0};
	ULARGE_INTEGER pos;
	pStream->Seek(seek_offset, STREAM_SEEK_CUR, &pos);

	DWORD BytesRead;
	char c1, c2, *p2 = (char*)(LPCSTR)str2;
	int cmp_res = 0;
	do
	{
		pStream->Read(&c1, sizeof(c1), &BytesRead);
		if( BytesRead < sizeof(c1) )
		{
			cmp_res = 1;
			break;
		}
		//skip \r \n in stream
		if( c1 == '\r' || c1 == '\n' ) continue;

		//skip \r \n in string
		while( *p2 == '\r' || *p2 == '\n' ) p2++;
		c2 = *p2++;
		if( c2 == '\0' )
			break;

		if( c1 < c2 )
			cmp_res = -1;
		else if( c1 > c2 )
			cmp_res = 1;

	} while( cmp_res == 0 );

	//restore stream position
	seek_offset.QuadPart = pos.QuadPart;
	pStream->Seek(seek_offset, STREAM_SEEK_SET, NULL);

	return cmp_res;
}

int CStorage::FullCompare(GCString& FileName)
{
	if( !FileExist(FileName) ) return 0;
	long FileSize = GetFileSize(FileName);
	int cmp = -1;
	
	if ( FileSize != StreamInfo.Size )
	{
		//Размеры не совпали
		Msg(2, "File size is: %d - %s", FileSize, FileName);
		Msg(2, "Strm size is: %d", StreamInfo.Size);
		cmp = (FileSize < StreamInfo.Size ? -1 : 1);
	}
	else
	{
		Rewind();
		cmp = Compare(FileName, 0);
	}

	return cmp;
}

void CStorage::ParseStream(CMMSObject* Object, GCString& StreamName, bool ParseWitnNoIDs)
{
	FILE* File;

	if( !OpenStream(StreamName) ) return;
	GetCleanStream();
	char* fname = NULL;	
	fname =  _tempnam( NULL, "tmp_");//"c:\\tmp"
	File = fopen(fname, "w+b");	
	if( File == NULL )
	{
		Msg(0, "ERR: Невозможно создать временный файл: %s", strerror(errno));
		throw NULL;
	}

	GCString FullStreamPath = FullPath()+"\\"+StreamName;

	if( !CopyToFile(File, 0) )
	{
		Msg(0, "ERR: Не могу извлечь '%s' во временный файл: %s", FullStreamPath, strerror(errno));
		throw NULL;
	}
	fclose(File);

	GCString FName(fname);
	CLexer* Lexer = new CLexer(FName);
	try
	{
		Object->ParseMetadata(*Lexer);
	}
	catch(...)
	{
		Msg(0, "\tпредыдущая ошибка возникла в потоке %s", FullStreamPath);
		throw NULL;
	}
	delete Lexer;

	remove(fname);
	if(fname!=NULL)
	{
		free(fname);
		fname = NULL;
	}	
}

void CStorage::AddToExtractedList(GCString& FullFName, void* object)
{
	ExtractedFiles->SetAt((LPCSTR)FullFName, object);
	if( object != NULL ) ExtractedObjects->SetAt(object, NULL);
}

bool CStorage::ObjectExtracted(void* object)
{
	void *ptr;
	return (bool)ExtractedObjects->Lookup(object, ptr);
}

void CStorage::RemoveDirectory(GCString& Path)
{
	struct _finddata_t find_data;
	long hFind;
	GCString Pattern(Path);
	GCString FileName;
	
	Pattern += "\\*.*";
	
	hFind = _findfirst(Pattern, &find_data);
	if( hFind != -1 )
	{
		do
		{
			if( IsIgnoredFolder(find_data.name) ) continue;

			FileName.Format("%s\\%s", Path, find_data.name);
			SetIOPre("DEL", FileName);
			if( (find_data.attrib & _A_SUBDIR) != 0 ) // подкаталог
			{
				RemoveDirectory(FileName);
				_rmdir((LPCSTR)FileName);
			}
			else // во всяких каталогах .svn могут оказаться и файлы тоже
			{
				// файлы внутри .svn могут оказаться ридонли
				_chmod((LPCSTR)FileName, _S_IREAD | _S_IWRITE);
				remove((LPCSTR)FileName);
			}
			Msg(1, "DEL: %s", (LPCSTR)FileName);
		} while( _findnext(hFind, &find_data) == 0 );
		_findclose(hFind);
		
		_rmdir((LPCSTR)Path);
	}
}

int CStorage::PruneDirectory(GCString& Path, bool NoEmptyFolders)
{
	struct _finddata_t find_data;
	long hFind;
	GCString Pattern(Path);
	GCString FileName;
	void *ptr;
	
	int result = 0;

	Pattern += "\\*.*";
	
	hFind = _findfirst(Pattern, &find_data);
	if( hFind != -1 )
	{
		do
		{
			if( IsIgnoredFile(find_data.name) ) continue;

			FileName.Format("%s\\%s", Path, find_data.name);

			if( (find_data.attrib & _A_SUBDIR) != 0 ) // подкаталог
			{
				int FilesInSubFolder = PruneDirectory(FileName, NoEmptyFolders); 
				// в текущем каталоге могут быть только подпапки. А вот в них...
				result += FilesInSubFolder;
				if( (0 == FilesInSubFolder) && NoEmptyFolders )
				{
					SetIOPre("DEL", FileName);
					RemoveDirectory(FileName);
					Msg(1, "DEL: %s", (LPCSTR)FileName);
				}
			}
			else if( !ExtractedFiles->Lookup((LPCSTR)FileName, ptr) ) // при декомпиляции не было такого файла
			{
				SetIOPre("DEL", FileName);
				remove((LPCSTR)FileName);
				Msg(1, "DEL: %s", (LPCSTR)FileName);
			}
			else // был такой файл, сосчитаем его
				result++;
			
		} while( _findnext(hFind, &find_data) == 0 );
		_findclose(hFind);
	}
	
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////
ULARGE_INTEGER GetStreamSize(IStream* pStream)
{
	STATSTG stat;
	pStream->Stat(&stat, STATFLAG_NONAME);
	return stat.cbSize;
}


IStorage* OpenStorage(IStorage* pParentStorage, wchar_t* wStorName)
{
	IStorage* pStorage;
	pParentStorage->OpenStorage(wStorName, NULL, STGM_SHARE_EXCLUSIVE | STGM_READWRITE, NULL, 0, &pStorage);
	//pParentStorage->OpenStorage(wStorName, NULL, STGM_SHARE_DENY_WRITE | STGM_READ, NULL, 0, &pStorage);
	return pStorage;
}

IStorage* OpenStorage(IStorage* pParentStorage, GCString& StorName)
{
	IStorage* pStorage;

	wchar_t* wStorName = new wchar_t[StorName.GetLength()+1];
	swprintf(wStorName, L"%S", StorName);
	pStorage = OpenStorage(pParentStorage, wStorName);
	delete[] wStorName;

	return pStorage;
}

IStream* OpenStream(IStorage* pParentStorage, wchar_t* wStreamName)
{
	IStream* pStream;
	pParentStorage->OpenStream(wStreamName, NULL, STGM_SHARE_EXCLUSIVE, 0, &pStream);
	return pStream;
}

IStream* OpenStream(IStorage* pParentStorage, GCString& StreamName)
{
	IStream* pStream;
	
	wchar_t* wStreamName = new wchar_t[StreamName.GetLength()+1];
	swprintf(wStreamName, L"%S", StreamName);
	pStream = OpenStream(pParentStorage, wStreamName);
	delete[] wStreamName;

	return pStream;
}


bool StringDifferFromFile(GCString& String, GCString& FileName)
{

	if( !FileExist(FileName) ) return true;

	FILE* File = fopen(FileName, "rb");
	if (File == NULL ) 
	{ 
		Msg(0, "ERR: Can\t open file '%s' - %s", FileName, strerror(errno));
		throw NULL;
    }

	long FileSize;
	long BytesRead;
	void* buff = NULL;
	
	fseek(File, 0, SEEK_END);
	FileSize = ftell(File);
	rewind(File);

	if ( String.GetLength() != FileSize ) goto FilesDiffer;

	buff = (void*) new char[FileSize];
	BytesRead = fread(buff, 1, FileSize, File);
	if( memcmp(buff, (LPCSTR)String, FileSize) != 0 ) goto FilesDiffer;

	delete[] buff;
	fclose(File);
	return false;

FilesDiffer:
	if( buff != NULL ) delete[] buff;
	fclose(File);
	return true;
}

bool CStorage::CopyStringToFile(GCString& String, GCString& FileName)
{
	char* MsgPrefix;

	AddToExtractedList(FileName, NULL);

	if( FileExist(FileName) )
		if( !StringDifferFromFile(String, FileName) )
			return true;
		else
			MsgPrefix = "UPD";
	else
		MsgPrefix = "NEW";
	SetIOPre(MsgPrefix, FileName);
	//Открываем файл как двоичный. О формате конца строк нужно заботиться во 
	//время формирования текста потока. А иначе потом сравнивать хреново.
	FILE *File = fopen(FileName, "wb");

	fwrite((LPCSTR)String, 1, String.GetLength(), File);
	fclose(File);
	
	Msg(1, "%s: %s", MsgPrefix, FileName);

	return true;
}



