
#include "UI.h"
#include "Strings.h"
#include "CommonFunctions.h"
#include "NameTable.h"
#include "MMS_Filter.h"
#include "Storage.h"
#include "MMS.h"


GCString CUIUnits::MenuDividorString = ">----------<";

//===============================================================================
void CUICommand::FormFullCommand()
{
	if( CmdParameter.IsEmpty() )
		FullCommand = Command;
	else
		FullCommand = Command + ":" + CmdParameter;
	//FullCommand.Replace(" ", "");
	//FullCommand.Replace("\t", "");
}

GCString CUICommand::AsString(bool IsChild)
{
	CBeautyString res;

	if( IsChild )
	{
		res.Format("\t%s: %s\r\n", Command, CmdParameter);
	}
	else
	{
		GCString strHotKey;
		if( HotKey != 0 ) strHotKey.Format("%02x:%04x", HotKeyModifier, HotKey);
		res.Format("\t%08x:\r\n\t{\r\n", ID);
		res.add_parameter(2, "Парам1", Param1, NULL);
		res.add_parameter(2, "Hint", Hint, NULL);
		res.add_parameter(2, "HotKey", strHotKey, NULL);
		res.add_parameter(2, "Команда", Command, NULL);
		res.add_parameter(2, "Параметр", CmdParameter, NULL);
		res += "\t}\r\n";
	}
	return res;
}


CUICommands::CUICommands()
{
	ID = 0x08;
	NCommands = 0;
	IsChild = false;
	NoOperMenuPresent = 0;
	IsDefault = 0;
}



DWORD CUICommands::Read(CStorage& Storage, GCString& UIName)
{
	CNameTableRecord* nt_rec = NameTable.Find(NT_UICommands);
	Storage.OpenStream(nt_rec->StorageName);
	IStream* pStream = Storage.pStream;

	DWORD i;
	
	InterfaceName = UIName;

	pStream->Read(&ID, sizeof(ID), NULL);
	if( IsChild )
	{
		ReadSizedString(ParentInterface, pStream);
	}
	else
	{
		ReadSizedString(Description, pStream);
	}

	pStream->Read(&NoOperMenuPresent, sizeof(NoOperMenuPresent), NULL);
	pStream->Read(&IsDefault, sizeof(IsDefault), NULL);

	if( IsChild )
	{
		ReadSizedString(Description, pStream);
	}
	
	pStream->Read(&NCommands, sizeof(NCommands), NULL);

	Commands.SetSize(NCommands);
	for( i = 0; i < NCommands; i++ )
	{
		CUICommand& cmd = Commands[i];
		
		if( IsChild )
		{
			if( !ReadSizedString(cmd.Command, pStream) )
			{
				Msg(0, "WARN: В интерфейсе %s заявлено %i команд, а содержится только %i", UIName, NCommands, i);
				NCommands = i;
				break;
			}
			if( !ReadSizedString(cmd.CmdParameter, pStream) )
			{
				Msg(0, "WARN: В интерфейсе %s заявлено %i команд, а содержится только %i", UIName, NCommands, i);
				NCommands = i;
				break;
			}
		}
		else
		{
			pStream->Read(&cmd.ID, sizeof(cmd.ID), NULL);
			
			ReadSizedString(cmd.Param1, pStream);
			ReadSizedString(cmd.Hint, pStream);
			
			pStream->Read(&cmd.HotKeyModifier, sizeof(cmd.HotKeyModifier), NULL);
			pStream->Read(&cmd.HotKey, sizeof(cmd.HotKey), NULL);

			ReadSizedString(cmd.Command, pStream);
			ReadSizedString(cmd.CmdParameter, pStream);
		}
		cmd.CmdParameter.Replace("\n", "~~");

		cmd.FormFullCommand();
	}

	return NCommands;
}

GCString CUICommands::AsString()
{
	CBeautyString res;

	res.add_parameter(0, "Контейнер", StorageName, NULL);
	res.add_parameter(0, "Тип", IsChild ? "Подчинённый" : "Основной", NULL);
	res.add_parameter(0, "ИмяИнтерфейса", InterfaceName, NULL);
	res.add_parameter(0, "Родитель", ParentInterface, NULL);
	res.add_parameter(0, "Описание", Description, NULL);
	res.add_parameter(0, "ОтключитьМенюОперация", NoOperMenuPresent == 0 ? "Нет" : "Да", "Нет");
	res.add_parameter(0, "ИспользоватьПриОтключеннойАвторизации", IsDefault == 0 ? "Нет" : "Да", "Нет");

	res += "{\r\n";
	for( DWORD i = 0; i < NCommands; i++ )
	{
		res += Commands[i].AsString(IsChild);
	}
	res += "}";

	return res;
}

