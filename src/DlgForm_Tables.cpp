
#include "DialogForm.h"


CHotKey::VK_Pair CHotKey::virtual_keys[] = {
	{0x01, "LButton"},
	{0x02, "RButton"},
	{0x08, "BackSpase"},
	{0x09, "Tab"},
	//{0x10, "Shift"},
	//{0x11, "Ctrl"},
	//{0x12, "Alt"},
	{0x0d, "Enter"},
	{0x13, "Pause"},
	{0x14, "CapsLock"},
	{0x1b, "Esc"},
	{0x20, "Space"},
	{0x21, "PgUp"},
	{0x22, "PgDn"},
	{0x23, "End"},
	{0x24, "Home"},
	{0x25, "Left"},
	{0x26, "Up"},
	{0x27, "Right"},
	{0x28, "Down"},
	{0x2c, "PrintScreen"},
	{0x2d, "Ins"},
	{0x2e, "Del"},
	{0x5d, "Apps"},
	{0x60, "Gray0"},
	{0x61, "Gray1"},
	{0x62, "Gray2"},
	{0x63, "Gray3"},
	{0x64, "Gray4"},
	{0x65, "Gray5"},
	{0x66, "Gray6"},
	{0x67, "Gray7"},
	{0x68, "Gray8"},
	{0x69, "Gray9"},
	{0x6a, "Gray*"},
	{0x6b, "Gray+"},
	{0x6c, "Separator"},
	{0x6d, "Gray-"},
	{0x6e, "GrayDecimal"},
	{0x6f, "Gray/"},
	{0x70, "F1"},
	{0x71, "F2"},
	{0x72, "F3"},
	{0x73, "F4"},
	{0x74, "F5"},
	{0x75, "F6"},
	{0x76, "F7"},
	{0x77, "F8"},
	{0x78, "F9"},
	{0x79, "F10"},
	{0x7a, "F11"},
	{0x7b, "F12"},
	{0x90, "NumLock"},
	{0x91, "ScrollLock"},
	{0xa0, "LShift"},
	{0xa1, "RShift"},
	{0xa2, "LControl"},
	{0xa3, "RControl"},
	{0x90, "NumLock"},
	{0, ""}
};

//===============================================================================
//======  CFormFont  ============================================================
//===============================================================================
CFormFont* CFormFont::DefVal = new CFormFont;
CObjectProperty CFormFont::Props[] = {
	{"Шрифт",   13, CObjectProperty::t_string, VAR_OFFSET(DefVal, FontName)},
	{"Размер",  0,  CObjectProperty::t_prop, VAR_OFFSET(DefVal, FontSize)},
	{"Param2",  1,  CObjectProperty::t_int,  VAR_OFFSET(DefVal, Param2)},
	{"Param3",  2,  CObjectProperty::t_int,  VAR_OFFSET(DefVal, Param3)},
	{"Param4",  3,  CObjectProperty::t_int,  VAR_OFFSET(DefVal, Param4)},
	{"Жирный",  4,  CObjectProperty::t_prop, VAR_OFFSET(DefVal, FontBold)},
	{"Наклон",  5,  CObjectProperty::t_prop, VAR_OFFSET(DefVal, FontItalic)},
	{"Подчёркивание", 6, CObjectProperty::t_prop, VAR_OFFSET(DefVal, FontUnderline)},
	{"Param8",  7,  CObjectProperty::t_int, VAR_OFFSET(DefVal, Param8)},
	{"Param9",  8,  CObjectProperty::t_int, VAR_OFFSET(DefVal, Param9)},
	{"Param10", 9,  CObjectProperty::t_int, VAR_OFFSET(DefVal, Param10)},
	{"Param11", 10, CObjectProperty::t_int, VAR_OFFSET(DefVal, Param11)},
	{"Param12", 11, CObjectProperty::t_int, VAR_OFFSET(DefVal, Param12)},
	{"Param13", 12, CObjectProperty::t_int, VAR_OFFSET(DefVal, Param13)},
	{NULL, 0, CObjectProperty::t_int, 0}
};
CPropertySet CFormFont::PropSet(Props, DefVal);


