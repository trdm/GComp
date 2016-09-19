#!/usr/bin/perl

#==============================================================================================================
# Скрипт для массовой вставки кода в модули.
#
#==============================================================================================================
#Автор - Диркс Алексей mailto:adirks@ngs.ru
#
#Эта программа является свободным программным обеспечением. Вы можете
#распространять и (или) модифицировать ее на условиях GNU Generic Public License.
#
#Данная программа распространяется с надеждой оказаться полезной, но
#БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ, в том числе без гарантий пригодности для продажи или
#каких-либо других практических целей.
#
#С полным текстом лицензии на английском языке можно ознакомитсья по адресу
#http://www.gnu.org/licenses/gpl.txt или в файле
#gnugpl.eng.txt
#
#С русским переводом лицензии можно ознакомиться по адресу
#http://gnu.org.ru/gpl.html или в файле
#gnugpl.rus.txt
#
#Вы должны получить копию GNU Generic Public License вместе с копией этой программы.
#Если это не так - сообщите об этом авторам (mailto:adirks@ngs.ru , mailto:fe@alterplast.ru) или напишите
#Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

use locale;
use File::Find; #package для рекурсивного обхода каталогов
use CodeWorks;

use strict "vars";
use vars qw/ $usage
	$root_dir $pattern_dir $backups $quiet $real_quiet /;

$usage = <<EOF
Использование:
   perl code_works.pl [параметры]
Параметры могут быть:
   -h        - этот текст
   -d <dir>  - каталог, откуда начнётся поиск модулей (*.1s). По умолчанию поиск начинается с
               текущего каталога.
   -p <dir>  - каталог с шаблонами кода. По умолчанию - текущий.
   -b        - будут создаваться копии исходных модулей (с расширением .1s~)
   -q        - тихий режим
   -qq       - совсем тихий режим
EOF
;

#default values
$root_dir = ".";
$pattern_dir = ".";
$backups = 0;
$quiet = 0;
$real_quiet = 0;

#command line processing
use Getopt::Long;
GetOptions(
	"dir|d=s" => \$root_dir,
	"patterns|p=s" => \$pattern_dir,
	"backup|b" => \$backups,
	"q" => \$quiet,
	"qq" => \$real_quiet
	) 
	or die CodeWorks::wintodos($usage);
$quiet = 1 if $real_quiet;

use Cwd;
$root_dir = Cwd::abs_path($root_dir);
$pattern_dir = Cwd::abs_path($pattern_dir);
#traverse through dirs and do work
File::Find::find(\&ProcessMod, $root_dir);


sub ProcessMod()
{
	my $dir = $File::Find::dir;
	my $fname = $_; #File::Find складывает в $_ имя текущего файла
	return unless $fname =~ m/^(.*)(\.1s)$/i; #нам нужны только файлы с расширением 1s

	my ($type, $ObjName, $ModuleType) = CodeWorks::GetObjectName($dir . "/" . $fname);
	my $pattern_fname = "$pattern_dir/$ModuleType.pl";
	
	if( not -f $pattern_fname )
	{
		$quiet or print CodeWorks::wintodos("$dir/$fname: нет шаблона ($pattern_fname)\n");
		return;
	}
	
	#load change rules
	open CODE, "<$pattern_fname";
	my $code = join("", (<CODE>));
	close CODE;

	CodeWorks::LoadModule($fname); #load module
	my $text = join("\n", @CodeWorks::lines);
	eval $code; #apply changes
	CodeWorks::PrintErrors();
	
	my $new_text = join("\n", @CodeWorks::lines);
	$text ne $new_text or return; #ничего не менялось - не будем и в файл писать
	
	#save changes
	open NEW_MODULE, "> $fname";
    print NEW_MODULE $new_text;
   	close NEW_MODULE;

	$real_quiet or print CodeWorks::wintodos("$dir/$fname: изменён ($pattern_fname)\n");
}
