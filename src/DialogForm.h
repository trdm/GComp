
#ifndef DialogForm_H
#define DialogForm_H

#include "Declarations.h"
#include "Storage.h"
#include "Strings.h"
#include "Lexer.h"
#include "MMS_BasicTypes.h"
#include "MMS.h"

class CFontSize : public CStoredProperty
{
private:
	double Value; //Размер шрифта в пунктах

public:
	double operator=(double NewVal) { Value = NewVal; return Value; };
	virtual void SetValue(GCString& Value);

	virtual GCString BeautyString();
	virtual GCString MMSString();
};

class CFormFont : public CMMSObject
{
public:
	CFontSize FontSize; //-1 - 1pt; -2 - 1.5pt; -3 - 2pt; -4 - 2.5pt ... etc
	//дальше параметры по видимому соответствуют стандартным полям структуры LOGFONT.  1С в основном их игнорирует.
	int Param2; //LONG lfWidth;
	int Param3; //LONG lfEscapement;
	int Param4; //LONG lfOrientation
	CFontBold	FontBold; //in source: 400 - normal; 700 - bold   //LONG lfWeight
	CStoredBool	FontItalic; //1 - italic; 0 - normal //BYTE lfItalic
	CStoredBool	FontUnderline; //1 - undeline; 0 - normal  //BYTE lfUnderline
	int Param8; //BYTE lfStrikeOut
	int Param9; //charset (0 - ANSI, 204 - Russian)
	int Param10; //BYTE lfOutPrecision
	int Param11; //BYTE lfClipPrecision
	int Param12; //BYTE lfQuality
	int Param13; //BYTE lfPitchAndFamily
	GCString FontName;

	bool FontNeeded; //для тех контролов, где шрифт не нужен

private:
	static CFormFont* DefVal;
	static CPropertySet PropSet;
	static CObjectProperty Props[];

public:
	CFormFont();
	CFormFont& operator=(CFormFont& src);

	virtual const char* BeautyName() {return "Шрифт";}
	CPropertySet* GetPropSet() {return &PropSet;};

	virtual void BeautyString(CBeautyString& String, int level);
	virtual void ToString(CMMSString& String);
};


class CHotKey : public CStoredObject
{
	typedef struct {
		int code;
		char* name;
	} VK_Pair;
public:
	int Modifier;
	int KeyCode;

private:
	static VK_Pair virtual_keys[];

public:
	CHotKey() { Modifier = 0; KeyCode = 0; };

	void FromString(GCString& String, CLexer& Lexer);
	virtual bool SetValue(GCString& Name, GCString& Value);
	virtual bool SetValue(int MMS_Index, GCString& Value);

	void BeautyString(CBeautyString& String, int level);
	void ToString(CMMSString& String);
};


class CFormControlBase : public CMMSObject
{
public:
	GCString Caption;
	GCString Class; //1CGROUPBOX, STATIC, 1CEDIT, BMASKED, BUTTON, CHECKBOX, RADIO, LISTBOX, CMBOBOX, TABLE, PICTURE, SysTreeView32
	GCString Identifier;
	CMMSObjRef ObjectID; //ID атрибута объекта-владельца. Для системных колонок - отрицательный
	CMMSAttributeType Type;
	CStoredBitSet ValueFlags;  //1 - неотрицательный, 2 - разделять триады. Комбинируются по OR.

protected:
	static char* FlagList[]; //массив из 32_х элементов, к которому можно обращаться по номеру бита
	static char* BasicFlagList[]; //массив из 32_х элементов, к которому можно обращаться по номеру бита
	static char* ValueFlagList[]; //массив из двух элементов.

public:
	void CorrectObjectID(CMMSObject* ParentObject);
	virtual void BeautyString(CBeautyString& String, int level);

protected:
	const char* ToRealType(const char* type);
	GCString FromRealType(const char* type, bool AsNumber);
	//void ObjectNameToID();
};

class CFormControl : public CFormControlBase
{
friend class CDialogForm;

public:
	CStoredBitSet Flags;
	int PosX;
	int PosY;
	int Width;
	int Height;
	bool IsObjectAttribute; //control привязан к атрибуту объекта - владельца формы. 1 - привязан, 0 - нет
	int Param9;
	int TabOrder;
	GCString Param11;
	GCString Formula;
	CStoredBitSet BasicFlags;
	GCString Mask;    //Для Строки - Маска, Для Справочника - "Связан С".
	GCString LongHint;
	GCString ShortHint;
	int Param24;
	CFormFont Font; //параметры с 25 по 38
	int FontColor; //-1 - default. В hex-формате: 0xBBGGRR
	int Param40;
	unsigned int PictureID;
	GCString Layer;
	CHotKey HotKey;

private:
	static CFormControl* DefVal;
	static CPropertySet PropSet;
	static CObjectProperty Props[];
	static void SetDefaulFont(CFormFont& Font) {DefVal->Font = Font;}

public:
	CFormControl();
	CFormControl(const char* aClass, const char* aIdentifier);

	CPropertySet* GetPropSet() {return &PropSet;};
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);

	virtual void BeautyString(CBeautyString& String, int level);
	virtual void ToString(CMMSString& String);

	static int __cdecl CompareByIdentifier(const void *elem1, const void *elem2);
	static int __cdecl CompareByTabOrder(const void *elem1, const void *elem2);

private:
	void Setup();
	bool NeedFont();
	bool NeedType();
};

