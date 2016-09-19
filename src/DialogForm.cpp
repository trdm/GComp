
#include "Declarations.h"
#include "DLL\GComp.h"
#include "CommonFunctions.h"
#include "DialogForm.h"
#include "MMS.h"
#include "NameTable.h"
#include "Storage.h"
#include "Strings.h"


/* нелепая таблица размеров шрифта в 1С
0    =   0
1    =  -1
1.5  =  -2
2.5  =  -3
3    =  -4
4    =  -5
4.5  =  -6
5.5  =  -7
6    =  -8
7    =  -9
7.5  = -10
8.5  = -11
9    = -12
10   = -13
10.5 = -14
11.5 = -15
12   = -16

  Таблица подобрана опытным путем, мы надеемся, что выявленная закономерность
  будет соблюдаться и при больших размерах шрифтов, но гарантии не даем.   */
void CFontSize::SetValue(GCString& NewVal)
{
	if( NewVal.Find('-') >= 0 )
	{
		int size = atoi(NewVal);
		int halves = -size / 2;
		int ones = halves + (-size % 2);
		
		Value = ones + (0.5 * halves);
	}
	else
	{
		Value = atof(NewVal);
	}
}

GCString CFontSize::BeautyString()
{
	return GCString(Value);
}

GCString CFontSize::MMSString()
{
	return GCString(-(int)((Value * 4.0) / 3.0)); // Потеря данных мне и нужна
}

/////////////////////////////////////////////////////////////////
//////  CFormFont  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
CFormFont::CFormFont()
{
	sID = "Font";

	FontSize = 8.5;
	Param2 = 0;
	Param3 = 0;
	Param4 = 0;
	FontBold = false;
	FontItalic = false;
	FontUnderline = false;
	Param8 = 0;
	Param9 = 0;
	Param10 = 0;
	Param11 = 0;
	Param12 = 0;
	Param13 = 0;
	FontName = "MS Sans Serif";

	FontNeeded = true;
}

CFormFont& CFormFont::operator =(CFormFont& src)
{
	FontSize = src.FontSize;
	Param2 = src.Param2;
	Param3 = src.Param3;
	Param4 = src.Param4;
	FontBold = src.FontBold;
	FontItalic = src.FontItalic;
	FontUnderline = src.FontUnderline;
	Param8 = src.Param8;
	Param9 = src.Param9;
	Param10 = src.Param10;
	Param11 = src.Param11;
	Param12 = src.Param12;
	Param13 = src.Param13;
	FontName = src.FontName;
	return *this;
}

void CFormFont::BeautyString(CBeautyString& String, int level)
{
	if( !FontNeeded )
		return;

	String.add_parameter(level, BeautyName(), FontName, "", true);
	String.add_brace(level, '{');
	
	level++;

	for( int i = 0; i < PropSet.nProps; i++ )
	{
		PropSet.BeautyString(String, level, this, i);
	}

	String.add_brace(level-1, '}');
}

void CFormFont::ToString(CMMSString& String)
{
	CStoredObject::ToString(String);
}

/////////////////////////////////////////////////////////////////
//////  CFormLayers  ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
bool CFormLayers::SetValue(int MMS_Index, GCString& Value)
{
	if( MMS_Index == 0 )
	{
		ActiveLayer = atoi(Value);
		return true;
	}
	else
	{
		return CMMSAttributes::SetValue(MMS_Index, Value);
	}
}

void CFormLayers::BeautyString(CBeautyString& String, int level)
{
	String.add_offset(level);
	String += "Слои\r\n";
	String.add_brace(level, '{');

	for( int i = 0; i < GetNChildren(); i++ )
	{
		CFormLayer& Layer = *(CFormLayer*)GetChild(i);
		char* descr = "";
		if( Layer.IsVisible && i == ActiveLayer )
			descr = "Видимый, Активный";
		else if( Layer.IsVisible )
			descr = "Видимый";
		String.add_parameter(level+1, Layer.sID, descr, "", true);
	}

	String.add_brace(level, '}');
}

void CFormLayers::ToString(CMMSString& String)
{
	String += "\r\n{";
	String.add_quoted(ActiveLayer, false);
	for( int i = 0; i < GetNChildren(); i++ )
	{
		String += ",";
		GetChild(i)->ToString(String);
	}
	String += "}";
}



