
use locale;
use Getopt::Long;
use MetadataWorks;

$usage = <<EOF
Использование:
   perl unique_ids.pl [параметры]
Параметры могут быть:
   -d <dir>  - каталог с разобранной конфигурацией. По умолчанию - текущий каталог.
EOF
;

#default values
$root_dir = ".";
GetOptions("dir|d=s" => \$root_dir) or die Metadata::wintodos($usage);

Metadata::Start($root_dir);

if( Metadata::MakeIDsUnique() > 0 )
{
	Metadata::Stop();
}
	  
exit(Metadata::CheckTextIDs());
