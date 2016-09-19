
#ifndef ORDERING_H
#define ORDERING_H

#include "Declarations.h"
#include "Strings.h"

class CObjectOrdering
{
private:
	int nValues;
	FILE* OrderFile;
	char ReadBuffer[4096];
	char** Values;
	int nStringsAdded;

public:
	GCString FormatString;
	GCString Order;

public:
	//При создании объекта для чтения надо указывать полное количество значений 
	//в строке файла, а для объекта на запись - на единицу меньше.
	CObjectOrdering(int nValues);
	~CObjectOrdering();

public:
	bool OpenFile(GCString& Dir, MMSObjectType Type = NT_ObjectOrdering);
	bool WriteFile(CStorage& Storage, GCString& Dir, MMSObjectType Type = NT_ObjectOrdering);

	int Add(int ID, ...);
	int Add(LPCSTR ID, ...);
	bool IsEmpty() {return nStringsAdded == 0;}

	bool ReadString();
	char* GetValue(int i);
};

#endif