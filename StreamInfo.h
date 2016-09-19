
#ifndef STREAM_INHO_H
#define STREAM_INHO_H

#include "Declarations.h"
#include "Strings.h"

class CStreamInfo
{
public:
	bool Packed; //Поток упакован zLibEngine
	bool HaveSize; //В потоке должна содержаться его длина

	bool MainMetadataStream;
	
	bool Picture;
	bool PictureGallery;
	GCString PictureType;

	unsigned long SizeOffset; //В начале потоков с размером зачем-то содержатся байты 0xFF
	unsigned long SizeBytes;  //Длина размера может быть разной
	unsigned long ContentOffset; //Начало собственно данных
	unsigned long FullSize;
	unsigned long Size; //Размер потока без служебной информации

public:
	CStreamInfo();

	void UpdateInfo(LPCSTR StreamName);
	void UpdateInfo(const wchar_t* wStreamName);

	unsigned long ReadSize(IStream* pStream); //Читает размер потока (если он с размером) или вычисляет его
	unsigned long ReadSize(GCString& String); //Вычисляет размер из строки
	unsigned long ReadSizeOfCleanStream(IStream* pStream); //Читает размер очищенного потока
	unsigned long ReadFileSize(FILE *File); //Читает размер файла

	//Сюда передаётся поток уже без размера
	void DeterminePictureType(IStream* pStream);
	//Тип картинки в файле будем определять по расширению
	void DeterminePictureType(GCString& FileName);

protected:
	void DetermineSizeFormat(unsigned long FullSize);
	void DetermineFileSizeFormat(unsigned long FullSize);
	
private:
	void Reset();
};

#endif