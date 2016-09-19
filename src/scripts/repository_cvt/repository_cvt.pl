
#==============================================================================================================
# Скрипт для конвертации репозитариев, созданных при помощи старых версий gcomp
#==============================================================================================================

use File::Find;
use File::Copy;
use File::Path;

#$SaveDateTime = 1;

#при необходимости можно установить здесь CVSROOT, если для конвертации используется не то, что по умолчанию (в соотв. переменой среды)
#$ENV{"CVSROOT"} = ":pserver:adirks@adirks:/CVS_Repository";
#$ENV{"CVSROOT"} = "E:/1C_repository";
# 
$auto_add_script = "c:\\script\\auto_add.pl";

$repostory_new_dir = "H:\\CVS\\Repositories\\1C\\Empty_new";
$top_dir = "D:\\1C\\tmp2\\Empty";
$working_dir = "SRC";
$new_working_dir = "SRC_new";
$module = "Empty";
$new_module = "Empty_new";
#$repostory_new_dir = "C:\\Shared\\CVS\\CVS_Repository\\Marta-M-New";
#$top_dir = "E:\\VSS\\Marta-M";
#$working_dir = "SRC";
#$new_working_dir = "SRC_new";
#$module = "Marta-M";
#$new_module = "Marta-M-New";

#Стартовое состояние очереди версий gcomp. Старые версии в процессе работы постепенно удаляются.
#@gcomp_versions = ("-1.0", "-2.0.4", "-2.0.5", "-2.0.6", "-2.0.7", "-2.0.8", "-2.0.9", "-2.0.10", "-2.0.11", "");
@gcomp_versions = ("-2.0.4", "-2.0.5", "-2.0.6", "-2.0.7", "-2.0.8", "-2.0.9", "-2.0.10", "-2.0.11", "");
#$gcomp_decompile_opts = "--dialog-symtypes";


$only_list = 1 if $ARGV[0] eq "-l";

RemoveDir($repostory_new_dir) if -d $repostory_new_dir;
RemoveDir("$top_dir\\$working_dir") if -d "$top_dir\\$working_dir";
RemoveDir("$top_dir\\$new_working_dir") if -d "$top_dir\\$new_working_dir";
BuildCommitLog();
DeleteAllFiles();

#-------------------------------------------------------------------------------------------------------------
$size = @commits;
$i = 0;
$commit_count = 0;
while( $i < $size )
{
	if( $only_list )
	{
		print $commits[$i], "\n";
		$i++;
		next;
	}

	#Разберём на составляющие ту страшную строку, которую сконструировали при анализе лога
	($date_time, $commit_id, $tag, $comment, $working_file, $date_time, $author, $state, $revision) = split(/\001/, $commits[$i]);
	if( (DateDiff($prev_date_time, $date_time) > 1) and (($prev_id ne $commit_id) or ($prev_comment ne $comment) or ($prev_author ne $author)) )
	{
		DoConversionStep() if $i > 0;
		print "==================================================================================\n";
		$last_commit_tag = "";
	}
	($prev_date_time, $prev_id, $prev_comment, $prev_author) = ($date_time, $commit_id, $comment, $author);

	print $commit_id, ": ", $working_file, ": ", $date_time, ": ", $author, ": ", $revision, ":", $tag, "\n";

	chdir "$top_dir\\$working_dir";
	$last_commit_tag = $tag if $tag;
	$last_commit_date_time = "$date_time";
	$i++;
}
DoConversionStep() unless $only_list;
#-------------------------------------------------------------------------------------------------------------