/////////////////////////////////////////////////////////////////
//////  CFormControlBase  ///////////////////////////////////////
/////////////////////////////////////////////////////////////////
void CFormControlBase::BeautyString(CBeautyString& String, int level)
{
	String.add_parameter(level, Class, Identifier, NULL, true);
	String.add_brace(level, '{');


	CPropertySet* pPropSet = GetPropSet();
	if( pPropSet != NULL )
	{
		for( int i = 2; i < pPropSet->nProps; i++ ) //пропускаем Class и Identifier
			pPropSet->BeautyString(String, level+1, this, i);
	}
	
	String.add_brace(level, '}');
}


void CFormControlBase::CorrectObjectID(CMMSObject* ParentObject)
{
	if( ParentObject == NULL ) return;
	if( ObjectID.iTypeObjectID <= 0 ) return;

	//Попробуем найти поле по имени, в объекте-владельце формы
	CMMSObject* field = ParentObject->FindChild(Identifier, true);
	if( field != NULL )
	{
		if( ObjectID.iTypeObjectID != field->ID )
		{
			ObjectID.iTypeObjectID = field->ID;
			ObjectID.sTypeObjectID = ObjectID.iTypeObjectID;
		}
		Type = ((CMMSObjectAttribute*)field)->Type; //тип тоже возьмём из параметров поля
	}
}

/////////////////////////////////////////////////////////////////
//////  CFormControls  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////
bool CHotKey::SetValue(int MMS_Index, GCString& Value)
{
	char* ptr = (char*)(LPCSTR)(Value) + 3;

	Modifier = strtol(ptr, &ptr, 10);
	KeyCode  = strtol(ptr+5, &ptr, 10);
	return true;
}

bool CHotKey::SetValue(GCString& Name, GCString& Value)
{
	if( Value.Left(3) == "{\"\"" && Value.Right(3) == "\"\"}" )
		return SetValue(-1, Value);

	GCString mod, key;

	mod = Value.SpanExcluding("+-");
	if( mod.GetLength() == Value.GetLength() )
	{
		key = mod;
		mod = "";
	}
	else
		key = Value.Mid(mod.GetLength() + 1);

	Modifier = atoi(mod);
	KeyCode = atoi(key);

	mod.MakeUpper();
	if( Modifier == 0 && !mod.IsEmpty() )
	{
		if( strstr(mod, "SHIFT") != NULL ) Modifier |= 0x07;
		if( strstr(mod, "CTRL")  != NULL ) Modifier |= 0x0b;
		if( strstr(mod, "ALT")   != NULL ) Modifier |= 0x13;
	}
	
	if( KeyCode == 0 && !key.IsEmpty() )
	{
		if( key.GetLength() == 1 )
		{
			key.MakeUpper();
			KeyCode = (unsigned char)(key[0]);
		}
		else
		{
			for( int i = 0; virtual_keys[i].code != 0; i++ )
			{
				if( stricmp(virtual_keys[i].name, key) != 0 ) continue;
				KeyCode = virtual_keys[i].code;
				i = -1;
				break;
			}
			if( i > 0 )
			{
				Msg(0, "ERR: Unknown key '%s'", key);
				throw NULL;
				//return false;
			}
		}
	}

	return true;
}

void CHotKey::BeautyString(CBeautyString& String, int level)
{
	if( Modifier == 0 && KeyCode == 0 )
		return;

	GCString str;
	/*
	  111 - Shift
	 1011 - Ctrl
	10011 - Alt
	*/
	if( Modifier == 0x03 ) str += "3";
	if( Modifier & 0x04 ) str += "Shift";
	if( Modifier & 0x08 ) str += "Ctrl";
	if( Modifier & 0x10 ) str += "Alt";
	if( !str.IsEmpty() ) str += "+";

	if( ('0' <= KeyCode && KeyCode <= '9') || ('A' <= KeyCode && KeyCode <= 'Z') )
		str += (char)KeyCode;
	else
	{
		for( int i = 0; virtual_keys[i].code != 0; i++ )
		{
			if( KeyCode != virtual_keys[i].code ) continue;
			str += virtual_keys[i].name;
			i  = -1;
			break;
		}

		if( i > 0 )
		{
			GCString strCode;
			strCode.Format("%i", KeyCode);
			str += strCode;
		}
	}

	String.add_parameter(level, "HotKey", str, "");
}

