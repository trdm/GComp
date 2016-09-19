
#ifndef UserInterfaces_H
#define UserInterfaces_H 1

#include "Declarations.h"
#include "Lexer.h"
#include "Container.h"
#include "Ordering.h"

class CUIUnits
{
protected:
	static GCString MenuDividorString;

	DWORD YesNo2DWORD(GCString& str);
};

class CUICommand
{
public:
	DWORD ID;
	GCString Param1;
	GCString Hint;
	BYTE HotKeyModifier; //Alt - 0x11, Ctrl - 0x09, Shift - 0x05 и их OR комбинации
	WORD HotKey; //ScanCode клавиши
	GCString Command;
	GCString CmdParameter;
	GCString FullCommand;

public:
	void FormFullCommand();

	GCString AsString(bool IsChild);
};

class CUICommands : public CUIUnits
{
public:
	DWORD ID;
	GCString StorageName;
	GCString InterfaceName;
	GCString ParentInterface;
	GCString Description;
	DWORD NoOperMenuPresent;
	DWORD IsDefault;
	DWORD NCommands;
	CArray<CUICommand, CUICommand&> Commands;
	bool IsChild;

public:
	CUICommands();

	//Возвращает количество прочитанных команд
	DWORD Read(CStorage& Storage, GCString& UIName);
	bool ReadFromFile(GCString& FileName);
	bool Write(CStorage& Storage, GCString& SrcDir);
	GCString AsString();

	bool CommandExist(DWORD CommandID);
	GCString& CommandID2Name(DWORD CommandID, GCString& Caption);
	DWORD CommandName2ID(GCString& CommandName);
};

class CUIMenu : public CUIUnits
{
	typedef enum {MainMenu, MenuGroup, MenuItem, MenuDividor} TMenuType;

public:
	TMenuType Type;
	CUIMenu* Parent;
	DWORD Level;
	DWORD NItems;
	GCString Caption;
	DWORD ID;
	CArray<CUIMenu, CUIMenu&> Items;

	CUICommands* Commands;

public:
	CUIMenu();

	CUIMenu* AddItem();

public:
	//Возвращает количество прочитанных элементов
	DWORD Read(CStorage& Storage, CUIMenu* Parent);
	bool ReadFromFile(GCString& FileName);
	bool ReadFromFile(CLexer& Lexer, CUIMenu* Parent, int level);
	bool Write(CStorage& Storage, GCString& SrcDir);
	GCString AsString();
};

class CUIPanel
{
public:
	GCString Caption;
	DWORD NButtons;
	DWORD Placement;
	DWORD IsVisible;
	DWORD OnNewLine;

public:
	CUIPanel()
	{
		NButtons = 0;
		Placement = 0xE81B;
		IsVisible = 1;
		OnNewLine = 0;
	};

public:
	//Возвращает количество прочитанных кнопок
	DWORD Read(CStorage& Storage);
	bool Write(CStorage& Storage);
	GCString AsString();
};

class CUIButton
{
public:
	DWORD ID;
	GCString Caption;

public:
	CUIButton() {ID = 0;};

	GCString AsString(bool WithCaption, CUICommands* Commands);
};

class CUIPanels : public CUIUnits
{
private:
	DWORD BMP_offset;

public:
	DWORD ID;
	DWORD NPanels;
	CArray<CUIPanel, CUIPanel&> Panels;
	DWORD NButtons;
	CArray<CUIButton, CUIButton&> AllButtons;
	DWORD NTextButtons;
	CArray<CUIButton, CUIButton&> TextButtons;
	DWORD NPictureButtons;
	CArray<CUIButton, CUIButton&> PictureButtons;

	CUICommands* Commands;

public:
	CUIPanels();

	CUIPanel* AddPanel();
	CUIButton* AddButton();
	CUIButton* AddTextButton();
	CUIButton* AddPictureButton();

public:
	bool Read(CStorage& Storage);
	bool ReadFromFile(GCString& FileName);
	bool Write(CStorage& Storage, GCString& SrcDir);
	GCString AsString();
	void ExtractBMP(GCString& Dir, CStorage& Storage);

private:
	void SetButtonCommand(CUIButton& Button, GCString& id, GCString& FileName);
};


class CUserInterface;
typedef CUserInterface* PCUserInterface;
class CUserInterfaces;

class CUserInterfaces : public CArray<PCUserInterface, PCUserInterface&>
{
public:
	~CUserInterfaces();

public:
	void AddInterface(CUserInterface* Interface);
	CUserInterface* FindInterface(GCString& Name);
	void Extract(GCString& Dir, CStorage& Storage);

	void ReadFromDir(GCString& SrcDir, CUserInterface* Parent = NULL);
};

class CUserInterface
{
public:
	GCString StorageName;
	GCString DirName;

public:
	GCString Name;
	CUserInterface* Parent;
	CUICommands Commands;
	CUIMenu Menu;
	CUIPanels Panels;

	CUserInterfaces Children;

public:
	CUserInterface();

public:
	void Read(GCString& Dir, CStorage& Storage, CContainerRecord* Container);
	bool ReadFromDir(GCString& SrcDir, CUserInterface* Parent);
	void Extract(GCString& Dir, CStorage& Storage, CObjectOrdering& Ordering);
	bool Write(CStorage& Storage);
	void MakeContainerContents(GCString& CC);
};


bool DecompileUI(GCString& DestDir, CStorage& Storage);
bool CompileUI(CStorage& Storage, GCString& SrcDir);

#endif
