
#==============================================================================================================
# Скрипт для конвертации репозитариев из VSS в CVS
#==============================================================================================================

#при необходимости можно установить здесь CVSROOT, если для конвертации используется не то, что по умолчанию (в соотв. переменой среды)
#$ENV{"CVSROOT"} = ":pserver:adirks@adirks:/1C";
#$ENV{"CVSROOT"} = "S:/CVS/CVS_Repository";
$SaveDateTime = 0;
#$auto_add_script = "C:\\Script\\auto_add.pl";
$auto_add_script = "D:\\gcomp\\scripts\\auto_add\\auto_add.pl";
$correct_dlg_script = "D:\\gcomp\\scripts\\correct_dlg\\correct_dlg.pl";

$ss_project = "$ARGV[0]"; #путь к файлу 1cv7.md в VSS
$module = "$ARGV[1]";     #имя модуля в CVS

$usage = "Usage:\n\tperl 1c_vss2cvs.pl <VSS project name> <CVS module name> [StartVersion [StopVersion]]";

$win_chars = "\xA8\xB8\xB9\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF";
$dos_chars = "\xF0\xF1\xFC\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF";

die "VSS project name missing\n$usage\n" if not $ss_project;
die "CVS module name missing\n$usage\n"  if not $module;

$start_version = $ARGV[2];
$stop_version = $ARGV[3];
$start_version = 1 if $start_version <= 0;
print wintodos("Try to convert VSS project \"$ss_project\" to \"$module\"\n");
print "Start version: $start_version, Stop version: $stop_version\n";

$SIG{'INT'}  = \&INT_handler;
ReadHistory();
while( $i > 0 )
{
	$version = $hist[$i][0];
	die "User break at version $version\n" if $stop_work;

	if( $version >= $start_version )
	{
		print STDERR "Converting v.$version\n";
		print "\n\n===========================================================================\n";
		print( wintodos("$hist[$i][0], $hist[$i][1], $hist[$i][2], $hist[$i][3], $hist[$i][4], $hist[$i][5]\n") );
		AddToModule();
		open LAST_VER, ">> last-ver";
		print LAST_VER "v.$version converted\n";
		close LAST_VER;
	}
	die if $stop_version > 0 and $version >= $stop_version;
	$i--;
}

sub AddToModule
{
	my $version = $hist[$i][0];
	my $user    = $hist[$i][1];
	my $date    = $hist[$i][2];
	my $time    = $hist[$i][3];
	my $label   = $hist[$i][4];
	my $comment = $hist[$i][5];

	open COMMENT, "> comment";
	print COMMENT "$date $time " unless $SaveDateTime;
	print COMMENT "$user\n$comment";
	close COMMENT;

	system "ss get -V$version -W \"$ss_project\"";
	if( $version == 1 )
	{
		ImportModule();
		return;
	}

	SetDateTime($date, $time) if $SaveDateTime;
	system "gcomp -d -v -D $module --sort-rights | perl $auto_add_script";
	system "perl \"$correct_dlg_script\" -p25 25 -d \"$module\"" if $correct_dlg_script;
	
	chdir $module;
	SetDateTime($date, $time) if $SaveDateTime;
	system "cvs commit -F ../comment 2>&1";
	SetDateTime($date, $time) if $SaveDateTime;
	DoCommand("cvs tag $label 2>&1", "..\\tags.log") if $label;
	$ver = sprintf "ver_%03i", $version;
	DoCommand("cvs tag $ver 2>&1", "..\\tags.log");
	chdir "..";
}

sub ImportModule
{
	SetDateTime($date, $time) if $SaveDateTime;
	system "gcomp -d -D $module --sort-rights";
	system "perl \"$correct_dlg_script\" -p25 25 -d \"$module\"" if $correct_dlg_script;
	chdir $module;
	SetDateTime($date, $time) if $SaveDateTime;
	system "cvs import -C -n -m \"\" $module 2>&1";
	SetDateTime($date, $time) if $SaveDateTime;
	DoCommand("cvs tag ver_001 $module 2>&1", "..\\tags.log");
	chdir "..";
}

sub DoCommand
{
	my $cmd = shift;
	my $log_name = shift;
	my $line;

	open(LOG, ">> $log_name");
	open(CMD, "$cmd 2>&1 |");
	foreach $line (<CMD>)
	{
		print LOG $line;
	}
	close CMD;
	close LOG;
}

sub SetDateTime
{
	local $date = shift;
	local $time = shift;
	print("Set date to $date $time");

	$date =~ m/(\d+)\.(\d+)\.(\d+)/;
	local ($DD, $MM, $YYYY) = ($1, $2, $3);
	$time =~ m/(\d+):(\d+)/;
	local ($hh, $minute) = ($1, $2);

	local $timestamp = sprintf("%02i%02i%02i%02i%04i", $MM, $DD, $hh, $minute, $YYYY);
	if( $SaveDateTime )
	{
		system("cmd /C date $DD.$MM.$YYYY");
		system("cmd /C time $hh.$minute");
	}
	return $timestamp;
}


sub ReadHistory
{
	open(HIST, "ss history \"$ss_project\" |") or die "Can't open history\n";
	foreach $histline (<HIST>)
	{
		$histline =~ s/[\n\r]//g;
		$histline = dostowin($histline);

		if( $histline =~ m/^[*]+\s+Version\s+(\d+).*$/ )
		{
			if( $i > 0 )
			{
				$comment = "$1" if $comment =~ m/^(.*)[\r\n]*$/; #remove trailing \r\n from comment
				AddHist($version, $user, $date, $time, $label, $comment);
			}
			$version = "$1";
			$user = "";
			$date = "";
			$time = "";
			$label = "";
			$comment = "";
			$i++;
		}
		#User: Adirks       Date:  7.04.04   Time: 15:48
		elsif( $histline =~ m/^User:\s*(\S+)\s*Date:\s*(\d+).(\d+).(\d+)\s*Time:\s*(\d+):(\d+)\s*$/ )
		{
			$user = "$1";
			$date = sprintf "%02i.%02i.%04i", $2, $3, $4+2000;
			$time = sprintf "%02i:%02i", $5, $6;
		}
		elsif( $histline =~ m/^Label:\s*"(\S+)".*$/ )
		{
			$label = "$1";
        	$label =~ s/[^\w\- \t]+/_/g;
			$label = "L$1" if $label =~ m/^(\d+.*)$/;
		}
		elsif( $histline =~ m/^Comment:\s(.*)$/ )
		{
			$comment = "$1";
		}
		elsif( $histline =~ m/^(.*)$/ )
		{
			$comment = "$comment\n$1";
		}
	}
	AddHist($version, $user, $date, $time, $label, $comment);

	close HIST;
}

sub	AddHist
{
	$hist[$i][0] = shift;
	$hist[$i][1] = shift;
	$hist[$i][2] = shift;
	$hist[$i][3] = shift;
	$hist[$i][4] = shift;
	$hist[$i][5] = shift;
}

sub INT_handler
{
	$stop_work = 1;
	print "break signaled....\n";
}

sub dostowin {
  $_ = shift;
  eval("tr/$dos_chars/$win_chars/");
  return $_;
}

sub wintodos {
  $_ = shift;
  my $printing = shift;
  return $_ if $printing and $^O eq "cygwin";
  eval("tr/$win_chars/$dos_chars/");
  return $_;
}