void CHotKey::ToString(CMMSString& String)
{
	GCString str;
	str.Format("{\"\"%i\"\",\"\"%i\"\"}", Modifier, KeyCode);
	String.add_quoted(str, false);
}

//==========================================================================
//========  CFormControl  ==================================================
//==========================================================================
void CFormControl::Setup()
{
	sID = "Control";

	Flags = 0;
	PosX = PosY = 0;
	Width = Height = 0;
	IsObjectAttribute = false; //control привязан к атрибуту объекта - владельца формы. 1 - привязан, 0 - нет
	Param9 = 0;
	TabOrder = 0;
	ValueFlags = 0;  //1 - неотрицательный, 2 - разделять триады. Комбинируются по OR.
	BasicFlags = 0;
	Param24 = 0;
	FontColor = -1; //-1 - default. В hex-формате: 0xBBGGRR
	Param40 = -1;
	PictureID = 0;

	Flags.SetTable(FlagList);
	BasicFlags.SetTable(BasicFlagList);
	ValueFlags.SetTable(ValueFlagList);

	ToDestroyChildren = false;
}

CFormControl::CFormControl()
{
	Setup();
}

CFormControl::CFormControl(const char* aClass, const char* aIdentifier)
{
	Setup();
	Class = aClass;
	Identifier = aIdentifier;
}

CMMSObject* CFormControl::CreateChild(const char* Name, const char* Value)
{
	if( _stricoll(Name, "Шрифт") == 0 )
	{
		Font.FontName = Value;
		return &Font;
	}
	return NULL;
}

void CFormControl::BeautyString(CBeautyString& String, int level)
{
	Font.FontNeeded = NeedFont();
	Type.NeedType = NeedType();

	CFormControlBase::BeautyString(String, level);
}

void CFormControl::ToString(CMMSString& String)
{
	//Font.FontNeeded = NeedFont();
	CFormControlBase::ToString(String);
}

bool CFormControl::NeedFont()
{
	if( TaskParameters.NoDefaults ) return true;

	if( BasicFlags.BitValue(6) ) //default font
	{
		return false;
		//Font = DefVal->Font;
	}

	if( stricmp(Class, "STATIC") == 0 ) return true;
	return false;
}
bool CFormControl::NeedType()
{
	if( TaskParameters.NoDefaults ) return true;

	if( strcmp(Class, "STATIC") == 0 ) return false;
	if( strcmp(Class, "BUTTON") == 0 ) return false;
	if( strcmp(Class, "COMBOBOX") == 0 ) return false;
	if( strcmp(Class, "LISTBOX") == 0 ) return false;
	if( strcmp(Class, "TABLE") == 0 ) return false;
	if( strcmp(Class, "PICTURE") == 0 ) return false;
	if( strcmp(Class, "1CGROUPBOX") == 0 ) return false;
	return true;
}

int __cdecl CFormControl::CompareByIdentifier(const void *elem1, const void *elem2)
{
	CFormControl *c1 = *(CFormControl**)elem1, *c2 = *(CFormControl**)elem2;
	
	int Comp = c1->Class.Compare(c2->Class);
	if( Comp != 0 )
		return Comp;

	if( !(c1->Identifier.IsEmpty() && c2->Identifier.IsEmpty()) )
	{
		if( c1->Identifier.IsEmpty() ) return 1;
		if( c2->Identifier.IsEmpty() ) return -1;

		return c1->Identifier.Compare(c2->Identifier);
	}

	if( !(c1->Caption.IsEmpty() && c2->Caption.IsEmpty()) )
	{
		if( c1->Caption.IsEmpty() ) return 1;
		if( c2->Caption.IsEmpty() ) return -1;

		Comp = c1->Caption.Compare(c2->Caption);
		if( Comp != 0 )
			return Comp;
	}

	if( !(c1->Formula.IsEmpty() && c2->Formula.IsEmpty()) )
	{
		if( c1->Formula.IsEmpty() ) return 1;
		if( c2->Formula.IsEmpty() ) return -1;

		Comp = c1->Formula.Compare(c2->Formula);
		if( Comp != 0 )
			return Comp;
	}

	if( c1->TabOrder < c2->TabOrder ) return -1;
	if( c1->TabOrder > c2->TabOrder ) return 1;
	return 0;
}

