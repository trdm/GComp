
#include "CommonFunctions.h"
#include "MMS_Filter.h"
#include "MMS.h"
#include "NameTable.h"


void CMMSFilter::Init(char* str)
{
	if( Filter != NULL ) delete Filter;

	if( str == NULL || strlen(str) == 0 )
		Filter = NULL;
	else
	{
		GCString Str;
		Str.Format("\\%s", str);
		Str.Replace('.', '\\');
		Str.Replace('/', '\\');
		Filter = SplitPath(Str);
	}
	Level = 0;
}

CMMSFilter::CMMSFilter()
{
	Filter = NULL;
	Level = 0;
}

CMMSFilter::~CMMSFilter()
{
	if( Filter != NULL ) delete Filter;
}

bool CMMSFilter::Test(CMMSObject* Object, CNameTableRecord* Record)
{
	if( Filter == NULL ) return true;
	if( Filter->GetSize() <= Level ) return true;

	CString FilterString = Filter->GetAt(Level);

	if( FilterString.IsEmpty() || FilterString == '*' ) return true;
	if( Object != NULL && FilterString.CollateNoCase(Object->sID) == 0 ) return true;
	if( Record != NULL && (
			FilterString.CollateNoCase(Record->Type1C) == 0 ||
			FilterString.CollateNoCase(Record->sID) == 0 ||
			FilterString.CollateNoCase(Record->Dir) == 0 ||
			FilterString.CollateNoCase(Record->FileName) == 0) ) return true;

	return false;
}

bool CMMSFilter::Test(GCString& Name)
{
	if( Filter == NULL ) return true;
	if( Filter->GetSize() <= Level ) return true;

	if( Name == Filter->GetAt(Level) )
		return true;
	else
		return false;
}