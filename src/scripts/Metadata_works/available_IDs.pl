
my $FName = $ARGV[0];

open MD, "< $FName" or die "Can not open file '$FName'";

my @IDs;
my $MaxID = 0;

foreach( <MD> )
{
	m/^\s*(\d+)\s+.*$/;
	my $ID = $1;
	$MaxID = $ID if $ID > $MaxID;
	push @IDs, $ID;
}

@IDs = sort {num_cmp($a, $b)} @IDs;
my $ID;
my $PrevID = 0;
foreach $ID (@IDs)
{
	#print "\t$PrevID, $ID\n";
	PrintDiapazon($PrevID, $ID) if ($PrevID+1) < $ID;
	$PrevID = $ID;
}

#print "\n========================\n";
#foreach(@IDs)
#{
#	print "$_\n";
#}


sub PrintDiapazon($$)
{
	my $i = shift;
	my $i2 = shift;
	while( ++$i < $i2 )
	{
		print "$i\n";
	}
}

sub num_cmp($$)
{
	if( @_[0] < @_[1] ) {return -1;}
	if( @_[0] > @_[1] ) {return 1;}
	return 0;
}
