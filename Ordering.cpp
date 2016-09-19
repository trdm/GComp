
#include "Ordering.h"
#include "NameTable.h"
#include "Storage.h"
#include "CommonFunctions.h"


CObjectOrdering::CObjectOrdering(int nValues)
{
	OrderFile = NULL;
	this->nValues = nValues;
	Values = new char*[nValues];
	memset(Values, 0, nValues);
	nStringsAdded = 0;

	while( nValues > 1 )
	{
		FormatString += "%s\t";
		nValues--;
	}
	FormatString += "%s\r\n";

}

CObjectOrdering::~CObjectOrdering()
{
	if( Values != NULL ) delete[] Values;
	if( OrderFile != NULL ) fclose(OrderFile);
}

int CObjectOrdering::Add(int ID, ...)
{
	va_list ap;
	va_start(ap, ID);

	GCString sID;
	GCString OrdStr;

	OrdStr.FormatV(FormatString, ap);
	
	sID.Format("%i\t", ID);
	Order += sID;
	Order += OrdStr;
	
	va_end(ap);

	return ++nStringsAdded;
}

int CObjectOrdering::Add(LPCSTR ID, ...) // TODO move Translit here. if need
{
	if( nValues == 0 )
	{
		Order += ID;
		Order += "\r\n";
	}
	else
	{
		va_list ap;
		va_start(ap, ID);

		GCString sID;
		GCString OrdStr;

		OrdStr.FormatV(FormatString, ap);
		
		sID.Format("%s\t", ID);
		Order += sID;
		Order += OrdStr;
		
		va_end(ap);
	}

	return ++nStringsAdded;
}

bool CObjectOrdering::ReadString()
{
	memset(Values, 0, nValues);
	if( NULL == fgets(ReadBuffer, sizeof(ReadBuffer), OrderFile) ) return false;

	char *ptr = ReadBuffer;
	int i = 0;
	while( i < nValues )
	{
		Values[i] = ptr;
		
		ptr = strchr(ptr, '\t'); 
		if( ptr == NULL )
		{
			ptr = strchr(Values[i], '\n');
			if( ptr != NULL ) *ptr = '\0';
			break;
		}
		if( ptr == NULL ) break;

		*ptr++ = '\0';
		i++;
	}
	return true;
}

char* CObjectOrdering::GetValue(int i)
{
	if( i >= nValues ) return NULL;
	return Values[i];
}

bool CObjectOrdering::OpenFile(GCString& Dir, MMSObjectType Type)
{
	CNameTableRecord* nt_rec_ord = NameTable.Find(Type);
	OrderFile = fopen(nt_rec_ord->PrepareFileName(Dir), "rt");
	if( OrderFile == NULL ) return false;
	return true;
}

bool CObjectOrdering::WriteFile(CStorage& Storage, GCString& Dir, MMSObjectType Type)
{
	CNameTableRecord* nt_rec_ord = NameTable.Find(Type);
	return Storage.CopyStringToFile(Order, nt_rec_ord->PrepareFileName(Dir));
}
