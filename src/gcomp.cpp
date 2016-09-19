
#include <locale.h>
#include "task.h"
#include "CommonFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CGCompParameters TaskParameters; //нужно для Msg(...)

int _tmain(int argc, TCHAR* argv[])
{
	//Первейшим делом установим локаль. Все буржуи сволочи!
	char* locale = setlocale( LC_ALL, ".ACP");

	CTask theTask(argc, argv);
	TaskParameters = theTask.TaskParameters;
	theTask.Action();

	return 0;
}