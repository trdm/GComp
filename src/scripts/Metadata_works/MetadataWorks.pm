package Metadata;

#==============================================================================================================
# Модуль для работы с метаданными 1С.
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


our @ISA = qw(Exporter);
our @EXPORT = qw(Start Stop 
	FindByID FindBysID 
	AddRefField AddDocHeadField AddDocTableField
	SetFeildAttr
);

use locale;
use warnings;
use strict "vars";

#Экспортируемые переменные, в которые записываются служебные данные. Модифицировать не рекомендуется.
our(
	$dir,
	$AvailableID, #первый доступный числовой ID
	@IDs #массив идентификаторов. Каждый элемент - массив из 2-х значений ($ID, $sID). Пример обращения: @IDs[$i]->[0] или @IDs[$i]->[1]
);

my $metadata_ids_file = "ИдентификаторыКонфигурации.mdp";
my $metadata_objs_file = "ОбъектыМетаданных.txt";
my $md_IDs_text;

###################################################################################################
# Метод должен быть вызван перед вызовом любых других функций
# Параметры:
#   $dir - каталог с разобранным МДшником
sub Start($)
{
	$dir = shift;
	$AvailableID = GetAvailableID($dir);
	@IDs = LoadIDs($dir);
}

###################################################################################################
# Метод должен быть вызван после всех изменений в метаданных.
# При этом будут записаны изменения в служебных файлах.
sub Stop
{
	my $fname = "$dir\\$metadata_ids_file";
	open IDS, "> $fname" or die wintodos("Can not open file $fname: $!\n");
	$md_IDs_text =~ s/(Next ID\s*:\s*)(\d+)/$1$AvailableID/;
	print IDS $md_IDs_text;
	close IDS;

	$fname = "$dir\\$metadata_objs_file";
	open MD, "> $fname" or die wintodos("Can not open file $fname: $!\n");
	my $info;
	foreach $info (@IDs)
	{
		print MD $info->[0], "\t", $info->[1], "\n";
	}
	close MD;
}	

###################################################################################################
# Поиск объекта по числовому ID
# Параметры:
#   $id - числовой ID
# Возвращаемое значение: номер в массиве @IDs или -1
sub FindByID($)
{
	my $id = shift;
	my $id_info;
	my $i = 0;
	foreach $id_info (@IDs)
	{
		return $i if $id == $id_info->[0];
		$i++;
	}
	return -1;
}

###################################################################################################
# Поиск объекта по текстовому ID
# Параметры:
#   $sid - полный текстовый ID. Например Справочник.Контрагенты.ПолноеНаименование
# Возвращаемое значение: номер в массиве @IDs или -1
sub FindBySID($)
{
	my $sid = lc(shift);
	my $id_info;
	my $i = 0;
	foreach $id_info (@IDs)
	{
		return $i if $sid eq lc($id_info->[1]);
		$i++;
	}
	return -1;
}

sub FindByShortName($)
{
	my $name = lc(shift);
	my $pattern = "^\\s*\\S+\\." . $name . "\$";
	my $id_info;
	my $i = 0;
	foreach $id_info (@IDs)
	{
		#return $i if lc($id_info->[1]) =~ s/$pattern/;
		$i++;
	}
	return -1;
}

#Проверяет числовые идентификаторы на предмет уникальности и в случае необходимости уникализирует их.
# Возвращаемое значение: число изменённых идентификаторов
sub MakeIDsUnique()
{
	my $changed = 0;
	my @SortedID;
	foreach (@IDs) { push(@SortedID, [$_->[0], $_->[1]]);}

	@SortedID = sort {num_cmp($a->[0], $b->[0])} @SortedID;
	my $i;
	for( $i = 1; $i < @SortedID; $i++ )
	{
		if( $SortedID[$i-1]->[0] == $SortedID[$i]->[0] )
		{
			my $ID = $SortedID[$i]->[0];
			my $NewID = CalculateNewID();
			my $j = FindBySID($SortedID[$i]->[1]);
			$IDs[$j]->[0] = $NewID;
			print wintodos("$SortedID[$i]->[1]: $ID --> $NewID\n");

			$changed++;
		}
	}

	return $changed;
}

sub CheckTextIDs()
{
	my $errors = 0;
	my @SortedID;
	foreach (@IDs) { push(@SortedID, [$_->[0], $_->[1]]);}

	my $i;
	@SortedID = sort {$a->[0] cmp $b->[0]} @SortedID;
	for( $i = 1; $i < @SortedID; $i++ )
	{
		if( $SortedID[$i-1]->[1] eq $SortedID[$i]->[1] )
		{
			print wintodos("$SortedID[$i]->[1] не уникален\n");
			$errors++;
		}
	}

	return $errors;
}

