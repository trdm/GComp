//////////////////////////////////////////////////////////////////////
// TaskDecompile.cpp: implementation of the CTaskDecompile class.
//////////////////////////////////////////////////////////////////////

#include "Task.h"
#include "CommonFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


void CTaskDecompile::Action()
{
	CTime Time = CTime::GetCurrentTime();
	if( TaskParameters.Verbose > 1) Msg(0, "\nExtraction started: %s", Time.Format("%Y.%m.%d %H:%M:%S"));

	GComp_Decompile(&TaskParameters);

	if( TaskParameters.CompoundType != CTaskParameters::external_report || TaskParameters.Verbose > 1)
	{
		CTimeSpan ExtractionTime = CTime::GetCurrentTime() - Time;
		Msg(0, "'%s' extracted into '%s' in %s", 
			TaskParameters.CompoundFileName, TaskParameters.DirectoryName, 
			ExtractionTime.Format("%H:%M:%S"));
	}
}
