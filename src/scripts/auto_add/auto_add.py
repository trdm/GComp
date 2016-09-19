# vim:fileencoding=cp1251

#==============================================================================================================
# Скрипт для анализа информации, выдаваемой gcomp -d -v, и автоматического добавления/удалени
# файлов в/из репозитарий.
#==============================================================================================================
# Скрипт получает в stdin log от gcomp -v, и если там есть новые файлы (префикс 'NEW:'),
# то для них выполняется команда cvs add. Для удалённых файлов (префикс 'DEL:') выполняется 
# cvs remove.
# Информация о добавленных и удалённых файлах выводятся в stdout.
#
# Команда cvs commit скриптом не выполняется - её нужно выполнить вручную.
#==============================================================================================================

import sys, os, os.path, time, getopt, re, string, array
working_dir = 'Src'
filter_expr = ''

session_splitter = "-------------------------------------------------------------------------------------------------"

def Analyze():
	reSplitPath = re.compile(r'^(.*)\+(.*)$')
	time_printed = 0
	for line in sys.stdin.readlines():
		line = string.strip(line, '\r\n')
		line = unicode(line, 'cp866').encode('cp1251')
		(command, msg, fname) = ParseLine(line)
		if command:
			if filter_expr:
				res = reFilter.search(fname)
				if res != None:
					print "File '" + fname + "' skipped by filter"
					continue

			time_printed = SessionStart(time_printed)
			print '%s %s' % (msg, fname)
			cwd = os.getcwd()
			(working_dir, fname) = AddParentDirs(fname)
			DoCommand(working_dir, 'cvs ' + command + ' ' + fname)
			os.chdir(cwd)
		else :
			print line
	SessionEnd(time_printed)

def AddParentDirs(path):
	dir_list = []
	while path != '':
		(path, dir) = os.path.split(path)
		dir_list.append(dir)

	working_dir = dir_list.pop()
	os.chdir(working_dir)
	print os.getcwd()

	path = ''
	while len(dir_list) > 1:
		dir = dir_list.pop()
		path = os.path.join(path, dir)
		print 'ADD Dir: %s' % (path)
		print >> sys.stderr,  "ADD Dir: %s" % (unicode(path, 'cp1251').encode('cp866'))
		DoCommand(working_dir, 'cvs add ' + path)
			
	path = os.path.join(path, dir_list.pop()) #file name for addition/deletion, without working dir
	return (working_dir, path)

def ParseLine(line):
	reNew = re.compile(r"^NEW:\s*'*(.+)'*\s*$", re.I)
	reDel = re.compile(r"^DEL:\s*'*(.+)'*\s*$", re.I)

	res = reNew.search(line)
	if res != None:
		return ('add', 'ADD', res.group(1))

	res = reDel.search(line)
	if res != None:
		return ('remove', 'DEL', res.group(1))

	return ('', '', '')

def DoCommand(working_dir, command):
	command = string.replace(command, '\\', '/')
	print '\\%s>%s' % (working_dir, command)
	(child_stdin, child_stdout) = os.popen2(command + ' 2>&1')
	for line in child_stdout.readlines():
		line = string.strip(line, '\r\n')
		print '\t%s' % (line)

def SessionStart(time_printed):
	if not time_printed:
		tm = time.localtime()
		print '%04i.%02i.%02i %02i:%02i:%02i' % (tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec)
		print session_splitter;
	return 1

def SessionEnd(time_printed):
	if time_printed:			
		print session_splitter

def usage():
	print "Usage:"
	print "\tpython auto_add.py [--filter <RegExp>]"
#==============================================================================
if __name__ == '__main__':
	try:
		opts, args = getopt.getopt(sys.argv[1:],
         	'hf:',
         	['help', 'filter='])
	except getopt.GetoptError:
		usage()
		sys.exit(2)

	for opt, arg in opts:
		if opt in ('-h', '--help'):
			usage()
			sys.exit(0)
		elif opt in ('-f', '--filter'):
			filter_expr = arg
			reFilter = re.compile(filter_expr, re.I)

	time_printed = False
	Analyze()