bool CUICommands::CommandExist(DWORD CommandID)
{
	if( CommandID == 0 ) return true;

	for( DWORD i = 0; i < NCommands; i++ )
	{
		if( Commands[i].ID == CommandID )
			return true;
	}
	return false;
}

static GCString MissingCommandID;
GCString& CUICommands::CommandID2Name(DWORD CommandID, GCString& Caption)
{
	if( CommandID == 0 )
		return MenuDividorString;

	for( DWORD i = 0; i < NCommands; i++ )
	{
		if( Commands[i].ID == CommandID )
			return Commands[i].FullCommand;
	}

	Msg(0, "WARN: Меню '%s' ссылается на несуществующий объект", Caption);
	char str[32];
	ltoa(CommandID, str, 16);
	MissingCommandID = str;
	return MissingCommandID;//(GCString&)GCString::EmptyString;
}

DWORD CUICommands::CommandName2ID(GCString& CommandName)
{
	if( CommandName == MenuDividorString )
		return 0;

	for( DWORD i = 0; i < NCommands; i++ )
	{
		if( Commands[i].FullCommand == CommandName )
			return Commands[i].ID;
	}

	char* stop;
	return strtol(CommandName, &stop, 16);
}

//=======================================================================================
CUIMenu::CUIMenu()
{
	Type = MainMenu;
	Parent = NULL;
	Commands = NULL;
	Level = 0;
	NItems = 0;
	ID = 0;
}


DWORD CUIMenu::Read(CStorage& Storage, CUIMenu* Parent)
{
	this->Parent = Parent;
	if( Parent != NULL )
	{
		Level = Parent->Level + 1;
	}
	else
	{
		CNameTableRecord* nt_rec = NameTable.Find(NT_UIMenu);
		if( !Storage.OpenStream(nt_rec->StorageName) ) return 0;
	}

	IStream* pStream = Storage.pStream;

	pStream->Read(&ID, sizeof(ID), NULL);

	switch( ID )
	{
		case 0x01      : Type = MainMenu; break;
		case 0xFFFFFFFF: Type = MenuGroup; break;
		case 0x00      : Type = MenuDividor; break;
		default        : Type = MenuItem;
	}

	if( Type == MenuGroup || Type == MenuItem )
	{
		ReadSizedString(Caption, pStream);
	}
	else if( Type == MenuDividor )
	{
		Caption = (LPCSTR)MenuDividorString;
	}

	if( Type == MainMenu || Type == MenuGroup )
	{
		pStream->Read(&NItems, sizeof(NItems), NULL);
		if( NItems > 0 )
		{
			Items.SetSize(NItems);
			for( DWORD i = 0; i < NItems; i++ )
			{
				Items[i].Commands = Commands;
				Items[i].Read(Storage, this);
			}
		}
		return NItems;
	}

	return 1;
}

GCString CUIMenu::AsString()
{
	GCString prefix, res;
	
	if( Type == MenuItem && TaskParameters.NoBrokenLinks )
	{
		if( !Commands->CommandExist(ID) )
			return res;
	}

	prefix.Add("\t", Level);

	res += prefix;
	switch( Type )
	{
		case MainMenu   : res += "ГлавноеМеню:";	break;
		case MenuGroup  : res += "Субменю: ";		break;
		case MenuItem   : res += "Элемент: ";		break;
		case MenuDividor: res += "Разделитель: ";	break;
	}
	res += Caption;
	if( Type == MenuItem )
	{
		res += "\r\n\t" + prefix + Commands->CommandID2Name(ID, Caption);
	}
	res += "\r\n";

	if( Type == MainMenu || Type == MenuGroup )
	{
		res += prefix;
		res += "{\r\n";
		for( DWORD i = 0; i < NItems; i++ )
		{
			res += Items[i].AsString();
		}
		res += prefix;
		res += "}\r\n";
	}

	return res;
}

DWORD CUIPanel::Read(CStorage& Storage)
{
	IStream* pStream = Storage.pStream;
	ReadSizedString(Caption, pStream);
	pStream->Read(&NButtons, sizeof(NButtons), NULL);
	pStream->Read(&Placement, sizeof(Placement), NULL);
	pStream->Read(&IsVisible, sizeof(IsVisible), NULL);
	pStream->Read(&OnNewLine, sizeof(OnNewLine), NULL);
	return NButtons;
}

