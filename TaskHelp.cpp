// TaskHelp.cpp: implementation of the CTaskHelp class.
//////////////////////////////////////////////////////////////////////

#include "Task.h"
#include "CommonFunctions.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


void CTask::HelpRu()
{
	Msg(0,"\nЗапуск: gcomp.exe <Параметр>\n\nПараметры: ");
	Msg(0,"  -h - Показать эту помощь");
	Msg(0,"  -d - Разобрать файл 1cv7.md в папку SRC");
	Msg(0,"  -c - Собрать папку SRC в файл 1cv7.md");
	Msg(0,"  --version - показать текущую версию GComp и выйти");
	Msg(0,""); /*------------------------*/
	Msg(0,"Вы можете использовать следующие дополнительные параметры");
	Msg(0,"  -q - Молчаливый режим. Никаких сообщений вообще.");
	Msg(0,"  -v - Сообщений чуть больше, чем по умолчанию.");
	Msg(0,"  -vv - Еще больше сообщений.");
	Msg(0,"  -F <ИмяФайла>");
	Msg(0,"  -D <ИмяДиректории>");
	Msg(0,"  -DD <ИмяДиректории>");
	Msg(0,"  --external-report - указать, что работаем с внешним отчетом (.ert формат)");
	Msg(0,"  --meta-data - указать, что работаем с файлом конфигурации (1cv7.md формат)");
	Msg(0,"  --no-order - не сохранять порядок объектов в конфигурации. Опция работает только при декомпиляции. При компиляции объекты будут располагаться в том порядке, как операционная система выдаст каталоги.");
	Msg(0,"  --no-profiles - Пропустить при работе файлы (потоки), с именем Container.Profile");
	Msg(0,"  --no-empty-mxl - Пропускать при работе пустые шаблоны печатных форм");
	Msg(0,"  --truncate-mms - При сборке внешних отчетов gcomp заменит поток 'Main MetaData Stream' на пустой");
	Msg(0,"  --eng - GComp попытается создать папки и файлы с английскими именами (данная опция находится в состоянии beta)");
	Msg(0,"  --translit - GComp попытается создать папки и файлы с транслитерованными именами (данная опция находится в состоянии beta)");
	Msg(0,"	   * Опции --eng и --translit не проверяют уникальности получаемых имен.");
	Msg(0,"");
	Msg(0,"  --no-defaults - выводятся все параметры выгружаемых объектов. Без этой опции выводится только та информация, которая отличается от значений по умолчанию. Опция действует только при декомпиляции.");
	Msg(0,"  --no-parse-dialogs - диалоговые формы будут выгружены As Is, без какой либо обработки. При компиляции формат диалогов определяется автоматически.");
	Msg(0,"  --no-version - при декомпиляции не будет сформирован файл gcomp.ver с информацией о версии gcomp.");
	Msg(0,"  --no-empty-folders - при декомпиляции будут удалены пустые папки (для SVN репозиториев).");
	Msg(0,"  --filter <Имя объекта> - выборочная декомпиляция/компиляция. Имя объекта задаётся как путь к каталогу или файлу. Например:");
	Msg(0,"\tСправочники\\Контрагенты");
	Msg(0,"\tСправочники\\Контрагенты\\МодульФормы");
	Msg(0,"\tДокумент\\*\\МодульПроведения");
	Msg(0,"В качестве разделителей можно использовать символы '\\', '/' и '.'");
	Msg(0,"  --more-subfolders - объекты, которые по умолчанию выгружаются в виде одного mdp-файла, будут разобраны по каталогам. Это: константы, перечисления, общие поля документов, нумераторы, графы отбора документов, последовательности документов, регистры, группы расчетов, правила перерасчета, календари");
	Msg(0,"  --sort-rights - объекты приложения пользовательских прав будут отсортированы по типу объекта (Константа, Справочник, Документ и т.д.) и по идентификатору объекта.");
	Msg(0,"  --sort-form-controls - сортировать элементы диалоговых форм по идентификатору. Элементы без идентификаторов сортируются по порядку обхода.");
	Msg(0,"  --no-broken-links - не выгружать объекты прав и интерфейсов, ссылающиеся на отсутствующие метаданные.");
	Msg(0,"  --save-all-rights - сохранить все наборы прав в одном csv файле (при компиляции игнорируется)");
	Msg(0,""); /*------------------------*/
	Msg(0,"По умолчанию, внешние отчеты обрабатываются особым способом.");
	Msg(0,"Имя для папки разбора формируется следующим образом:");
	Msg(0,"<СтартовыйКаталог>\\<ИмяФайлаБезРасширения>");
	Msg(0,"Однако, если указать параметр -DD, то имя каталога останется таким, как указано в ключе -DD");
	Msg(0,""); /*------------------------*/
	Msg(0,"Примеры:");
	Msg(0," > gcomp -d");
	Msg(0,"Разберет содержимое 1cv7.md в каталог SRC");
	Msg(0," > gcomp -d -D C:\\Papka");
	Msg(0,"Разберет содержимое 1cv7.md в каталог C:\\Papka");
	Msg(0," > gcomp -d -F TestReport.ert -D Reports");
	Msg(0,"Разберет содержимое TestReport.ert в каталог Reports\\TestReport");
	Msg(0," > gcomp -d -F TestReport.ert");
	Msg(0,"Разберет содержимое TestReport.ert в каталог SRC\\TestReport");
	Msg(0," > gcomp -d --external-report");
	Msg(0,"Разберет содержимое 1cv7.md в каталог SRC\\1cv7");
	Msg(0," > gcomp -c -F TestReport.ert");
	Msg(0,"Соберет файл TestReport.ert из каталога SRC\\TestReport");
	Msg(0," > gcomp -c -F TestReport.ert -DD TestReportSource");
	Msg(0,"Соберет файл TestReport.ert из каталога TestReportSource");
	Msg(0,""); /*------------------------*/
	Msg(0,"Предупреждение: декомпиляция в непустой каталог может вызвать потерю данных!!!");
	Msg(0,"Будьте внимательны!!!");
	Msg(0,"");/*------------------------*/
	Msg(0,"Используйте \"gcomp -h | more\", для постраничного просмотра помощи, или");
	Msg(0,"\"gcomp -h > gcomp_hlp.txt\" для создания файла с текстом помощи.");
}

