
#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include <afx.h>
#include <afxtempl.h>
#include <ole2.h>   //IStream, IStorage
#include <io.h> //_findfirst(), _findnext()
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 256 //buffer for I/O operations


struct CGCompParameters;
class CNameTable;
class CNameTableRecord;
class CMMSFilter;

class CStorage; //Compound storages
class CContainer;
class CStreamInfo;

class CObjectOrdering;


class CMMSObject; //generic MMS object
class CFormDescrList;
class CMMS;

class CMMSBuh;
class CMMSAccountPlan;
class CMMSAccount;


//Types of Main Metadata Stream parts and name table objects
typedef enum {
	MMS_Property,
	MMS_Object, //untyped object
	MMS_MainDataContDef,
	MMS_TaskItem,
	MMS_TaskItemContent,
	MMS_TaskItemDescr,
	MMS_GenJrnlFldDef,
	MMS_DocSelRefObj,
	MMS_DocNumDef,
	MMS_Consts,
	MMS_Const,
	MMS_SbCnts,
	MMS_SbCnt,
	MMS_SbCntParams,
	MMS_Registers,
	MMS_Register,
	MMS_RegisterProps,
	MMS_RegisterFigures,
	MMS_RegisterFlds,
	MMS_Documents,
	MMS_Document,
	MMS_DocRefers, //ввод на основании
	MMS_DocHeadFields,
	MMS_DocTableFields,
	MMS_Journalisters,
	MMS_Journalister,
	MMS_JournalFlds,
	MMS_EnumList,
	MMS_Enum,
	MMS_EnumVal,
	MMS_Report,
	MMS_ReportList,
	MMS_CalcJournals,
	MMS_CalcJournal,
	MMS_CJParams,
	MMS_Calendars,
	MMS_Calendar,
	MMS_Holydays,
	MMS_CalcAlgorithms,
	MMS_CalcAlgorithm,
	MMS_RecalcRules,
	MMS_CalcVars,
	MMS_Groups,
	MMS_DocumentStreams,
	MMS_DocumentStream,
	MMS_BuhContainer,
	MMS_Buh,
	MMS_AccParams,
	MMS_Plans,
	MMS_Plan,
	MMS_Account,
	MMS_ProvParams,
	MMS_OperParams,
	MMS_SbKind,
	MMS_TypOpersDef,
	MMS_CorrProvsDef,
	MMS_ProvHardFldDef,
	MMS_OperJournal,
	MMS_DistributionRuler,
	//Далее идут служебные значения для поиска в таблице имён
	NT_ObjNameTable,
	NT_MainMetadataStream,
	NT_MetadataPart,
	NT_Workbook,
	NT_FormModule,
	NT_DialogForm,
	NT_ListForm,
	NT_MoxelSheet,
	NT_TransModuleStor,
	NT_TransactModule,
	NT_GlobalModuleStor,
	NT_GlobalModule,
	NT_UserHelpStor,
	NT_UserHelp,
	NT_ErtUserHelp,
	NT_SubcontoFolder,
	NT_SubcontoListForm,
	NT_CalcAlgorithm,
	NT_AccFormStor,
	NT_AccForm,
	NT_OperFormStor,
	NT_OperForm,
	NT_AccListForm,
	NT_OperListForm,
	NT_ProvListForm,
	NT_UnknownListForm,
	NT_GlobalData,
	NT_CommonMXL,
	NT_PictureGallery,
	NT_GUIDData,
	NT_TagStream,
	NT_UserInterface,
	NT_UserRights,
	NT_UserRightsOneFile,
	NT_ObjectOrdering,
	//Пользовательские интерфейсы
	NT_UICommands,
	NT_UIMenu,
	NT_UIPanels,
	NT_UIPanelsBMP
} MMSObjectType;


enum TTaskType {
	TaskType_Decompile = 1,
	TaskType_Compile = 2
};

//Global variables
extern CGCompParameters TaskParameters;
extern TTaskType TaskType;
extern CNameTable NameTable;
extern CMMSFilter MMS_Filter;
extern CMMS* MainMetadataStream;
extern int TotalObjectsProcessed;

#endif