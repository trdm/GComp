import types
import sys

###################
## Functions beg ##
###################	

def isPrintable(theObject):
	if types.IntType == type(theObject):
		result = 1
	elif types.StringType == type(theObject):
		result = 1
	else:
		result = 0
	return result
	

def NodeView(theNode):
	if types.IntType == type(theNode):
		result = '' + theNode + ' (n)'

	elif types.StringType == type(theNode):
		result = theNode + ' (s)'

	elif types.ListType == type(theNode):
		Name = theNode.pop(0)
		theNode.insert(0, Name)
		if 1 == isPrintable(Name):
			result = '' + Name + ' (l)'
		else:
			result = repr(theNode)[:30] + '...'

	else:
		result = 'unknown type'

	return result
		

def ListView(theList):
	Num = 0
	for x in theList:
		Num += 1
		print Num, NodeView(x)
	print 0, 'Exit'

def BrowseTree(theTree):
	while 1:
		ListView(theTree)
		Command = int(input())
		if 0 == Command:
			break

		else:
			Branch = theTree.pop(Command - 1)
			theTree.insert(Command - 1, Branch)
			BrowseTree(Branch)

###################
## Functions end ##
###################	

cp_1251 = '\
\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\
\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\
\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F\
\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F\
\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F\
\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5A\x5B\x5C\x5D\x5E\x5F\
\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F\
\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7A\x7B\x7C\x7D\x7E\x7F\
\x3F\x3F\x27\x3F\x22\x3A\xC5\xD8\x3F\x25\x3F\x3C\x3F\x3F\x3F\x3F\
\x3F\x27\x27\x22\x22\x07\x2D\x2D\x3F\x54\x3F\x3E\x3F\x3F\x3F\x3F\
\xFF\xF6\xF7\x3F\xFD\x3F\xB3\x15\xF0\x63\xF2\x3C\xBF\x2D\x52\xF4\
\xF8\x2B\x49\x69\x3F\xE7\x14\xFA\xF1\xFC\xF3\x3E\x3F\x3F\x3F\xF5\
\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\
\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F\
\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\
\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF'

#################
## Parser begins
fName = 'Main Metadata Stream'
if len(sys.argv) > 1:
	fName = sys.argv[1]

f=open(fName, 'r')
Str = f.read().translate(cp_1251) 
# Сразу перекодируем весь поток, чтобы ошибки вылезали в читаемом виде.
f.close()

# Приведем строку потока к виду, соответсвующему питоновому списку.
Str = Str.replace('{', '[')
Str = Str.replace('}', ']')

Str = Str.replace('\\', '\\\\') # слэш меняем на двойной
Str = Str.replace('\'', '\\\'') # одинарную кавычку предваряем слэшем

"""
 Двойные кавычки меняем на одинарные. При этом надо учесть, что внутри строки
   так же могут оказаться кавычки. Поэтому меняем только те, которые находятся
   на границах параметров. А эти границы - это запятые или скобки.

   Конечно запятые или скобки могут встретиться и внутри строки - эта ситуация
   тут не обрабатывается.
"""
Str = Str.replace('[\"', '[\'') 
Str = Str.replace(',\"', ',\'') 
Str = Str.replace('\"]', '\']') 
Str = Str.replace('\",', '\',') 

Tree = eval(Str)

## Parser ends
#################

BrowseTree(Tree)