sub DoConversionStep
{
	my $date = $prev_date_time;
	$date =~ s/[^\d]/\./g; #заменяем все разделители на '.': хотя в доке написано, что дату можно как угодно писать, но видимо CVSNT об этом ничего не читал
	chdir "$top_dir\\$working_dir";
	CvsCommand("cvs update -d -P -D $date");

	chdir "$top_dir";

	if( $AppendToComment )
	{
		#Предыдущая попытка декомпиляции закончилась неудачно, коммита не было - комментарий нужно сохранить
		open COMMENT, ">> comment";
		print COMMENT "\n" if $AppendToComment
	}
	else
	{
		open COMMENT, "> comment";
	}

	print COMMENT "$prev_date_time " unless $SaveDateTime;
	print COMMENT "$prev_author: $prev_comment";
	close COMMENT;

	return if not TryToCompile();

	SetDateTime($prev_date_time) if $SaveDateTime;
	if( $commit_count == 0 )
	{
		CvsCommand("gcomp -d -F 1cv7.new.md -D \"$new_working_dir\"");
		DoImport();
		chdir "$top_dir\\$new_working_dir";
		CvsCommand("cvs update -d -P -C -A");
	}
	else
	{
		CvsCommand("gcomp -d -F 1cv7.new.md -D \"$new_working_dir\" -v $gcomp_decompile_opts | perl $auto_add_script");
		DoCommit();
	}

	if( $last_commit_tag && chdir("$top_dir\\$new_working_dir") )
	{
		CvsCommand("cvs update -d -P -C -A");
		CvsCommand("cvs tag -d $last_commit_tag");
		CvsCommand("cvs tag $last_commit_tag");
	}
}

sub TryToCompile
{
	my $i = 0;
	my $ver;
	my $line;

	$AppendToComment = 0;

	if( -f "$working_dir\\gcomp.ver" ) #если есть gcomp.ver, то возьмём версию оттуда
	{
		open VER, "< $working_dir\\gcomp.ver";
		$line = <VER>;
		close VER;
		$line =~ m/^([.\d]+)((\.[^.\d]*)|())$/;
		$ver = "-$1";
		return 1 if CvsCommand("gcomp$ver -c -F 1cv7.new.md -D \"$working_dir\"") == 0;
	}

	while( $i < @gcomp_versions )
	{
		$ver = $gcomp_versions[$i];
		print "\ttry gcomp$ver\n";
		if( CvsCommand("gcomp$ver -c -F 1cv7.new.md -D \"$working_dir\"") == 0 )
		{
			shift @gcomp_versions while $i-- > 0; #убираем из очереди отработанные версии
			return 1;
		}
		$i++;
	}

	$AppendToComment = 1;
	return 0;
}

sub DoImport
{
	chdir "$top_dir\\$new_working_dir" or return 0;
	SetDateTime($prev_date_time) if $SaveDateTime;
	CvsCommand("cvs import -n -m \"$prev_comment\" -C -d $new_module");
	$commit_count++;
}

sub	DoCommit
{
	chdir $top_dir;

	chdir $new_working_dir or return 0;
	SetDateTime($prev_date_time) if $SaveDateTime;
	CvsCommand("cvs commit -F ..\\comment");
	$commit_count++;
}

sub SetDateTime
{
	my $date_time = shift;
	#format: 2004/04/13 00:00:00
	$date_time =~ m#(\d+)/(\d+)/(\d+)\s+(\d+):(\d+)#;
	system "cmd /C date $3.$2.$1";
	system "cmd /C time $4:$5";
}


sub DetermineGCompVersion
{
	return "gcomp"        if $last_commit_date_time ge "2004/04/13 00:00:00";
	return "gcomp-2.0.11" if $last_commit_date_time ge "2004/04/05 00:00:00";
	return "gcomp-2.0.10" if $last_commit_date_time ge "2004/03/13 00:00:00";
	return "gcomp-2.0.9"  if $last_commit_date_time ge "2004/03/11 00:00:00";
	return "gcomp-2.0.8"  if $last_commit_date_time ge "2004/03/10 00:00:00";
	return "gcomp-2.0.7"  if $last_commit_date_time ge "2004/02/25 06:29:00";
	return "gcomp-2.0.6"  if $last_commit_date_time ge "2004/02/16 00:00:00";
	return "gcomp-2.0.5"  if $last_commit_date_time ge "2004/02/03 00:00:00";
	return "gcomp-2.0.4"  if $last_commit_date_time ge "2004/01/27 00:00:00";
	return "gcomp-1.0";
}