//===============================================================================
//======  CFormControlBase  =====================================================
//===============================================================================
char* CFormControlBase::FlagList[] = {
	"Flag0",				//0
	"Flag1",				//1
	"Flag2",				//2
	"КартинкаПоЦентру",		//3
	"Flag4",				//4
	"ПрижатьТекстВлево",	//5 (CHECKBOX - ПрижатьТекстВлево; 1CEDIT - для ввода пароля)
	"РастянутьКартинку",	//6
	"Flag7",				//7
	"Flag8",				//8
	"Flag9",				//9
	"Flag10",				//10
	"Flag11",				//11
	"КнопкаПоУмолчанию",	//12
	"Flag13",				//13
	"Flag14",				//14
	"Flag15",				//15
	"Flag16",				//16
	"ПервыйВГруппе",		//17
	"Flag18",				//18
	"Flag19",				//19
	"Flag20",				//20
	"Flag21",				//21
	"Flag22",				//22
	"Flag23",				//23
	"Flag24",				//24
	"Flag25",				//25
	"Flag26",				//26
	"Flag27",				//27
	"Flag28",				//28
	"Flag29",				//29
	"Flag30",				//30
	"Flag31"				//31
};

char* CFormControlBase::BasicFlagList[] = {
	"Flag0",						//0
	"Flag1",						//1
	"ПропускатьПриВводе",			//2
	"Flag3",						//3
	"ИспользоватьОписание",			//4
	"Педалька",						//5 // нужно назвать нормально. кнопка выбора. Но не сейчас.
	"ШрифтПоУмолчанию",				//6
	"ЦветТекстаАвто",				//7
	"РастянутьКартинку",			//8
	"НетАвтоВыбора",				//9
	"КартинкаПоЦентру",				//10
	"КартинкаПропорционально",		//11
	"Многострочное",				//12
	"Невидимо",						//13
	"Недоступно",					//14
	"Flag15",						//15
	"ПрозрачныйФон",				//16
	"ОбычнаяРамка",					//17
	"ВыпуклаяРамка",				//18
	"ВогнутаяРамка",				//19
	"АвтоВыбор",					//20
	"Flag21",						//21
	"СНовойСтроки",					//22
	"ВТойЖеКолонке",				//23
	"НеСохранять",					//24
	"СПометками",					//25
	"ОтрицательноеКрасным",			//26
	"ПоказыватьПиктограммы",		//27
	"ЗапретитьРедактирование",		//28
	"Flag29",						//29
	"Flag30",						//30
	"Flag31"						//31
};

char* CFormControlBase::ValueFlagList[] = {
	"Неотрицательный",
	"РазделятьТриады",
	"Bit2", //на всякий случай добавим битов
	"Bit3",
	"Bit4",
	"Bit5",
	"Bit6",
	"Bit7"
};

//===============================================================================
//======  CFormControl  =========================================================
//===============================================================================
CFormControl* CFormControl::DefVal = new CFormControl;
CObjectProperty CFormControl::Props[] = {
	{"Класс",          1, CObjectProperty::t_string, VAR_OFFSET(DefVal, Class)},
	{"Идентификатор", 12, CObjectProperty::t_string, VAR_OFFSET(DefVal, Identifier)},
	{"Заголовок",      0, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Caption)},
	{"ИдентификаторМетаданных", 13, CObjectProperty::t_prop, VAR_OFFSET(DefVal, ObjectID)},
	{"Тип",           14, CObjectProperty::t_subobject, VAR_OFFSET(DefVal, Type)},
	{"ФлагиЗначения", 18, CObjectProperty::t_prop, VAR_OFFSET(DefVal, ValueFlags)},
	{"ДопФлаги",       2, CObjectProperty::t_prop, VAR_OFFSET(DefVal, Flags)},
	{"X",              3, CObjectProperty::t_int, VAR_OFFSET(DefVal, PosX)},
	{"Y",              4, CObjectProperty::t_int, VAR_OFFSET(DefVal, PosY)},
	{"Ширина",         5, CObjectProperty::t_int, VAR_OFFSET(DefVal, Width)},
	{"Высота",         6, CObjectProperty::t_int, VAR_OFFSET(DefVal, Height)},
	{"Атрибут",        7, CObjectProperty::t_bool, VAR_OFFSET(DefVal, IsObjectAttribute)},
	{"Param9",         8, CObjectProperty::t_int, VAR_OFFSET(DefVal, Param9)},
	{"ПорядокОбхода",  9,  CObjectProperty::t_int, VAR_OFFSET(DefVal, TabOrder)},
	{"Param11",       10, CObjectProperty::t_string, VAR_OFFSET(DefVal, Param11)},
	{"Формула",       11, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Formula)},
	{"ОснФлаги",      19, CObjectProperty::t_prop, VAR_OFFSET(DefVal, BasicFlags)},
	{"Маска",         20, CObjectProperty::t_string, VAR_OFFSET(DefVal, Mask)},
	{"Hint",          21, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, LongHint)},
	{"ShortHint",     22, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, ShortHint)},
	{"Param24",       23, CObjectProperty::t_int, VAR_OFFSET(DefVal, Param24)},
	{"Шрифт",         24, CObjectProperty::t_subobject, VAR_OFFSET(DefVal, Font)},
	{"ЦветТекста",    38, CObjectProperty::t_int, VAR_OFFSET(DefVal, FontColor)},
	{"Param40",       39, CObjectProperty::t_int, VAR_OFFSET(DefVal, Param40)},
	{"Пиктограмма",   40, CObjectProperty::t_uint, VAR_OFFSET(DefVal, PictureID)},
	{"Слой",          41, CObjectProperty::t_string, VAR_OFFSET(DefVal, Layer)},
	{"HotKey",        42, CObjectProperty::t_object, VAR_OFFSET(DefVal, HotKey)},
	{NULL, 0, CObjectProperty::t_int, 0}
};
CPropertySet CFormControl::PropSet(Props, DefVal);

