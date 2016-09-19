
#ifndef TASK_H
#define TASK_H

#include <stdio.h>
#include "DLL\GComp.h"
#include "Strings.h"
#include <afx.h>

class CTaskParameters : public CGCompParameters
{
public:
	enum TaskTypes {
		compile,
		decompile,
		help,
		show_version,
		name_translation
	};

	TaskTypes TaskType;
	GCString csCompoundFileName;
	GCString csDirectoryName;
	GCString ObjectName;
	bool CreateIniFile;

public:
	inline CTaskParameters()
	{
		TaskType = help;
		CreateIniFile = true;
	};

	void ParseCommandLine(int argc, TCHAR* argv[]);
};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class CTask  
{
public:
	CTaskParameters  TaskParameters;

public:
	CTask(int argc, TCHAR* argv[]);
	CTask();

private:
	void HelpRu();
	void HelpEn();
	void Help();
	void Decompile();
	void Compile();

public:
	void Action();
};


extern CGCompParameters TaskParameters;

#endif
