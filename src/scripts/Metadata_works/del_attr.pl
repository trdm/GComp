
use locale;
use Getopt::Long;
use File::Find; #package для рекурсивного обхода каталогов
use MetadataWorks;

my $usage = <<EOF
Использование:
   perl del_attr.pl [параметры]
Параметры могут быть:
   -d <dir>  - каталог с разобранной конфигурацией. По умолчанию - текущий каталог.
   --fld <field name> - имя поля, которое надо удалить.
EOF
;

#default values
my $root_dir = ".";
my $fld_name = "";
GetOptions("dir|d=s" => \$root_dir, "fld=s" => \$fld_name) or die wintodos($usage);

$fld_name or die "Не указано имя поля";

File::Find::find(\&DelField, $root_dir);


###################################################################################################################
sub DelField
{
	my $dir = $File::Find::dir;
	my $fname = $_;
	return unless $fname =~ m/^(.*)(\.mdp)$/i; #нам нужны только файлы с расширением mdp

	my $obj_name = GetObjectName($dir);
	my $ID = Metadata::DelField($fname, $fld_name);
	if( $ID > 0 )
	{
		print wintodos("$dir/$fname: Deleted $fld_name\n");
	}	
}	


sub GetObjectName
{
	my $dir = shift;
	my @parts = split /[\/\\]+/, $dir;
	my $i = @parts - 1;
	while( @parts[$i] =~ m/\./ )
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