class CFormColumn : public CFormControlBase
{
public:
	int ColumnType;
	int Width;
	int TabOrder;
	GCString Param6;
	GCString Formula;
	GCString Param15;
	GCString Param16;
	int TextAlignment;
	CStoredBitSet BasicFlags;
	GCString Mask;    //Для Строки - Маска, Для Справочника - "Связан С".
	GCString LongHint;
	GCString ShortHint;
	GCString Param22;

private:
	static CFormColumn* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CFormColumn();
	CFormColumn(const char* aClass, const char* aIdentifier);

	CPropertySet* GetPropSet() {return &PropSet;};

private:
	void Setup();
};

class CFormControls : public CMMSAttributes
{
public:
	CMMSObject* Parent;

public:
	CFormControls();

	virtual const char* BeautyName() {return "Элементы";}
	virtual const char* MMSName() {return "Controls";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CFormControl;};
	virtual CMMSObject* CreateChild(const char* Name, const char* Value)
		{return new CFormControl(Name, Value);}

	virtual void BeautyString(CBeautyString& String, int level);
	virtual void ToString(CMMSString& String);
};

class CFormColumns: public CMMSAttributes
{
public:
	CMMSObject* Parent;

public:
	CFormColumns();

	virtual const char* BeautyName() {return "Колонки";}
	virtual const char* MMSName() {return "Fixed";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CFormColumn;};
	virtual CMMSObject* CreateChild(const char* Name, const char* Value)
		{return new CFormColumn(Name, Value);}

	virtual void BeautyString(CBeautyString& String, int level);
};

class CFormMulticolumn : public CMMSAttributes
{
public:
	virtual const char* BeautyName() {return "Multicolumn";}
	virtual const char* MMSName() {return "Multicolumn";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CFormControl;};
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);

	virtual void BeautyString(CBeautyString& String, int level);
};

class CFormBrowser : public CMMSAttributes
{
public:
	int Param1;
	int Param2;
	CFormMulticolumn Multicolumn;
	CFormColumns Fixed;

private:
	static CFormBrowser* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CFormBrowser();
	CPropertySet* GetPropSet() {return &PropSet;};
	virtual bool SetValue(int MMS_Index, GCString& Value);
	virtual const char* BeautyName() {return "Табличная часть";}
	virtual const char* MMSName() {return "Browser";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);
	virtual CMMSObject* AddChild(CMMSObject* child) {return NULL;}
};

class CFormLayer : public CMMSObject
{
public:
	CStoredBool IsVisible;

private:
	static CFormLayer* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CFormLayer() { IsVisible = false; };
	CPropertySet* GetPropSet() {return &PropSet;};
};

class CFormLayers : public CMMSAttributes
{
public:
	int ActiveLayer; //zero based

public:
	CFormLayers() { ActiveLayer = 0; };

	virtual const char* BeautyName() {return "Слои";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CFormLayer;};
	virtual void ParseFile(CLexer& Lexer);

	bool SetValue(int MMS_Index, GCString& Value);
	virtual void BeautyString(CBeautyString& String, int level);
	virtual void ToString(CMMSString& String);
};

class CDialogFrameContent : public CMMSObject
{
public:
	CFormFont Font;
	int Width;
	int Height;
	GCString Caption;
	GCString Param18;
	GCString Param19;
	CStoredBitSet Flags1; //Флаги формы
	GCString Param21;
	bool FontDefault; //1 - default; 0 - user defined
	bool AutoTabOrder;
	GCString Param24;
	GCString Param25;
	int BackgroundColor; //-1 - default. В hex-формате: 0xBBGGRR
	unsigned int PictureID;
	CStoredBitSet Flags2; //дублирует Flags1
	CFormLayers Layers;
	bool ShowPanel; //1 - показывать; 0 - не показывать
	bool AllowResize; //1 - размер можно менять; 0 - нельзя

private:
	static CDialogFrameContent* DefVal;
	static char* FlagList[]; //массив из 32_х элементов, к которому можно обращаться по номеру бита
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CDialogFrameContent();
	CPropertySet* GetPropSet() {return &PropSet;};
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);
	virtual CMMSObject* AddChild(CMMSObject* Child) {return NULL;}

	virtual const char* BeautyName() {return "Параметры";}
	virtual void BeautyString(CBeautyString& String, int level);
};

class CDialogFrame : public CMMSAttributes
{
public:
	CDialogFrameContent Content;

public:
	CDialogFrame();

	virtual const char* BeautyName() {return "Параметры формы";}
	virtual const char* MMSName() {return "Frame";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
};

class CFrameBraces : public CMMSAttributes
{
public:
	virtual const char* BeautyName() {return "Скобки";}
	virtual void BeautyString(CBeautyString& String, int level);
	virtual void ToString(CMMSString& String);
};

class CDialogCntVer : public CMMSNamedList
{
public:
	CDialogCntVer() {sID = "Cnt_Ver";}
	virtual CStoredProperty* CreateProperty() {return new CStoredString;}
	virtual const char* BeautyName() {return "Cnt_Ver";}
	virtual const char* MMSName() {return "Cnt_Ver";}
};

class CDialogForm : public CMMSAttributes
{
public:
	CFrameBraces Braces; //Конструкция {{{}},{}} - если есть
	CDialogFrame Frame;
	CFormBrowser Browser;
	CFormColumns Fixed;
	CFormControls Controls;

	CDialogCntVer Cnt_Ver; //вроде как везде 10001

public:
	CDialogForm(CMMSObject* aParent);

	virtual const char* BeautyName() {return "Диалог";}
	virtual const char* MMSName() {return "Dialogs";}

	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);
	virtual void ParseMetadata(CLexer& Lexer);

	virtual void ToString(CMMSString& String);

	void WriteToFile(CStorage& Storage, GCString FileName);
};

#endif
