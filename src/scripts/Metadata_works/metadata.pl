
# 
# Скрипт, демонстрирующий работу с MetadataWorks.pm, и по совместительству болванка для реально работающего скрипта
# 

use locale;
use Getopt::Long;
use File::Find; #package для рекурсивного обхода каталогов
use MetadataWorks;

$usage = <<EOF
Использование:
   perl Metadata.pl [параметры]
Параметры могут быть:
   -d <dir>  - каталог с разобранной конфигурацией. По умолчанию - текущий каталог.
EOF
;

#default values
$root_dir = ".";
GetOptions("dir|d=s" => \$root_dir) or die Metadata::wintodos($usage);

$FldText = <<EOF
		GUID: 
		{
			Комментарий: ""
			Синоним: ""
			Для элемента: Да
			Для группы: Да
			Тип: Строка
			Длина: 33
			Точность: 0
			Агрегатный тип: 
			Неотрицательный: Нет
			Разделять триады: Нет
			Периодический: Нет
			Изменение истории документами: Нет
			Ручное изменение истории: Да
			Сортировка: Да
			Отбор: Да
		}
EOF
;


$TableFldText = <<EOF
		Сумма: 
		{
			Комментарий: ""
			Синоним: ""
			Тип: Число
			Длина: 19
			Точность: 2
			Агрегатный тип: 
			Неотрицательный: Нет
			Разделять триады: Да
			Итог по колонке: Да
		}
EOF
;

AddRefFields(); #Добавляем поле во все справочники
ChangeRefFields(); #Меняем атрибуты поля во всех справочниках
AddDocHeadFields(); #Добавляем поле в шапки документов
AddDocTableFields(); #Добавляем поле в ТЧ документов
ChangeDocTableFields(); # Меняем атрибуты полей табличной части документов




###################################################################################################################
sub AddRefFields
{
	Metadata::Start($root_dir);
	File::Find::find(\&_AddRefField, $root_dir);
	Metadata::Stop();
}

sub _AddRefField
{
	my $dir = $File::Find::dir;
	my $fname = $_;
	return unless $fname =~ m/^(.*)(\.mdp)$/i; #нам нужны только файлы с расширением mdp
	return unless $dir =~ m/[\/\\]Справочники[\/\\]/i;

	my $obj_name = Metadata::GetObjectName($dir);
	if( Metadata::AddRefField($fname, "Справочник.$obj_name.GUID", $FldText) > 0 )
	{
		print Metadata::wintodos("Add GUID to $obj_name\n");
	}	
}	

#------------------------------------------------------------------------------------------------------
sub ChangeRefFields
{
	Metadata::Start($root_dir);
	File::Find::find(\&_ChangeRefField, $root_dir);
	Metadata::Stop();
}

sub _ChangeRefField
{
	my $dir = $File::Find::dir;
	my $fname = $_;
	return unless $fname =~ m/^(.*)(\.mdp)$/i; #нам нужны только файлы с расширением mdp
	return unless $dir =~ m/[\/\\]Справочники[\/\\]/i;

	if( Metadata::SetFieldAttr($fname, "GUID", "Длина", "40") > 0 )
	{
		my $obj_name = Metadata::GetObjectName($dir);
		print Metadata::wintodos("$obj_name Changed\n");
	}	
}	

#------------------------------------------------------------------------------------------------------
sub AddDocHeadFields
{
	Metadata::Start($root_dir);
	File::Find::find(\&_AddDocHeadField, $root_dir);
	Metadata::Stop();
}	

sub _AddDocHeadField
{
	my $dir = $File::Find::dir;
	my $fname = $_;
	return unless $fname =~ m/^(.*)(\.mdp)$/i; #нам нужны только файлы с расширением mdp
	return unless $dir =~ m/[\/\\]Документы[\/\\]/i;

	my $obj_name = Metadata::GetObjectName($dir);
	if( Metadata::AddDocHeadField($fname, "Документ.$obj_name.GUID", $FldText) > 0 )
	{
		print Metadata::wintodos("Add GUID to head $obj_name\n");
	}	
}

#------------------------------------------------------------------------------------------------------
sub AddDocTableFields
{
	Metadata::Start($root_dir);
	File::Find::find(\&_AddDocTableField, $root_dir);
	Metadata::Stop();
}	

sub _AddDocTableField
{
	my $dir = $File::Find::dir;
	my $fname = $_;
	return unless $fname =~ m/^(.*)(\.mdp)$/i; #нам нужны только файлы с расширением mdp
	return unless $dir =~ m/[\/\\]Документы[\/\\]/i;

	my $obj_name = Metadata::GetObjectName($dir);
	if( Metadata::AddDocTableField($fname, "Документ.$obj_name.Сумма", $TableFldText) > 0 )
	{
		print Metadata::wintodos("Add Сумма to table $obj_name\n");
	}	
}
#------------------------------------------------------------------------------------------------------


#------------------------------------------------------------------------------------------------------
sub ChangeDocTableFields
{
	Metadata::Start($root_dir);
	File::Find::find(\&_ChangeDocTableFields, $root_dir);
	Metadata::Stop();
}

sub _ChangeDocTableFields
{
	my $dir = $File::Find::dir;
	my $fname = $_;
	return unless $fname =~ m/^(.*)(\.mdp)$/i; #нам нужны только файлы с расширением mdp
	return unless $dir =~ m/[\/\\]Документы[\/\\]/i;

	if( Metadata::SetFieldAttr($fname, "Сумма", "Итог по колонке", "Да") > 0 )
	{
		my $obj_name = Metadata::GetObjectName($dir);
		print Metadata::wintodos("$obj_name Changed\n");
	}	
}	
#------------------------------------------------------------------------------------------------------


