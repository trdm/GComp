use locale;
use File::Find; #package для рекурсивного обхода каталогов
use Getopt::Long;

$usage = <<EOF
Использование:
   perl code_works.pl [параметры]
Параметры могут быть:
   -h        - этот текст
   -d <dir>  - каталог, откуда начнётся поиск модулей (*.mdp). По умолчанию поиск начинается с
               текущего каталога.
EOF
;

$root_dir = ".";
GetOptions(
	"dir|d=s" => \$root_dir,
	) 
	or die wintodos($usage);


$migration_rules = <<EOF
	Параметры миграции: 
	{
		Область распространения: Место создания
	}
EOF
;

#traverse through dirs and do work
File::Find::find(\&ProcessMetadata, $root_dir);

sub ProcessMetadata
{
	my $dir = $File::Find::dir;
	my $fname = $_; #File::Find складывает в $_ имя текущего файла
	return unless $fname =~ m/^(.*)(\.mdp)$/i; #нам нужны только файлы с расширением mdp

	my $line;
	my $new_text = "";
	my $level = 0;
	my $already_exists = 0;
	my $need_write = 0;

	open MDP, "<$fname";
	foreach $line (<MDP>)
	{
		$line =~ s/[\r\n]//g; #уберём завершающие \n и \r, если они там есть
		if( $line =~ m/^\s*{\s*$/ )
		{
			$level++;
		}
		elsif( $line =~ m/^\s*}\s*$/ )
		{
			$level--;
			if( $level == 0 and $already_exists == 0 )
			{
				$new_text .= $migration_rules;
				$need_write = 1;
			}	
		}
		elsif( $line =~ m/^\s*Параметры миграции:\s*$/ )
		{
			$already_exists = 1;
		}	
		$new_text .= "$line\n";
	}
	close MDP;

	if( $need_write > 0 )
	{
		open MDP, ">$fname";
		print MDP $new_text;
		close MDP;
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