//===============================================================================
//======  CFormColumn  ==========================================================
//===============================================================================
CFormColumn* CFormColumn::DefVal = new CFormColumn;
CObjectProperty CFormColumn::Props[] = {
	{"Класс",           3, CObjectProperty::t_string, VAR_OFFSET(DefVal, Class)},
	{"Идентификатор",   7, CObjectProperty::t_string, VAR_OFFSET(DefVal, Identifier)},
	{"Заголовок",       1, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Caption)},
	{"ИдентификаторМетаданных", 8,  CObjectProperty::t_prop, VAR_OFFSET(DefVal, ObjectID)},
	{"Тип",             9, CObjectProperty::t_subobject, VAR_OFFSET(DefVal, Type)},
	{"ФлагиЗначения",  13, CObjectProperty::t_prop, VAR_OFFSET(DefVal, ValueFlags)},
	{"ТипКолонки",      0, CObjectProperty::t_int, VAR_OFFSET(DefVal, ColumnType)},
	{"Ширина",          2, CObjectProperty::t_int, VAR_OFFSET(DefVal, Width)},
	{"ПорядокОбхода",   4, CObjectProperty::t_int, VAR_OFFSET(DefVal, TabOrder)},
	{"Param6",          5, CObjectProperty::t_string, VAR_OFFSET(DefVal, Param6)},
	{"Формула",         6, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Formula)},
	{"Param15",        14, CObjectProperty::t_string, VAR_OFFSET(DefVal, Param15)},
	{"Param16",        15, CObjectProperty::t_string, VAR_OFFSET(DefVal, Param16)},
	{"Выравнивание",   16, CObjectProperty::t_int, VAR_OFFSET(DefVal, TextAlignment)},
	{"ОснФлаги",       17, CObjectProperty::t_prop, VAR_OFFSET(DefVal, BasicFlags)},
	{"Маска",          18, CObjectProperty::t_string, VAR_OFFSET(DefVal, Mask)},
	{"Hint",           19, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, LongHint)},
	{"ShortHint",      20, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, ShortHint)},
	{"Param22",        21, CObjectProperty::t_string, VAR_OFFSET(DefVal, Param22)},
	{NULL, 0, CObjectProperty::t_int, 0}
};
CPropertySet CFormColumn::PropSet(Props, DefVal);


//===============================================================================
//======  CFormBrowser  =========================================================
//===============================================================================
CFormBrowser* CFormBrowser::DefVal = new CFormBrowser;
CObjectProperty CFormBrowser::Props[] = {
	//{"sID",         0, CObjectProperty::t_string,  VAR_OFFSET(DefVal, sID)},
	{"Param1",      0, CObjectProperty::t_int,  VAR_OFFSET(DefVal, Param1)},
	{"Param2",      1, CObjectProperty::t_int,  VAR_OFFSET(DefVal, Param2)},
	{"Multicolumn", 2, CObjectProperty::t_object,  VAR_OFFSET(DefVal, Multicolumn)},
	{"Fixed",       3, CObjectProperty::t_object,  VAR_OFFSET(DefVal, Fixed)},
	{NULL, 0, CObjectProperty::t_int, 0}
};
CPropertySet CFormBrowser::PropSet(Props, DefVal);



