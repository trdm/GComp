
#include <io.h>
#include "StreamInfo.h"
#include "CommonFunctions.h"

void CStreamInfo::Reset()
{
	Packed = false;
	MainMetadataStream = false;
	PictureGallery = false;
	Picture = false;
	PictureType = "";
	HaveSize = false;
	SizeOffset = 0;
	SizeBytes = 0;
	ContentOffset = 0;
	FullSize = 0;
	Size = 0;
}

CStreamInfo::CStreamInfo()
{
	Reset();
}

void CStreamInfo::UpdateInfo(LPCSTR StreamName)
{
	Reset();

	if     ( 0 == stricmp("MD Programm text", StreamName)     ) Packed = true;
	else if( 0 == stricmp("Dialog Stream", StreamName)        ) HaveSize = true;
	else if( 0 == stricmp("Inplace description", StreamName)  ) Packed = true;
	else if( 0 == _strnicmp("__Picture.1", StreamName, 11)    ) 
	{
		Packed = true;
		Picture = true;
		HaveSize = true;
	}
	else if( 0 == _strnicmp("__Picture.2", StreamName, 11)    ) 
	{
		Packed = true;
		Picture = true;
		HaveSize = false;
	}
	else if( 0 == _strnicmp("Gallery", StreamName, 7)         )
	{
		PictureGallery = true;
	}
	else if( 0 == stricmp("Main MetaData Stream", StreamName) ) 
	{
		HaveSize = true;
		MainMetadataStream = true;
	}
}

void CStreamInfo::DetermineSizeFormat(unsigned long FullSize)
{
	if( !HaveSize ) return;

	if( Picture )
	{
		SizeOffset = 4;
		SizeBytes = 4;
	}
	else
	{
		if( FullSize == 0 )
		{
			SizeOffset = 0;
			SizeBytes = 1;
			Size = 0;
		}
		else
		{
			if( FullSize < (0xFF + 1) )
			{
				SizeOffset = 0;
				SizeBytes = 1;
			}
			else if( FullSize < (0xFFFF + 2) )
			{
				SizeOffset = 1;
				SizeBytes = 2;
			}
			else
			{
				SizeOffset = 3;
				SizeBytes = 4;
			}
		}
	}

	ContentOffset = SizeOffset + SizeBytes;
}

void CStreamInfo::DetermineFileSizeFormat(unsigned long FullSize)
{
	if( !HaveSize ) return;

	if( Picture )
	{
		SizeOffset = 4;
		SizeBytes = 4;
	}
	else
	{
		if( FullSize == 0 )
		{
			SizeOffset = 0;
			SizeBytes = 1;
			Size = 0;
		}
		else
		{
			if( FullSize <= (0xFF - 1) )
			{
				SizeOffset = 0;
				SizeBytes = 1;
			}
			else if( FullSize <= (0xFFFF - 2) )
			{
				SizeOffset = 1;
				SizeBytes = 2;
			}
			else
			{
				SizeOffset = 3;
				SizeBytes = 4;
			}
		}
	}

	ContentOffset = SizeOffset + SizeBytes;
}

unsigned long CStreamInfo::ReadSize(GCString& String)
{
	Size = FullSize = String.GetLength();
	DetermineFileSizeFormat(FullSize);
	return Size;
}


unsigned long CStreamInfo::ReadSize(IStream* pStream)
{
	LARGE_INTEGER Offset = {0, 0};
	ULARGE_INTEGER EndPos = {0, 0};

	Offset.LowPart = 0;
	pStream->Seek(Offset, STREAM_SEEK_END, &EndPos);
	FullSize = EndPos.LowPart;
	pStream->Seek(Offset, STREAM_SEEK_SET, NULL);

	SizeOffset = 0;
	SizeBytes = 0;
	ContentOffset = 0;

	if( HaveSize )
	{
		DetermineSizeFormat(FullSize);
		if( FullSize > 0 )
		{
			Offset.LowPart = SizeOffset;
			pStream->Seek(Offset, STREAM_SEEK_SET, NULL);
			switch( SizeBytes )
			{
				case 1:
					BYTE bSize;
					pStream->Read(&bSize, SizeBytes, NULL);
					Size = bSize;
					break;
				case 2:
					WORD wSize;
					pStream->Read(&wSize, SizeBytes, NULL);
					Size = wSize;
					break;
				case 4:
					DWORD dwSize;
					pStream->Read(&dwSize, SizeBytes, NULL);
					Size = dwSize;
					break;
				default:
					STATSTG StatSgts;
					pStream->Stat(&StatSgts, STATFLAG_DEFAULT);
					Msg(0, "ERR: Don't know how to read size of stream '%S' (%i bytes)", StatSgts.pwcsName, (int)SizeBytes);
					throw NULL;
					break;
			}
		}
	}
	else
	{
		Size = FullSize;
		ContentOffset = 0;
	}

	//Спозиционируемся на начало потока
	Offset.LowPart = 0;
	pStream->Seek(Offset, STREAM_SEEK_SET, NULL);

	return Size;
}

unsigned long CStreamInfo::ReadFileSize(FILE* File)
{
	SizeOffset = 0;
	SizeBytes = 0;
	ContentOffset = 0;

	fseek(File, 0, SEEK_END);
	FullSize = ftell(File);
	fseek(File, 0, SEEK_SET);

	Size = FullSize;

	DetermineFileSizeFormat(FullSize);
	ContentOffset = SizeOffset + SizeBytes;

	return Size;
}

void CStreamInfo::DeterminePictureType(IStream* pStream) //этот метод работает при декомпиляции
{
	if ( !HaveSize )
	{
		PictureType = ".dat"; // по крайней мере file на них говорит data :))
		return;
	}
	
	LARGE_INTEGER Offset = {0, 0};
	unsigned char id_bytes[3];

	pStream->Seek(Offset, STREAM_SEEK_SET, NULL);
	pStream->Read(id_bytes, sizeof(id_bytes), NULL);
	pStream->Seek(Offset, STREAM_SEEK_SET, NULL);

	if( id_bytes[0] == 0x42 && id_bytes[1] == 0x4d )
		PictureType = ".bmp";
	else if( id_bytes[0] == 0 && id_bytes[1] == 0 && id_bytes[2] != 0 )
		PictureType = ".ico";
	else if( id_bytes[0] == 0xd7 && id_bytes[1] == 0xcd && id_bytes[2] == 0xc6 )
		PictureType = ".wmf";
}

void CStreamInfo::DeterminePictureType(GCString& FileName) // этот метод работает при компиляции
{
	char ext[_MAX_EXT];
	
	_splitpath(FileName, NULL, NULL, NULL, ext);
	if     ( stricmp(ext, ".bmp") == 0 ) PictureType = ".bmp";
	else if( stricmp(ext, ".ico") == 0 ) PictureType = ".ico";
	else if( stricmp(ext, ".wmf") == 0 ) PictureType = ".wmf";
	else if( stricmp(ext, ".dat") == 0 ) PictureType = ".dat";
}