sub BuildCommitLog
{
	my $line;
	my $i = 0;
	my @files;

	chdir $top_dir;
	RemoveDir("$top_dir\\$working_dir") if -d $working_dir;
	CvsCommand("cvs co -d \"$working_dir\" $module");
	chdir $working_dir;
	CvsCommand("cvs update -d");

	open CVS, "cvs log |";
	foreach $line (<CVS>)
	{
		$line =~ s/[\r\n]//g; #уберём завершающие \n и \r, если они там есть

		if( $commit_id )
		{
			if( $line =~ m/^(-{28})|(={77})\s*$/ )
			{
				$tag = $tags{"$revision"};
				#Утолкаем все поля в одну строку, чтобы потом сортировать можно было
				$files[$i++] = join("\001", $date_time, $commit_id, $tag, $comment, $working_file, $date_time, $author, $state, $revision);
				$commit_id = "";
			}
			#branches:  1.8.2;
			elsif( $line =~ m/^branches:\s+(\S+);$/ )
			{
				#чего делать с бранчами - пока неизвестно
				$branch = "$1";
			}
			else
			{
				$comment .= "\n" if $comment_line > 0;
				$comment .= $line;
				$comment_line++;
			}
		}
		elsif( $read_tags )
		{
			if( $line =~ m/\t+(\S*):\s*(\S*)\s*/ )
			{
				$tags{"$2"} = "$1";
			}
			else
			{
				$read_tags = 0;
			}
		}
		elsif( $line =~ m/^RCS file:\s*(.*)$/ )
		{
			$rcs_file = "$1";
			foreach $key (keys %tags) {delete $tags{$key};}
		}
		elsif( $line =~ m/^Working file:\s*(.*)$/ )
		{
			$working_file = "$1";
			$working_file =~ s/\//\\/g;
		}
		elsif( $line =~ m/^symbolic names:\s*$/ )
		{
			$read_tags = 1;
		}
		elsif( $line =~ m/^revision\s*(.*)$/ )
		{
			$revision = "$1";
		}
		#date: 2004/04/17 15:55:33;  author: leha;  state: Exp;
		#date: 2004/04/17 17:04:26;  author: leha;  state: Exp;  lines: +0 -2;  kopt: kv;  commitid: 5544081639a0000;
		elsif( $line =~ m/^date:\s*(.*);\s*author:\s*(.*);\s*state:\s*(\S*);\s*(.*)$/ )
		{
			$date_time = "$1";
			$author = "$2";
			$state = "$3";
			$rest = "$4";
			if( $4 =~ m/lines:.*;\s*kopt:.*;\s*commitid:\s*(.*);.*/ )
			{
				$commit_id = "$1";
			}
			else
			{
				$commit_id = "000000000000000";
			}
			$comment = "";
			$comment_line = 0;
		}
	}
	close CVS;

	@commits = sort @files;
}

#add slashes into paths an do cvs command
sub CvsCommand
{
	my $cmd = shift;
	$cmd =~ s/\\/\\\\/g;
	print "\t", $cmd, "\n";
	return system "$cmd 2>&1";
}

sub DateDiff
{
	my $d1 = shift;
	my $d2 = shift;
	$d1 =~ s/[\/: ]//g;
	$d2 =~ s/[\/: ]//g;
	return $d2 - $d1;
}

sub DeleteAllFiles
{
	print "deleting files....";
	find(\&DeleteFile, "$top_dir\\$working_dir");
	print "done\n"
}

sub DeleteFile
{
	#system "rm -f $_" unless -d $_ or $File::Find::dir =~ m/^.*\\+(CVS)|(cvs)\\*$/i;
	system "del /F /Q $_" unless -d $_ or $File::Find::dir =~ m/^.*\\+(CVS)|(cvs)\\*$/i;
}

sub RemoveDir
{
	my $dir = shift;
	#CvsCommand("rm -rf $dir");
	CvsCommand("rmdir /S /Q $dir");
}