int __cdecl CFormControl::CompareByTabOrder(const void *elem1, const void *elem2)
{
	CFormControl *c1 = *(CFormControl**)elem1, *c2 = *(CFormControl**)elem2;
	
	if( c1->TabOrder < c2->TabOrder ) return -1;
	if( c1->TabOrder > c2->TabOrder ) return 1;
	return 0;
}

/////////////////////////////////////////////////////////////////
//////  CFormColumn  ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
void CFormColumn::Setup()
{
	ColumnType = 0;
	Width = 0;
	TabOrder = 0;
	ValueFlags = 0;  //1 - неотрицательный, 2 - разделять триады. Комбинируются по OR.
	TextAlignment = 0;
	BasicFlags = 0;

	Type.SetNumeric();
	BasicFlags.SetTable(BasicFlagList);
	ValueFlags.SetTable(ValueFlagList);
}

CFormColumn::CFormColumn()
{
	Setup();
}

CFormColumn::CFormColumn(const char* aClass, const char* aIdentifier)
{
	Setup();
	Class = aClass;
	Identifier = aIdentifier;
}

CFormColumns::CFormColumns()
{
	Parent = NULL;
}

void CFormColumns::BeautyString(CBeautyString& String, int level)
{
	for( int i = 0; i < GetNChildren(); i++ )
		((CFormControlBase*)GetChild(i))->CorrectObjectID(Parent);

	CMMSAttributes::BeautyString(String, level);
}

/////////////////////////////////////////////////////////////////
//////  CFormControls  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////
CFormControls::CFormControls()
{
	sID = "Controls";
	Parent = NULL;
}

void CFormControls::BeautyString(CBeautyString& String, int level)
{
	if( TaskParameters.SortFormControls )
	{
		qsort(Children.GetData(), Children.GetSize(), sizeof(CMMSObject*), 
			CFormControl::CompareByIdentifier);
	}

	for( int i = 0; i < GetNChildren(); i++ )
		((CFormControlBase*)GetChild(i))->CorrectObjectID(Parent);

	CMMSAttributes::BeautyString(String, level);
}

void CFormControls::ToString(CMMSString& String)
{
	if( TaskParameters.SortFormControls )
	{
		qsort(Children.GetData(), Children.GetSize(), sizeof(CMMSObject*), 
			CFormControl::CompareByTabOrder);
	}

	CMMSAttributes::ToString(String);
}

/////////////////////////////////////////////////////////////////
//////  CFormBrowser  ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////
CMMSObject* CFormMulticolumn::CreateChild(const char* Name, const char* Value)
{
	CFormControl* control = new CFormControl(Name, Value);
	control->Font.FontBold.UnNeeded();
	return control;
}

void CFormMulticolumn::BeautyString(CBeautyString& String, int level)
{
	if( GetNChildren() > 0 )
	{
		CFormControl* browser = (CFormControl*)GetChild(0);
		//У параметров многострочной части не может быть типа. Откуда он берётся в 1С - непонятно
		browser->Identifier = "";
		browser->Type.NeedType = false;
		//Ссылки на поле тоже не может быть
		browser->ObjectID.iTypeObjectID = 0;
		browser->ObjectID.sTypeObjectID = "0";
	}
	CMMSAttributes::BeautyString(String, level);
}

CFormBrowser::CFormBrowser()
{
	Param1 = 0;
	Param2 = 0;
	ToDestroyChildren = false;
}

bool CFormBrowser::SetValue(int MMS_Index, GCString& Value)
{
	return CStoredObject::SetValue(MMS_Index, Value);
}

CMMSObject* CFormBrowser::CreateChild(const char* Name, const char* Value)
{
	if( _stricoll(Name, Multicolumn.BeautyName()) == 0 )
		return &Multicolumn;
	if( _stricoll(Name, Fixed.BeautyName()) == 0 )
		return &Fixed;
	return NULL;
}

