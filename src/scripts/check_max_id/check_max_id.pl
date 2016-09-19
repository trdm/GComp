#!/usr/bin/perl

#==============================================================================================================
# Скрипт для проверки максимального идентификатора метаданных на предмет попадания в заданный диапазон
#==============================================================================================================

use Getopt::Long;


#default values
$working_dir = "SRC";
$id_settings_file_name = "..\\id_settings.txt";
$metadata_ids_file = "ИдентификаторыКонфигурации.mdp";
$warning_threshold = 100;

$usage = <<EOF
Использование:
   perl check_max_id.pl [параметры]
Параметры могут быть:
   -h|--help - этот текст
   -d|-D|--dir <dir> - рабочий каталог. По умолчанию $working_dir.
   -t|-w <n> - пороговое значение для предупреждений. По умолчанию $warning_threshold.
   -f|--file <file> - имя файла, в котором хранятся диапазоны идентификаторов. Задаётся относительно рабочего каталога. По умолчанию $id_settings_file_name
   -u|--user <user> - имя пользователя в CVS. По умолчанию берётся из файла <working dir>\\CVS\\Root.
EOF
;


$res = GetOptions("help|h" => \$help,
	"dir|d|D=s" => \$working_dir, 
	"t|w=i" => \$warning_threshold,
	"file|f=s" => \$id_settings_file_name,
	"user|u=s" => \$user_name);
if( not $res or $help )
{
	print wintodos($usage);
	exit 1;
}	

GetLoginName() if not defined $user_name;
FindCurrentRange();
if( not defined $max_id )
{
	print "Can not determine metadata ID range for user $user_name\n";
	exit 1;
}
GetMaxMetadataID();
if( not defined $max_metadata_id )
{
	print "Can not determine max metadata ID. May be file format had changed?\n";
	exit 1;
}
print "$user_name: $min_id, $max_id\n";

if( $max_metadata_id < $min_id )
{
	print "ERROR: Current metadata ID is out of range! Commit impossible!\n";
	print "\tID = $max_metadata_id; min ID = $min_id\n";
	exit 1;
}
if( $max_metadata_id >= $max_id )
{
	print "ERROR: Current metadata ID is out of range! Commit impossible!\n";
	print "\tID = $max_metadata_id; max ID = $max_id\n";
	exit 1;
}
if( ($max_metadata_id + $warning_threshold) >= $max_id )
{
	print "WARNING: Current metadata ID is near the top! You need to get new ID's range from project administrator!\n";
	print "\tID = $max_metadata_id; max ID = $max_id\n";
}


sub GetMaxMetadataID
{
	if( !open IDS, "< $working_dir\\$metadata_ids_file" )
	{
		$full_name = wintodos("$working_dir\\$metadata_ids_file");
		print "Can not open file $full_name: $!\n";
		exit 1;
	}

	foreach $line (<IDS>)
	{
		#{"MainDataContDef","39494","10009","7120"}
		$max_metadata_id = $1 if $line =~ m/^\s*Next ID\s*:\s*(\d+)[\s\r\n]*$/;
	}
	close IDS;
}

#находим диапазон идентификаторов из файла ограничений
#Формат строки:
# <user_name> <any spaces> <min ID> <any spaces> <max ID>
# строк м.б. несколько на одного юзера - берётся всегда самая последн
sub FindCurrentRange
{
	if( !open ID_SETTINGS, "< $working_dir\\$id_settings_file_name" )
	{
		$full_name = wintodos("$working_dir\\$id_settings_file_name");
		print "Can not open file $full_name: $!\n";
		return;
	}

	foreach $line (<ID_SETTINGS>)
	{
		next if $line =~ m/^\s*([;#])|(\/\/).*$/; #skip comments
		if( $line =~ m/^\s*(\S+)\s+(\S+)\s+(\S+)\s*$/ and $1 eq $user_name )
		{
			$min_id = $2;
			$max_id = $3;
		}
	}
	close ID_SETTINGS;
}

sub GetLoginName
{
	$user_name = getlogin();
	die "Can not open CVS\\Root file: $!\n" unless open CVS_ROOT, "< $working_dir\\CVS\\Root";
	foreach $line (<CVS_ROOT>)
	{
		#:protocol:user@host:cvs_repository_path_on_host
		$user_name = $1 if $line =~ m/.*:(\S+)@(\S+):.*/;
	}
	close CVS_ROOT;
}

sub wintodos {
  my $pvdcoderdos = shift;
  $pvdcoderdos =~ tr/\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF/\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF/;
  return $pvdcoderdos;
}