GCString CUIPanel::AsString()
{
	GCString res;
	GCString sPlacement, sVisible, sOnNewLine;

	switch( Placement )
	{
		case 0xE81B: sPlacement = "Сверху"; break;
		case 0xE81C: sPlacement = "Слева"; break;
		case 0xE81D: sPlacement = "Справа"; break;
		case 0xE81E: sPlacement = "Снизу"; break;
		default: 
			sPlacement = "Ошибка чтения расположения панели!!!";
			Msg(0, "ERR: Ошибка чтения расположения панели %s", Caption);
	}
	
	if( IsVisible == 0 )
		sVisible = "Нет";
	else
		sVisible = "Да";

	if( OnNewLine == 0 )
		sOnNewLine = "Нет";
	else
		sOnNewLine = "Да";

	res.Format("Панель: %s\r\n{\r\n\tРасположение: %s\r\n\tВидимость: %s\r\n\tСНовойСтроки: %s\r\n\tКнопок: %i\r\n}\r\n", 
		Caption, sPlacement, sVisible, sOnNewLine, (int)NButtons);

	return res;
}

CUIPanels::CUIPanels()
{
	ID = 0xFFFFFFFD;
	NPanels = 0;
	NButtons = 0;
	NTextButtons = 0;
	NPictureButtons = 0;
}

bool CUIPanels::Read(CStorage& Storage)
{
	CNameTableRecord* nt_rec = NameTable.Find(NT_UIPanels);
	GCString StreamName = nt_rec->PrepareStorageName();
	long sz = Storage.GetStreamSize(StreamName);
	if( !Storage.OpenStream(StreamName) ) return false;
	Storage.StreamInfo.FullSize = sz;

	IStream* pStream = Storage.pStream;
	DWORD i;

	pStream->Read(&ID, sizeof(ID), NULL);
	pStream->Read(&NPanels, sizeof(NPanels), NULL);
	
	Panels.SetSize(NPanels);
	for( i = 0; i < NPanels; i++ )
	{
		NButtons += Panels[i].Read(Storage);
	}

	AllButtons.SetSize(NButtons);
	for( i = 0; i < NButtons; i++ )
	{
		pStream->Read(&AllButtons[i].ID, sizeof(AllButtons[i].ID), NULL);
	}

	pStream->Read(&NTextButtons, sizeof(NTextButtons), NULL);
	TextButtons.SetSize(NTextButtons);
	for( i = 0; i < NTextButtons; i++ )
	{
		ReadSizedString(TextButtons[i].Caption, pStream);
		pStream->Read(&TextButtons[i].ID, sizeof(TextButtons[i].ID), NULL);
	}

	pStream->Read(&NPictureButtons, sizeof(NPictureButtons), NULL);
	PictureButtons.SetSize(NPictureButtons);
	for( i = 0; i < NPictureButtons; i++ )
	{
		pStream->Read(&PictureButtons[i].ID, sizeof(PictureButtons[i].ID), NULL);
	}

	LARGE_INTEGER SeekZero = {0, 0};
	ULARGE_INTEGER pos;
	pStream->Seek(SeekZero, STREAM_SEEK_CUR, &pos);
	BMP_offset = pos.LowPart;

	return true;
}

void CUIPanels::ExtractBMP(GCString& Dir, CStorage& Storage)
{
	CNameTableRecord* nt_rec = NameTable.Find(NT_UIPanelsBMP);
	LARGE_INTEGER offset = {BMP_offset, 0};
	GCString StreamName = nt_rec->StorageName;
	if( !Storage.OpenStream(StreamName) ) return;
	Storage.pStream->Seek(offset, STREAM_SEEK_SET, NULL);
	Storage.CopyToFile(nt_rec->PrepareFileName(Dir), true);
}

GCString CUIButton::AsString(bool WithCaption, CUICommands* Commands)
{
	GCString res;
	if( WithCaption )
	{
		res.Format("\t%s\r\n\t\t%s\r\n", Commands->CommandID2Name(ID, Caption), Caption);
	}
	else
	{
		res.Format("\t%s\r\n", Commands->CommandID2Name(ID, Caption));
	}

	return res;
}

GCString CUIPanels::AsString()
{
	GCString res;
	DWORD i;

	for( i = 0; i < NPanels; i++ )
	{
		res += Panels[i].AsString();
	}

	res += "Кнопки:\r\n{\r\n";
	for( i = 0; i < NButtons; i++ )
	{
		res += AllButtons[i].AsString(false, Commands);
	}
	res += "}\r\n";

	res += "ТекстовыеКнопки:\r\n{\r\n";
	for( i = 0; i < NTextButtons; i++ )
	{
		res += TextButtons[i].AsString(true, Commands);
	}
	res += "}\r\n";

	res += "КнопкиСПиктограммами:\r\n{\r\n";
	for( i = 0; i < NPictureButtons; i++ )
	{
		res += PictureButtons[i].AsString(false, Commands);
	}
	res += "}\r\n";

	return res;
}