sub SortObjectsByNumericIDs()
{
	@IDs = sort {num_cmp($a->[0], $b->[0])} @IDs;
}	

sub SortObjectsByTextualIDs()
{
	@IDs = sort {$a->[1] cmp $b->[1]} @IDs;
}	

sub AddRefField($$$) ## $FileName, $QualifiedFieldName, $FieldText
{
	return AddField(shift, "Атрибуты", shift, shift);
}
sub AddDocHeadField($$$)
{
	return AddField(shift, "Атрибуты шапки", shift, shift);
}
sub AddDocTableField($$$)
{
	return AddField(shift, "Атрибуты табличной части", shift, shift);
}
	  
####################################################################################
# Функция для добавления поля в объект метаданных.
# Добавляет поле в указанный файл, и обновляет служебные файлы. Числовой идентификатор генерируется автоматически.
# Параметры:
#   $FileName - имя mdp-файла
#   $Section - имя секции в метаданных, куда нужно добавлять поле. Для справочников это "Атрибуты", для документов "Атрибуты шапки" или "Атрибуты табличной части"
#   $QualifiedFieldName - полное имя поля. Например: Справочник.Контрагенты.Комментарий
#   $FieldText - текст с описанием поля, который будет добавлен в файл метаданных
# Возвращаемое значение: ID нового поля или 0
sub AddField($$$$)
{
	my $FileName = shift;
	my $Section = lc(shift);
	my $QualifiedFieldName = shift;
	my $FieldText = shift;
	
	return 0 if FindBySID($QualifiedFieldName) >= 0;

	open MD, "< $FileName" or die wintodos("Can not open file $FileName: $!\n");
	my ($line, $text, $block_name, $added) = ("", "", "", 0);
	my @blocks;
	foreach $line (<MD>)
	{
		$line =~ s/[\r\n]//g;

		if( $line =~ m/^\s*{\s*$/ )
		{
			push @blocks, $block_name;
		}
		elsif( $line =~ m/^\s*}\s*$/ )
		{
			if( @blocks[@blocks - 1] eq $Section )
			{
				$text .= "$FieldText\n";
				$added = 1;
			}
			pop @blocks;

			if( @blocks == 0 and $added == 0 ) #no section found - must create it
			{
				$text .= "\t$Section:\n\t{\n";
				$text .= "$FieldText\n";
				$text .= "\t}\n";
				$added = 1;
			}	
		}
		elsif( $line =~ m/^\s*(.+):\s*$/ )
		{
			$block_name = lc($1);
			$block_name =~ s/^(.*\S)\s*$/$1/g;
		}

		$text .= "$line\n";
	}	
	close MD;

	return 0 if $added == 0;

	open MD, "> $FileName" or die wintodos("Can not open file $FileName: $!\n");
	print MD $text;
	close MD;

	my $ID = $AvailableID;
	while( FindByID($ID) >= 0 ) {$ID++;}
	$AvailableID = $ID + 1;
	push @IDs, [$ID, $QualifiedFieldName];

	return $ID;
}

sub	SetFieldAttr($$$$)
{
	my $fname = shift;
	my $FieldName = lc(shift);
	my $AttrName = lc(shift);
	my $AttrNewVal = shift;

	open MD, "< $fname" or die wintodos("Can not open file $fname: $!\n");
	my ($line, $text, $block_name, $attr_exist, $changed) = ("", "", "", 0, 0);
	my @blocks;
	foreach $line (<MD>)
	{
		$line =~ s/[\r\n]//g;

		if( $line =~ m/^\s*{\s*$/ )
		{
			push @blocks, $block_name;
			$attr_exist = 0;
		}
		elsif( $line =~ m/^(\s*)}\s*$/ )
		{
			if( !$attr_exist and @blocks[@blocks - 1] eq $FieldName)
			{
				$text .= "$1\t$AttrName: $AttrNewVal\n";
				$changed = 1;
			}	
			pop @blocks;
		}
		elsif( $line =~ m/^\s*(.+):\s*$/ )
		{
			$block_name = lc($1);
			$block_name =~ s/^(.*\S)\s*$/$1/g;
		}
		
		#print wintodos("$line (@blocks[@blocks - 1],  $FieldName)\n");
		if( @blocks > 0 and @blocks[@blocks - 1] eq $FieldName )
		{
			$line =~ m/^(\s*([\w\d_ ]+?)\s*:\s*)(.*)$/;
			#print wintodos("\t" . lc($2) . ":$3 = $AttrName\n");
			$attr_exist = 1 if lc($2) eq $AttrName;
			if( lc($2) eq $AttrName and $3 ne $AttrNewVal )
			{
				#print wintodos("\t$FieldName\n");
				$line = "$1$AttrNewVal";
				$changed = 1;
			}	
		}

		$text .= "$line\n";
	}
	close MD;

	if( $changed == 1 )
	{
		open MD, "> $fname" or die wintodos("Can not open file $fname: $!\n");
		print MD $text;
		close MD;
	}

	return $changed;
}	  