char* CDialogFrameContent::FlagList[] = {
	"FormFlag0",				//0
	"FormFlag1",				//1
	"FormFlag2",				//2
	"FormFlag3",				//3
	"FormFlag4",				//4
	"FormFlag5",				//5
	"FormFlag6",				//6
	"ЦветАвто",					//7
	"РастянутьКартинку",		//8
	"FormFlag9",				//9
	"КартинкаПоЦентру",			//10
	"КартинкаПропорционально",	//11
	"FormFlag12",				//12
	"FormFlag13",				//13
	"FormFlag14",				//14
	"FormFlag15",				//15
	"FormFlag16",				//16
	"FormFlag17",				//17
	"FormFlag18",				//18
	"FormFlag19",				//19
	"НеСохранятьНастройки",		//20
	"FormFlag21",				//21
	"FormFlag22",				//22
	"FormFlag23",				//23
	"FormFlag24",				//24
	"FormFlag25",				//25
	"FormFlag26",				//26
	"FormFlag27",				//27
	"FormFlag28",				//28
	"FormFlag29",				//29
	"FormFlag30",				//30
	"FormFlag31"				//31
};

//===============================================================================
//======  CFormLayer  ===========================================================
//===============================================================================
CFormLayer* CFormLayer::DefVal = new CFormLayer;
CObjectProperty CFormLayer::Props[] = {
	{"sID",       0, CObjectProperty::t_string,  VAR_OFFSET(DefVal, sID)},
	{"Видимость", 1, CObjectProperty::t_prop,  VAR_OFFSET(DefVal, IsVisible)},
	{NULL, 0, CObjectProperty::t_int, 0}
};
CPropertySet CFormLayer::PropSet(Props, DefVal);

//===============================================================================
//======  CDialogFrameContent  ==================================================
//===============================================================================
CDialogFrameContent* CDialogFrameContent::DefVal = new CDialogFrameContent;
CObjectProperty CDialogFrameContent::Props[] = {
	{"Шрифт",               0, CObjectProperty::t_subobject,  VAR_OFFSET(DefVal, Font)},
	{"Ширина",              14, CObjectProperty::t_int,    VAR_OFFSET(DefVal, Width)},
	{"Высота",              15, CObjectProperty::t_int,    VAR_OFFSET(DefVal, Height)},
	{"Заголовок",           16, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Caption)},
	{"Param18",             17, CObjectProperty::t_string, VAR_OFFSET(DefVal, Param18)},
	{"Param19",             18, CObjectProperty::t_string, VAR_OFFSET(DefVal, Param19)},
	{"Флаги",               19, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, Flags1)},
	{"Param21",             20, CObjectProperty::t_string, VAR_OFFSET(DefVal, Param21)},
	{"ШрифтПоУмолчанию",    21, CObjectProperty::t_bool,   VAR_OFFSET(DefVal, FontDefault)},
	{"АвтоПорядокОбхода",   22, CObjectProperty::t_bool,   VAR_OFFSET(DefVal, AutoTabOrder)},
	{"Param24",             23, CObjectProperty::t_string, VAR_OFFSET(DefVal, Param24)},
	{"Param25",             24, CObjectProperty::t_string, VAR_OFFSET(DefVal, Param25)},
	{"ЦветФона",            25, CObjectProperty::t_int,    VAR_OFFSET(DefVal, BackgroundColor)},
	{"ИдентификаторКартинки", 26, CObjectProperty::t_uint, VAR_OFFSET(DefVal, PictureID)},
	{"Флаги2",              27, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, Flags2)},
	{"Слои",                28, CObjectProperty::t_object, VAR_OFFSET(DefVal, Layers)},
	{"ПанельИнструментов",  29, CObjectProperty::t_bool,   VAR_OFFSET(DefVal, ShowPanel)},
	{"ИзменятьРазмер",      30, CObjectProperty::t_bool,   VAR_OFFSET(DefVal, AllowResize)},
	{NULL, 0, CObjectProperty::t_int, 0}
};
CPropertySet CDialogFrameContent::PropSet(Props, DefVal);
