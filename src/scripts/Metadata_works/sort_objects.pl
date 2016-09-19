
use locale;
use Getopt::Long;
use MetadataWorks;

$usage = <<EOF
Использование:
   perl sort_objects.pl [параметры]
Параметры могут быть:
   -d <dir>  - каталог с разобранной конфигурацией. По умолчанию - текущий каталог.
   -n - сортировка по числовым идентификаторам
   -t - сортировка по текстовым идентификаторам (по умолчанию)
EOF
;

#default values
my $root_dir = ".";
my $numeric = 0;
my $textual = 0;
GetOptions("dir|d=s" => \$root_dir, "numeric|n" => \$numeric, "textual|text|t" => \$textual ) or die Metadata::wintodos($usage);

Metadata::Start($root_dir);

if( $textual )
{
	Metadata::SortObjectsByTextualIDs();
}
elsif( $numeric )
{
	Metadata::SortObjectsByNumericIDs();
}
else
{
	die Metadata::wintodos("Не указан способ сортировки (опции -n или -t)\n");
}

Metadata::Stop();
	  

