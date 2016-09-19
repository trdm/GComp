#!/usr/bin/perl

#==============================================================================================================
# Скрипт для анализа информации, выдаваемой gcomp -d -v, и автоматического добавления/удалени
# файлов в/из репозитарий.
#==============================================================================================================
# Скрипт получает в stdin log от gcomp -v, и если там есть новые файлы (префикс 'NEW:'),
# то для них выполняется команда cvs add. Для удалённых файлов (префикс 'DEL:') выполняетс
# cvs remove.
# Информация о добавленных и удалённых файлах выводятся в stdout.
#
# Поскольку gcomp выдает log в OEM кодировке, то перед направлением лога в скрипт его нужно
# перекодировать. Например так:
# \>gcomp -d -v | perl c:\Script\auto_add.pl >>log
#
# Команда cvs commit скриптом не выполняется - её нужно выполнить вручную.
#==============================================================================================================

use Getopt::Long;

#Тип слэша в путях для cvs
$single_back = 1;  # '\'
$double_back = 2;  # '\\'
$single_right = 3; # '/'
$slash_type = $single_right;
$filter_expr = "";
$diff_updated = 0;
$diff_command = 'C:\Progra~1\KDiff3\kdiff3.exe'; #в пути не должны содержаться пробелы
#trying to get diff command from environment
$diff_command = $ENV{"DIFFCMD"}  if $ENV{"DIFFCMD"};
$tmp_dir = "c:\\tmp";
#trying to get temp dir from environment
$tmp_dir = $ENV{"TMP"}  if $ENV{"TMP"};
$tmp_dir = $ENV{"TEMP"} if $ENV{"TEMP"};


$usage = <<EOF
Использование:
   perl auto_add.pl [параметры]
Параметры могут быть:
   -h|--help - этот текст
   --filter|-f <RegExp> - регулярное выражение. Если указано, то будут обрабатываться только файлы, соответствующие этому выражению.
   --diff|-d - если указан, то для каждого изменённого файла будет выдаваться diff.
EOF
;

die wintodos($usage) unless GetOptions("filter|f=s" => \$filter_expr, "diff|d" => \$diff_updated);
print "Filter expression: $filter_expr\n" if $filter_expr;
#print "diff = $diff_updated\n";
$filter_expr = wintodos($filter_expr);
if( $diff_updated )
{
	-d $tmp_dir or die "Temp directory '$tmp_dir' not exist\n";
	#-X $diff_command or die "Diff command '$diff_command' not found\n";
}


$session_splitter = "-------------------------------------------------------------------------------------------------\n";
foreach $line (<STDIN>)
{
	($cvs_cmd, $info, $fname) = ParseLine();
	if( $cvs_cmd )
	{
		if( $filter_expr and $fname =~ m/$filter_expr/i )
		{
			print "File '$fname' skipped by filter\n";
			next;
		}
		
		($work_dir, $path) = SplitPath($fname);

		if( $info eq "UPD" )
		{
			print STDERR $line, "\n";

			#skip binary files
			$path =~ m/^.*\.(1s|txt|mdp|ord|frm)$/i or next;

			if( $diff_updated )
			{
				my $tmp_file = $path;
				$tmp_file =~ s/(\\|\\\\|\/)/_/g;
				$tmp_file = "$tmp_dir\\$tmp_file";
				chdir $work_dir;
				DoCommand("cvs -q update -p \"$path\" > \"$tmp_file\"", 1);
				system("start", "$diff_command \"$tmp_file\" \"$path\"");
				chdir "..";
			}
			next;
		}

		SessionStart();

		chdir $work_dir;
		AddParentDirs("$path") if $cvs_cmd eq "add";
		print wintodos("$info File: $path\n");
		print STDERR wintodos("$info File: $path\n");
		DoCommand("cvs $cvs_cmd \"$path\"");
		chdir "..";
	}
	else
	{
		print STDERR $line, "\n";
	}
}
SessionEnd();


sub ParseLine
{
	$line =~ s/[\r\n]//g; #уберём завершающие \n и \r, если они там есть

	if( $line =~ m/^NEW:\s*'*(.+)'*\s*$/ )
	{
		my $fname = $1;
		my ($work_dir, $path) = SplitPath($fname);
		return ("add" . KOpt($fname), "ADD", $fname);
	}
	elsif( $line =~ m/^DEL:\s*'*(.+)'*\s*$/ )
	{
		return ("remove", "DEL", "$1");
	}
	elsif( $line =~ m/^UPD:\s*'*(.+)'*\s*$/ )
	{
		return ("UPD", "UPD", "$1");
	}
	return ("", "", "");
}

sub SplitPath
{
	my $path = shift;
	$path =~ m/^([^\\]*)\\+(.*)$/;
	return (dostowin($1), dostowin($2));
}

sub AddParentDirs
{
	my $dir = shift;
	my $i = 0;
	my @dir_list;

	while( $dir =~ s/(.*)\\([^\\]*)/$1/ )
	{
		$dir_list[$i++] = "$dir" unless -d "$dir\\CVS" or -d "$dir\\cvs";
	}

	while( --$i >= 0 )
	{
		$dir = $dir_list[$i];
		print wintodos("ADD Dir: $dir\n");
		print STDERR wintodos("ADD Dir: $dir\n");
		DoCommand("cvs add \"$dir\"");
	}
}

sub PreparePath
{
	my $path = shift;
	if( $slash_type == $double_back )
	{
		$path =~ s/\\/\\\\/g; #заменяем одиночный слэш на двойной
	}
	elsif( $slash_type == $single_right )
	{
		$path =~ s/\\/\//g; #заменяем обратный слэш на прямой
	}
	return $path;
}

sub DoCommand
{
	my $cmd = shift;
	my $no_log = shift;
	$cmd = PreparePath($cmd);

	if( $no_log )
	{
		system($cmd);
	}
	else
	{
		print wintodos("\\$work_dir>$cmd\n");
		my $line;
		open(CMD, "$cmd 2>&1 |");
		foreach $line (<CMD>)
		{
			print "\t", wintodos($line); #всё, что выдаёт cvs, будем выводить с отступом
		}
		close CMD;
	}
}

sub SessionStart
{
	if( not $time_printed )
	{
		($sec, $min, $hour, $mday, $mon, $year) = localtime();
		$year += 1900;
		$mon++;
		printf "%04i.%02i.%02i %02i:%02i:%02i\n", $year, $mon, $mday, $hour, $min, $sec;
		print $session_splitter;
		$time_printed = 1;
	}
}

sub SessionEnd
{
	print $session_splitter, "\n" if $time_printed;
}

sub dostowin
{
	my $win_chars = "\xA8\xB8\xB9\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF";
	my $dos_chars = "\xF0\xF1\xFC\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF";
	$_ = shift;
	eval("tr/$dos_chars/$win_chars/");
	return $_;
}

sub KOpt($)
{
	my $name = shift;
	if( $name =~ m/^(.*?)\.(mxl|ico|bmp|gif|wmf|jpg)$/i )
	{
		return " -kb";
	}
	if( $name =~ m/^(.*?)[\\\/]?(Gallery\.head|Gallery\.tail|GUIDData|TagStream)$/i )
	{
		return " -kb";
	}
	
	return "";
}

sub wintodos {
	my $win_chars = "\xA8\xB8\xB9\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF";
	my $dos_chars = "\xF0\xF1\xFC\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF";
	$_ = shift;
	return $_ if $^O eq "cygwin";
	eval("tr/$win_chars/$dos_chars/");
	return $_;
}
