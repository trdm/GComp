
my $src_dir = $ARGV[0];
$src_dir or die wintodos("Не указан каталог с разобранной конфигурацией\n");

my @src;
my @new;
my @dest;

chdir $src_dir or die "$!\n";
open REPO_CONTENT, "cvs -q -z9 update -p -r HEAD -- ОбъектыМетаданных.txt |" or die "$!\n";
@src = LoadIDs(<REPO_CONTENT>);
close REPO_CONTENT;

open OBJECTS, "< ОбъектыМетаданных.txt" or die "$!\n";
@new = LoadIDs(<OBJECTS>);
close OBJECTS;

my $MaxID = GetMaxID(@src);
my $NeedCompile;
foreach (@new)
{
	my $id = $_->[0];
	my $sid = $_->[1];
	if( Find($id, $sid) == 1 )
	{
		my $new_id = ++$MaxID;
		print wintodos("Not unique $id, $sid --> $new_id\n");
		$id = $new_id;
		$NeedCompile = 1;
	}
	push @dest, [$id, $sid];
}

if( $NeedCompile )
{
	open DEST, "> ОбъектыМетаданных.txt" or die "$!\n";
	foreach (@dest)
	{
		printf DEST "%10d\t%s\n", $_->[0], $_->[1];
	}
	close DEST;

	print wintodos("\nВНИМАНИЕ!  Файл ОбъектыМетаданных.txt был изменён!\nТребуется повторная компиляция!\n");
	exit(1); #inform caller that changes where made
}


########################################################################
sub Find($$)
{
	my $id = shift;
	my $sid = shift;
	#print "Check for $id, $sid\n";
	my $pair;
	foreach $pair (@src)
	{
		#print "\t$pair->[0], $pair->[1]\n";
		return 1 if ($pair->[0] == $id) and ($pair->[1] ne $sid);
	}
	return 0;
}

sub GetMaxID(@)
{
	my @ids = @_;
	my $MaxID = -1;
	my $pair;
	foreach $pair (@ids)
	{
		my $id = $pair->[0];
		$MaxID = $id if $id > $MaxID;
	}
	return $MaxID;
}

sub LoadIDs(@)
{
	my @lines = @_;
	my @IDs;
	my $line;
	foreach $line (@lines)
	{
		if( $line =~ m/^\s*(\d+)\s+(\S+)[\s\r\n]*$/ )
		{
			#print "$1  $2\n";
			push(@IDs, [$1, $2]);
		}
	}	

	return @IDs;
}


sub num_cmp($$)
{
	if( @_[0] < @_[1] ) {return -1;}
	if( @_[0] > @_[1] ) {return 1;}
	return 0;
}

sub wintodos {
	my $win_chars = "\xA8\xB8\xB9\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF";
	my $dos_chars = "\xF0\xF1\xFC\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF";
	$_ = shift;
	return $_ if $^O eq "cygwin";
	eval("tr/$win_chars/$dos_chars/");
	return $_;
}