void CTask::HelpEn()
{
	Msg(0,"\nUsage: gcomp.exe <Parameter>\n");
	Msg(0,"Parameterss: ");
	Msg(0,"  -h - Show this help");
	Msg(0,"  -d - Decompile 1cv7.md into SRC folder");
	Msg(0,"  -c - Compile SRC folder into 1cv7.md");
	Msg(0,"  --version - show current GComp version and exit");
	Msg(0,"");/*------------------------*/
	Msg(0,"You may use this optional parameters");
	Msg(0,"  -q - Quiet mode. No messages in stdout at all.");
	Msg(0,"  -v - Verbose gcomp job.");
	Msg(0,"  -vv - Much more verbose gcomp job.");
	Msg(0,"  -F <FileName>");
	Msg(0,"  -D <Directory>");
	Msg(0,"  -DD <Directory>");
	Msg(0,"  --external-report - tell to gcomp, that the file is external report (.ert style)");
	Msg(0,"  --meta-data - tell to gcomp, that the file is metadata (1cv7.md style)");
	Msg(0,"  --no-profiles - gcomp will skip files (streams), named Container.Profile");
	Msg(0,"  --no-empty-mxl - gcomp will throw away empty mxl patterns");
	Msg(0,"  --truncate-mms - when compiling external reports gcomp will repalce original 'Main MetaData Stream' with empty copy");
	Msg(0,"  --eng - GComp will be try to create folders and files with english names (beta state)");
	Msg(0,"  --translit - GComp will be try to create folders and files with translit names (beta state)");
	Msg(0,"		\'--eng\' and \'--translit\' options didn't check originality of result names.");
	Msg(0,"  --no-defaults - objects will be extracted with all propertys regardless of defaul values. Works only for decompile.");
	Msg(0,"  --no-version - file gcomp.ver (info about version of gcomp) will not create on decompile.");
	Msg(0,"  --no-empty-folders - empty folders will be deleted on decompile (for SVN repository).");
	Msg(0,"  --filter <ObjectName> - selective compilation/decompilation. Object name as path to folder or file. For example:");
	Msg(0,"\tСправочники\\Контрагенты");
	Msg(0,"\tСправочники\\Контрагенты\\МодульФормы");
	Msg(0,"\tДокумент\\*\\МодульПроведения");
	Msg(0,"It is possible to use simbols '\\', '/' and '.' as separators");
	Msg(0,"  --more-subfolders - Objects, decompiled as one mdp-file, will be decompile in folder. This is: constants, enumerators, common document fields, numerators, document fields of selectiopn, document orders, registers, calc groups, calc rules, calendars");
	Msg(0,"  --sort-rights - user rights object willbe sorted by type (Constant, Reference, Document etc) and by object identifier.");
	Msg(0,"  --sort-form-controls - sort dialog form controls by identifier. Controls without identifier sorted by taborder.");
	Msg(0,"  --save-all-rights - save all user rights in one csv file (ignored on compile)");
	Msg(0,"");/*------------------------*/
	Msg(0,"By default external reports treated in some special way.");
	Msg(0,"When they extracted/compiled the destination/source directory formed as");
	Msg(0,"<StartDirectory>\\<FileNameWithoutExtension>");
	Msg(0,"But if you specify -DD parameter, directory name will be");
	Msg(0,"leaved unchanged.");
	Msg(0,"");/*------------------------*/
	Msg(0,"For example:");
	Msg(0," > gcomp -d");
	Msg(0,"will extract contents of 1cv7.md into directory SRC");
	Msg(0," > gcomp -d -D C:\\Papka");
	Msg(0,"will extract contents of 1cv7.md into directory C:\\Papka");
	Msg(0," > gcomp -d -F TestReport.ert -D Reports");
	Msg(0,"will extract contents of TestReport.ert into directory Reports\\TestReport");
	Msg(0," > gcomp -d -F TestReport.ert");
	Msg(0,"will extract contents of TestReport.ert into directory SRC\\TestReport");
	Msg(0," > gcomp -d --external-report");
	Msg(0,"will extract contents of 1cv7.md into directory SRC\\1cv7");
	Msg(0," > gcomp -c -F TestReport.ert");
	Msg(0,"will compile file TestReport.ert from directory SRC\\TestReport");
	Msg(0," > gcomp -c -F TestReport.ert -DD TestReportSource");
	Msg(0,"will compile file TestReport.ert from directory TestReportSource");
	Msg(0,"");/*------------------------*/
	Msg(0,"Warning: decompiling into not empty directory may cause data loss!!!");
	Msg(0,"Be carefull!!!");
	Msg(0,"");/*------------------------*/
	Msg(0,"Use \"gcomp -h | more\", for paging help message, or");
	Msg(0,"\"gcomp -h > gcomp_hlp.txt\" for create help file");
}

void CTask::Help()
{
	if( lang_Eng == TaskParameters.Language )
		HelpEn();
	else
		HelpRu();

}