####################################################################################
# Функция для удаления поля из объекта метаданных.
# Удаляет поле из указанного файла, и обновляет служебные файлы.
# Параметры:
#   $FileName - имя mdp-файла
#   $FieldName - имя поля.
# Возвращаемое значение: 1 - поле удалено, 0 - нет
sub DelField($$)
{
	my $FileName = shift;
	my $FieldName = lc(shift);
	
	open MD, "< $FileName" or die wintodos("Can not open file $FileName: $!\n");
	my $line;
	my @lines;
	my $block_name;
	my $block_start = -1;
	my $block_end = -1;
	my $state = 0; #0 - start, 1 - name found, 2 - in block, 3 - after the block
	foreach $line (<MD>)
	{
		$line =~ s/[\r\n]//g;
		push @lines, $line;

		if( $state == 0 )
		{
			if( $line =~ m/^\s*([^\s:]+):\s*$/ )
			{
				$block_name = lc($1);
				if( $block_name eq $FieldName )
				{
					$block_start = @lines - 1; 
					$state = 1;
				}
			}
		}
		elsif( $state == 1 )
		{
			if( $line =~ m/^\s*{\s*$/ )
			{
				$state = 2;
			}
			else
			{
				$state = 0;
			}
		}
		elsif( $state == 2 )
		{
			if( $line =~ m/^\s*}\s*$/ )
			{
				$block_end = @lines - 1;
				$state = 3;
			}
		}
	}	
	close MD;

	return 0 if $block_end < 0;

	splice(@lines, $block_start, $block_end - $block_start + 1);
	my $text = join("\n", @lines);
	open MD, "> $FileName" or die wintodos("Can not open file $FileName: $!\n");
	print MD $text;
	close MD;

	return 1;
}

sub CalculateNewID()
{
	my $ID = $AvailableID;
	while( FindByID($ID) >= 0 ) {$ID++;}
	$AvailableID = $ID + 1;
	return $ID;
}	

sub GetAvailableID($)
{
	my $dir = shift;
	my $fname = "$dir\\$metadata_ids_file";
	open IDS, "< $fname" or die wintodos("Can not open file $fname: $!\n");
	my $AvailableID = 1;
	$md_IDs_text = "";
	my $line;
	foreach $line (<IDS>)
	{
		$AvailableID = $1 if $line =~ m/Next ID\s*:\s*(\d+)/;
		$md_IDs_text .= $line;
	}
	close IDS;
	return $AvailableID;
}	

sub LoadIDs($)
{
	my $dir = shift;
	my $fname = "$dir\\$metadata_objs_file";
	my @IDs;
	open MD, "< $fname" or die wintodos("Can not open file $fname: $!\n");
	my $line;
	foreach $line (<MD>)
	{
		if( $line =~ m/^\s*(\d+)\s+(\S+)[\s\r\n]*$/ )
		{
			push(@IDs, [$1, $2]);
		}
	}	
	close MD;

	return @IDs;
}

sub num_cmp($$)
{
	my $left = shift;
	my $right = shift;
	if( $left < $right ) {return -1;}
	if( $left > $right ) {return 1;}
	return 0;
}

#Формирует имя объекты из имени каталога
sub GetObjectName($)
{
	my $dir = shift;
	my @parts = split /[\/\\]+/, $dir;
	my $i = @parts - 1;
	while( $parts[$i] =~ m/\./ )
	{
		$i--;
	}
	return $parts[$i];
}

sub wintodos {
	my $win_chars = "\xA8\xB8\xB9\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF";
	my $dos_chars = "\xF0\xF1\xFC\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF";
	$_ = shift;
	return $_ if $^O eq "cygwin";
	eval("tr/$win_chars/$dos_chars/");
	return $_;
}


1;
