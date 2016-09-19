
#ifndef MMS_Filter_H
#define MMS_Filter_H

#include "Declarations.h"


class CMMSFilter
{
private:
	CStringArray* Filter;
	int Level;

public:
	void Init(char* Str);
	CMMSFilter();
	~CMMSFilter();

public:
	bool IsEmpty() {return Filter == NULL || Filter->GetSize() == 0;};
	int Down() {return ++Level;};
	int Up()   {return --Level;};
	bool Test(CMMSObject* Object, CNameTableRecord* Record);
	bool Test(GCString& Name);
};

#endif