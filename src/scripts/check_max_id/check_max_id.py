# vim:fileencoding=cp1251

#==============================================================================================================
# Скрипт для проверки максимального идентификатора метаданных на предмет попадания в заданный диапазон
#==============================================================================================================

import sys, os, getopt, fileinput, re, string

#default values
working_dir = r'SRC'
id_settings_file_name = r'..\id_settings.txt'
metadata_ids_file = 'ИдентификаторыКонфигурации.mdp'
warning_threshold = 100
user_name = ''

#---------------------------------------------------------------
def usage():
	print u"""
Использование:
   python check_max_id.py [параметры]
Параметры могут быть:
   -h - этот текст
   -d <dir>, -D <dir> - рабочий каталог. По умолчанию SRC.
   -t <n>, -w <n> - пороговое значение для предупреждений. По умолчанию 100.
   -f <file> - имя файла, в котором хранятся диапазоны идентификаторов. Задаётся относительно рабочего каталога. По умолчанию ..\\id_settings.txt
   -u <user> - имя пользователя в CVS. По умолчанию берётся из файла <working dir>\\CVS\\Root.
"""

#---------------------------------------------------------------
def ParseCommandLine():
	global working_dir, warning_threshold, id_settings_file_name, user_name
	try:
		opts, args = getopt.getopt(sys.argv[1:],
         	'hd:D:t:w:f:u:',
         	['help',
				'dir=', 'threshold=', 'range-file=', 'user='])
	except getopt.GetoptError:
		usage()
		sys.exit(2)

	for opt, arg in opts:
		if opt in ('-h', '--help'):
			usage()
			sys.exit(0)
		elif opt in ('-d', '-D', '--dir'):
			working_dir = arg
		elif opt in ('-t', '-w', '--threshold'):
			warning_threshold = int(arg)
		elif opt in ('-f', '--range-file'):
			id_settings_file_name = arg
		elif opt in ('-u', '--user'):
			user_name = arg

#---------------------------------------------------------------
def GetLoginName():
	#user_name = os.getlogin()
	fname = working_dir + r'\CVS\Root'
	#:protocol:user@host:cvs_repository_path_on_host
	reName = re.compile(r'.*:(?P<uname>\S+)@(\S+):.*')

	try:
		for line in fileinput.input(fname):
			res = reName.search(line)
			if None != res:
				user_name = res.group('uname')
	except IOError, (errno, strerror):
		print 'Can not open file ' + fname + ': ' + strerror
		sys.exit(1)

	return user_name

#---------------------------------------------------------------
#находим диапазон идентификаторов из файла ограничений
#Формат строки:
# <user_name> <any spaces> <min ID> <any spaces> <max ID>
# строк м.б. несколько на одного юзера - берётся всегда самая последн
def FindCurrentRange(user_name):
	fname = working_dir + r'\\' + id_settings_file_name
	reComment = re.compile(r'^\s*([;#])|(//).*$')
	reRange = re.compile(r'^\s*(?P<uname>\S+)\s+(?P<min_id>\S+)\s+(?P<max_id>\S+)\s*$')
	min_id = 0
	max_id = 0

	try:
		for line in fileinput.input(fname):
			if reComment.search(line) != None:
				continue
			res = reRange.search(line)
			if (res != None) and (string.upper(res.group('uname')) == string.upper(user_name)) :
				min_id = int( res.group('min_id') )
				max_id = int( res.group('max_id') )
	except IOError, (errno, strerror):
		print 'Can not open file ' + fname + ': ' + strerror
		sys.exit(1)

	if (min_id == 0) or (max_id == 0):
		print "ERROR: ID's range not found for user", user_name
		sys.exit(1)

	return (min_id, max_id)

#---------------------------------------------------------------
def GetMaxMetadataID():
	fname = working_dir + r'\\' + metadata_ids_file
	#{"MainDataContDef","39494","10009","7120"}
	reMaxID = re.compile(r'^\s*Next ID\s*:\s*(?P<max_id>\d+)\s*$')

	try:
		for line in fileinput.input(fname):
			res = reMaxID.search(line)
			if None != res:
				return int( res.group('max_id') )
	except IOError, (errno, strerror):
		print 'Can not open file ' + fname + ': ' + strerror
		sys.exit(1)

	print 'Can not determine max metadata ID. May be file format had changed?'
	sys.exit(1)


#########################################
def main():
	global working_dir, warning_threshold, id_settings_file_name, user_name

	ParseCommandLine()
	if not user_name:
		user_name = GetLoginName()
	(min_id, max_id) = FindCurrentRange(user_name)
	max_metadata_id = GetMaxMetadataID()

	print '%s: %i, %i; max MD ID = %i' % (user_name, min_id, max_id, max_metadata_id)

	if max_metadata_id < min_id :
		print "ERROR: Current metadata ID is out of range! Commit impossible!"
		print "\tID = %i < min ID = %i" % (max_metadata_id, min_id)
		sys.exit(1)
	if max_metadata_id >= max_id :
		print "ERROR: Current metadata ID is out of range! Commit impossible!"
		print "\tID = %i >= max ID = %i" % (max_metadata_id, max_id)
		sys.exit(1)
	if (max_metadata_id + warning_threshold) >= max_id :
		print "WARNING: Current metadata ID is near the top! You need to get new ID's range from project administrator!"
		print "\tID = %i; max ID = %i" % (max_metadata_id, max_id)

###################################################################################
if __name__ == '__main__':
   main()