CUserInterfaces::~CUserInterfaces()
{
	int i = GetSize();
	while( --i >= 0 )
	{
		delete GetAt(i);
	}
}

CUserInterface* CUserInterfaces::FindInterface(GCString& Name)
{
	int i = GetSize() - 1;
	while( i >= 0 )
	{
		if( GetAt(i)->Name == Name ) return GetAt(i);
		CUserInterface* Interface = GetAt(i)->Children.FindInterface(Name);
		if( Interface != NULL ) return Interface;
		i--;
	}
	return NULL;
}

void CUserInterfaces::AddInterface(CUserInterface* Interface)
{
	if( Interface->Commands.IsChild )
	{
		CUserInterface* Parent = FindInterface(Interface->Commands.ParentInterface);
		Parent->Children.Add(Interface);
		Interface->Parent = Parent;
	}
	else
	{
		Add(Interface);
	}
}

void CUserInterfaces::Extract(GCString& Dir, CStorage& Storage)
{
	CObjectOrdering Ordering(2);
	for( int i = 0; i < GetSize(); i++ )
	{
		GetAt(i)->Extract(Dir, Storage, Ordering);
	}
	if( !Ordering.IsEmpty() )
		Ordering.WriteFile(Storage, Dir);
}

//=======================================================================================
CUserInterface::CUserInterface()
{
	Parent = NULL;
	Menu.Commands = &Commands;
	Panels.Commands = &Commands;
}

void CUserInterface::Read(GCString& Dir, CStorage& Storage, CContainerRecord* Container)
{
	StorageName = Container->StreamName;
	Name = Container->ObjectName;
	Msg(2, "Интерфейс %s", Name);

	Storage.Open(StorageName);
	
	if( Container->Type == "UsersInterfaceType" )
	{
		Commands.IsChild = false;
	}
	else if( Container->Type == "SubUsersInterfaceType" )
	{
		Commands.IsChild = true;
	}

	Commands.StorageName = StorageName;
	Commands.Read(Storage, Name);
	if( !Commands.IsChild )
	{
		Menu.Read(Storage, NULL);
		Panels.Read(Storage);
	}

	Storage.Close();
}

void CUserInterface::Extract(GCString& Dir, CStorage& Storage, CObjectOrdering& Ordering)
{
	MMS_Filter.Down();
	if( MMS_Filter.Test(Name) )
	{
		GCString MangledName = Translit(Name);
		MangleFileName(MangledName);
		GCString UIDir = Dir + "\\" + MangledName; 
		// TODO конструкция тут такая же, как и в SbCnts <-> SbCnt. 
		// Только там Decompile(), а не Extract(), и sID, а не Name.
		CreateDirectory(UIDir);

		CNameTableRecord* nt_rec = NameTable.Find(NT_UICommands);
		Storage.CopyStringToFile(Commands.AsString(), nt_rec->PrepareFileName(UIDir));
		if( !Commands.IsChild )
		{
			nt_rec = NameTable.Find(NT_UIMenu);
			Storage.CopyStringToFile(Menu.AsString(), nt_rec->PrepareFileName(UIDir));
			nt_rec = NameTable.Find(NT_UIPanels);
			Storage.CopyStringToFile(Panels.AsString(), nt_rec->PrepareFileName(UIDir));

			Storage.Open(StorageName);
			Panels.ExtractBMP(UIDir, Storage);
			Storage.Close();
		}

		Children.Extract(UIDir, Storage);
		Ordering.Add(MangledName, Name, StorageName);
	}
	MMS_Filter.Up();
}

bool DecompileUI(GCString& Dir, CStorage& Storage)
{
	CNameTableRecord* nt_rec = NameTable.Find(NT_UserInterface);

	MMS_Filter.Down();
	bool ToExtract = MMS_Filter.Test(NULL, nt_rec);
	if( ToExtract )
	{
		GCString DestDir = nt_rec->PrepareDirName(Dir);
		CStorage UIStorage(Storage);
		CContainer CC;
		CUserInterfaces UserInterfaces;

		if( UIStorage.Open(nt_rec->PrepareStorageName()) )
		{
			UIStorage.ParseStream(&CC, GCString("Container.Contents"), false);
			CreateDirectory(DestDir);

			for( int i = 0; i < CC.GetNChildren(); i++ )
			{
				CContainerRecord* UIDescr = (CContainerRecord*)CC.GetChild(i);
				CUserInterface* Interface = new CUserInterface;
				Interface->Read(DestDir, UIStorage, UIDescr);
				UserInterfaces.AddInterface(Interface);
			}

			UserInterfaces.Extract(DestDir, UIStorage);
		}
	}
	MMS_Filter.Up();
	
	return ToExtract;
}