CMMSObject* CFormBrowser::CreateChild(const char* Name, int MMS_Index)
{
	switch( MMS_Index )
	{
	case 2:
		return &Multicolumn;
	case 3:
		return &Fixed;
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////
//////  CDialogFrame  ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////
CDialogFrameContent::CDialogFrameContent()
{
	Width = Height = 0;
	Flags1 = 0;
	FontDefault = true;
	AutoTabOrder = true;
	BackgroundColor = -1; //-1 - default. В hex-формате: 0xBBGGRR
	PictureID = 0;
	Flags2 = 0; //дублирует Flags1
	ShowPanel = true;
	AllowResize = true;

	Flags1.SetTable(FlagList);
	Flags2.SetTable(FlagList);

	ToDestroyChildren = false;
}

CMMSObject* CDialogFrameContent::CreateChild(const char* Name, const char* Value)
{
	if( _stricoll(Name, Layers.BeautyName()) == 0 )
		return &Layers;
	if( _stricoll(Name, Font.BeautyName()) == 0 )
		return &Font;
	return NULL;
}

CMMSObject* CDialogFrameContent::CreateChild(const char* Name, int MMS_Index)
{
	switch( MMS_Index )
	{
		case 28: return &Layers;
	}
	return NULL;
}

void CDialogFrameContent::BeautyString(CBeautyString& String, int level)
{
	if( FontDefault && !TaskParameters.NoDefaults )
		Font.FontNeeded = false; //Font = DefVal->Font;

	String.add_parameter(level, BeautyName(), "", "", true);
	String.add_brace(level, '{');
	CMMSObject::BeautyString(String, level+1);
	String.add_brace(level, '}');
}

CDialogFrame::CDialogFrame()
{
	ToDestroyChildren = false;
}

CMMSObject* CDialogFrame::CreateChild(const char* Name, int MMS_Index)
{
	return &Content;
}

void CFrameBraces::BeautyString(CBeautyString& String, int level)
{
	if( TaskParameters.NoDefaults )
	{
		String.add_parameter(level, BeautyName(), "", "", true);
		String.add_brace(level, '{');
		String.add_brace(level, '}');
	}
}

void CFrameBraces::ToString(CMMSString& String)
{
	String += "\r\n{\r\n{\r\n{}},\r\n{}}";
}

/////////////////////////////////////////////////////////////////
//////  CDialogForm  ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
CDialogForm::CDialogForm(CMMSObject* aParent)
{
	//Cnt_Ver = "10001";
	Browser.Fixed.Parent = aParent;
	Fixed.Parent = aParent;
	Controls.Parent = aParent;
	ToDestroyChildren = false;
}

CMMSObject* CDialogForm::CreateChild(const char* Name, int MMS_Index)
{
	if( MMS_Index == 2 && Name[0] == '\0' )
		return &Braces;
	if( strcmp(Name, Frame.MMSName()) == 0 )
		return &Frame;
	if( strcmp(Name, Browser.MMSName()) == 0 )
		return &Browser;
	if( strcmp(Name, "Fixed") == 0 )
		return &Fixed;
	if( strcmp(Name, "Controls") == 0 )
		return &Controls;
	if( strcmp(Name, "Cnt_Ver") == 0 )
		return &Cnt_Ver;

	return NULL;
}

CMMSObject* CDialogForm::CreateChild(const char* Name, const char* Value)
{
	if( _stricoll(Name, Frame.BeautyName()) == 0 )
		return &Frame;
	if( _stricoll(Name, Braces.BeautyName()) == 0 )
		return &Braces;
	if( _stricoll(Name, Browser.BeautyName()) == 0 )
		return &Browser;
	if( _stricoll(Name, Fixed.BeautyName()) == 0 )
		return &Fixed;
	if( _stricoll(Name, Controls.BeautyName()) == 0 )
		return &Controls;
	if( strcmp(Name, "Cnt_Ver") == 0 )
		return &Cnt_Ver;

	return NULL;
}


void CDialogForm::ToString(CMMSString& String)
{
	CFormControl::SetDefaulFont(Frame.Content.Font);

	if( FindChild(Controls.sID) == NULL )
		AddChild(&Controls);

	if( FindChild(Cnt_Ver.sID) == NULL )
	{
		Cnt_Ver.SetValue(-1, GCString("10001"));
		AddChild(&Cnt_Ver);
	}

	CMMSAttributes::ToString(String);

	String += "  ";//В 1С накосячили, а мы повторяем
}

void CDialogForm::WriteToFile(CStorage& Storage, GCString FileName)
{
	CBeautyString String;
	BeautyString(String, 0);
	Storage.CopyStringToFile(String, FileName);
}

