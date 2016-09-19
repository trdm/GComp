
#include "Declarations.h"
#include "CommonFunctions.h"
#include "Storage.h"
#include "MMS.h"
#include "NameTable.h"


DWORD ReadBMPSize(IStream* pStream)
{
	//save stream position
	LARGE_INTEGER seek_offset = {0, 0};
	ULARGE_INTEGER pos;
	pStream->Seek(seek_offset, STREAM_SEEK_CUR, &pos);

	DWORD image_start, image_size;

	seek_offset.LowPart = 10; //10 байт от начала картинки
	pStream->Seek(seek_offset, STREAM_SEEK_CUR, NULL);
	pStream->Read(&image_start, 4, NULL);
	seek_offset.LowPart = 20; //34 байт от начала картинки
	pStream->Seek(seek_offset, STREAM_SEEK_CUR, NULL);
	pStream->Read(&image_size, 4, NULL);

	//restore stream position
	seek_offset.QuadPart = pos.QuadPart;
	pStream->Seek(seek_offset, STREAM_SEEK_SET, NULL);

	return image_start + image_size;
}

bool CStorage::ExtractPictureGallery(GCString& FileName)
{
	const int HdrSize = 28;
	int TailSize;
	int BMP_size;

	GCString HdrName, TailName;
	char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	LARGE_INTEGER seek_offset = {0, 0};

	_splitpath(FileName, drive, dir, fname, ext);
	HdrName.Format("%s%s%s%s.head", drive, dir, fname, ext);
	TailName.Format("%s%s%s%s.tail", drive, dir, fname, ext);
	FileName += ".bmp";

	Rewind();

	//Gallery Header
	if( Compare(HdrName, HdrSize) != 0 )
	{
		SetIOPre("UPD", HdrName);
		FILE* HdrFile = fopen(HdrName, "wb");
		CopyToFile(HdrFile, HdrSize);
		fclose(HdrFile);
		Msg(1, "UPD: '%s' -> '%s'", StreamName, HdrName);
	}
	AddToExtractedList(HdrName, NULL);

	//Gallery bitmap
	seek_offset.LowPart = HdrSize;
	pStream->Seek(seek_offset, STREAM_SEEK_SET, NULL);
	BMP_size = ReadBMPSize(pStream);
	TailSize = StreamInfo.FullSize - HdrSize - BMP_size;
	
	if( Compare(FileName, BMP_size) != 0 )
	{
		SetIOPre("UPD", FileName);
		FILE* BMPFile = fopen(FileName, "wb");
		CopyToFile(BMPFile, BMP_size);
		fclose(BMPFile);
		Msg(1, "UPD: '%s' -> '%s'", StreamName, FileName);
	}
	AddToExtractedList(FileName, NULL);

	//Gallery tail
	seek_offset.LowPart = HdrSize + BMP_size;
	pStream->Seek(seek_offset, STREAM_SEEK_SET, NULL);
	if( Compare(TailName, TailSize) != 0 )
	{
		SetIOPre("UPD", TailName);
		FILE* TailFile = fopen(TailName, "wb");
		CopyToFile(TailFile, TailSize);
		fclose(TailFile);
		Msg(1, "UPD: '%s' -> '%s'", StreamName, TailName);
	}
	AddToExtractedList(TailName, NULL);

	return true;
}

bool CStorage::ImportPictureGallery(GCString& StreamName, GCString& FileName)
{
	if( GalleryImported ) return true;

	GCString FileNameNoExt = FileName;
	GCString HdrName, BmpName, TailName;
	char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];

	//Уберём расширения
	int i = FileNameNoExt.ReverseFind('.');
	FileNameNoExt.SetAt(i, '\0');
	i = StreamName.ReverseFind('.');
	StreamName.SetAt(i, '\0');

	_splitpath(FileNameNoExt, drive, dir, fname, ext);

	HdrName.Format("%s%s%s%s.head", drive, dir, fname, ext);
	BmpName.Format("%s%s%s%s.bmp", drive, dir, fname, ext);
	TailName.Format("%s%s%s%s.tail", drive, dir, fname, ext);

	if( !CreateStream(StreamName) ) return false;
	if( !FromFile(HdrName) )
	{
		Msg(0, "ERR: Невозможно открыть файл %s: %s", HdrName, strerror(errno));
		throw NULL;
	}
	if( 0 == AppendFile(pStream, BmpName) )
	{
		Msg(0, "ERR: Невозможно открыть файл %s: %s", BmpName, strerror(errno));
		throw NULL;
	}
	if( 0 == AppendFile(pStream, TailName) )
	{
		Msg(0, "ERR: Невозможно открыть файл %s: %s", TailName, strerror(errno));
		throw NULL;
	}

	Msg(2, "CPY: '%s'  >> '%s'", BmpName, StreamName);
	Msg(2, "CPY: '%s' >> '%s'", TailName, StreamName);

	GalleryImported = true;
	return true;
}
