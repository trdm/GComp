#!/usr/bin/perl

#==============================================================================================================
# Скрипт для коррекции диалоговых форм. Исправляется ситуация, когда верхние элементы диалога "выползают" за
# верхний край формы. В этом случае все элементы диалога опускаются вниз, и высота окна увеличивается.
# Диалоговые формы должны быть в human-readable формате, т.е. разобранные gcomp версией
# не ниже 2.0.10, без ключа --no-parse-dialogs
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
use strict "vars";
use vars qw/ $usage
	$root_dir $min_y $shift_all_forms $shift_y $Param25 $keep_old /;

$usage = <<EOF
Использование:
   perl correct_dlg.pl [параметры]
Параметры могут быть:
   -h        - этот текст
   -miny <n> - минимальная Y-координата элементов диалога. Если самый верхний элемент диалога имеет
               Y-координату меньше указанной, то все элементы опускаются на разницу координат (minY - minItemY)
   -shifty <n> - сдвиг по Y-координате. Отрицательное значение - сдвиг вверх, положительное - вниз.
   -p25 <n>  - корректирует Param25
   -d <dir>  - каталог, откуда начнётся поиск диалоговых форм (*.frm). По умолчанию поиск начинается с
               текущего каталога.
   -k        - сохраняет исходные формы нетрнутыми, изменения записываются в файлы *.new.frm
EOF
;

#default values
$min_y = 17;
$root_dir = ".";
$keep_old = 0;
$shift_all_forms = 0;

#command line args processing
use Getopt::Long;
GetOptions(
	"dir|d=s" => \$root_dir,
	"miny=s" => \$min_y, 
	"shifty=s" => \$shift_y,
	"p25=s" => \$Param25,
	"keep|k" => \$keep_old
) 
	or die wintodos($usage);

$shift_all_forms = 1 if $shift_y;
#print "min Y = $min_y, keep = $keep_old\n";

#traverse through dirs and do work
use File::Find; #package для рекурсивного обхода каталогов
find(\&CorrectDialog, $root_dir);

sub CorrectDialog
{
	my $name = $_; #File::Find складывает в $_ имя текущего файла
	return unless $name =~ m/^(.*)(\.frm)$/i; #нам нужны только файлы с расширением frm
	return if $name =~ m/^(.*)(\.new\.frm)$/i; #нам не нужны файлы с расширением new.frm

	my $new_name = "$1.new.frm";
	my ($line, $dlg_min_y, $y, $dlg_height, $v_shift);
	my $ToSetParam25;

	if( $shift_all_forms )
	{
		$v_shift = $shift_y;
		print wintodos("$File::Find::name: смещение = $v_shift\n");
	}
	else
	{
		$dlg_min_y = $min_y + 10;
		open DLG, "< $name";
		foreach $line (<DLG>)
		{
			if( $line =~ m/^\s*Высота:\s*(\d+).*$/ and !defined $dlg_height )
			{
				$dlg_height = $1;
			}
			elsif( $line =~ m/^\s*Y:\s*(\d+).*$/ )
			{
				$y = $1;
				$dlg_min_y = $y if $y < $dlg_min_y;
			}
			elsif( $Param25 and $line =~ m/^\s*Param25:\s*(\d+).*$/ )
			{
				$ToSetParam25 = $1 if $Param25 ne $1;
			}
		}
		close DLG;

		return if $dlg_min_y >= $min_y and !$ToSetParam25;
		$v_shift = $min_y - $dlg_min_y;

		print wintodos("$File::Find::name\n") if $v_shift or $ToSetParam25;
		print wintodos("   Param25: $ToSetParam25  --> $Param25\n") if $ToSetParam25;
		print wintodos("   min. Y = $dlg_min_y смещение = $v_shift\n") if $v_shift > 0;
	}
	
	$v_shift = 0 if $v_shift < 0;

	if( $v_shift > 0 or $ToSetParam25 )
	{
		my $height;
		open DLG, "< $name";
		open DLG_NEW, "> $new_name";
		foreach $line (<DLG>)
		{
			$line =~ s/[\r\n]//g; #уберём завершающие \n и \r, если они там есть

			if( $line =~ m/^(\s*Высота:\s*)(\d+)(.*)$/i )
			{
				if( $height == 0 )
				{
					$height = $2 + $v_shift;
				}
				else
				{
					$height = $2;
				}
				$line = sprintf "%s%i%s", $1, $height, $3;
			}
			elsif( $line =~ m/^(\s*Y:\s*)(\d+)(.*)$/i )
			{
				$line = sprintf "%s%i%s", $1, $2 + $v_shift, $3;
			}
			elsif( $ToSetParam25 and $line =~ m/^(\s*Param25:\s*)(\d+)(.*)$/i )
			{
				$line = sprintf "%s%i%s", $1, $Param25, $3;
			}
			
			printf DLG_NEW "%s\n", $line;
		}
		close DLG;
		close DLG_NEW;

		if( not $keep_old )
		{
			if( !rename($new_name, $name) )
			{
				print wintodos("Can't move $new_name to $name: $!\n")
			}
		}
	}
}

sub wintodos {
	my $win_chars = "\xA8\xB8\xB9\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF";
	my $dos_chars = "\xF0\xF1\xFC\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF";
	$_ = shift;
	return $_ if $^O eq "cygwin";
	eval("tr/$win_chars/$dos_chars/");
	return $_;
}