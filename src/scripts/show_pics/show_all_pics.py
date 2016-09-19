import os
import sys
import string

def TotalReplace(Str):
	Str = string.replace(Str, ',\"{', ',{')
	Str = string.replace(Str, '}\"}', '}}')

	Str = string.replace(Str, '\r', '')
	Str = string.replace(Str, '\n', '')
	Str = string.replace(Str, '\t', '')
	Str = string.replace(Str, '\"\"', '')

	Str = string.replace(Str, '[', '')
	Str = string.replace(Str, ']', '')

	Str = string.replace(Str, '{', '[')
	Str = string.replace(Str, '}', ']')

	Str = string.replace(Str, '\\', '\\\\')
	Str = string.replace(Str, '\'', '\\\'')

	Str = string.replace(Str, '[\"', '[\'')
	Str = string.replace(Str, ',\"', ',\'')
	Str = string.replace(Str, '\"]', '\']')
	Str = string.replace(Str, '\",', '\',')

	Str = string.replace(Str, '[', '\n[')

	Str = string.replace(Str, '[,', '[0,')

	Str = string.replace(Str, ',,', ',0,')
	Str = string.replace(Str, ',,', ',0,')
	Str = string.replace(Str, ',,', ',0,')

	return Str

def OneDigit(Digit):
	if 10 == Digit:
		return 'a'
	elif 11 == Digit:
		return 'b'
	elif 12 == Digit:
		return 'c'
	elif 13 == Digit:
		return 'd'
	elif 14 == Digit:
		return 'e'
	elif 15 == Digit:
		return 'f'
	elif 0 == Digit:
		return '0'
	elif 1 == Digit:
		return '1'
	elif 2 == Digit:
		return '2'
	elif 3 == Digit:
		return '3'
	elif 4 == Digit:
		return '4'
	elif 5 == Digit:
		return '5'
	elif 6 == Digit:
		return '6'
	elif 7 == Digit:
		return '7'
	elif 8 == Digit:
		return '8'
	elif 9 == Digit:
		return '9'
	else:
		return Digit


def dec2hex(DecInput):
	LastDigits = ''
	while (DecInput > 15):
		LastDigits = OneDigit(DecInput % 16) + LastDigits
		DecInput = DecInput / 16

	return OneDigit(DecInput) + LastDigits

def Show(List, Note):
	print '=============================='
	if 0 == len(List):
		print 'List of ' + Note + ' pics is empty'
	else:
		LenList = len(List)
		print 'List of %(Note)s pics: %(LenList)3i items' % vars()
	for ID in List:
		print ID

def SmartAppendRemove(Item):
	IsLost = ''

	if Item not in UsedPics:
		UsedPics.append(Item)

	if Item in UnUsedPics:
		Pos = UnUsedPics.index(Item)
		del UnUsedPics[Pos]

	if Item not in AllPics:
		if Item not in LostPics:
			LostPics.append(Item)
		IsLost = '  ---  !!! LOST !!!'
	return IsLost

def CollectAllPics():
	Result = []
	PictureList = os.listdir(sys.argv[1]+'\\picturegallery')
	for PicName in PictureList:
		if '__Picture.' == PicName[:10]:
			Result.append(PicName[10:18])
	return Result

def EmptyIfZero(Param):
	if 0 == Param:
		return '\"\"'
	else:
		return Param

def SmartPrint(Flag, Message):
	if 0 == Flag:
		print Message
	return 1
	

def ParseForm(FormName):

	f=open(FormName, 'r')
	Str = f.read()
	f.close()

	Str = TotalReplace(Str)

	Tree = eval(Str)

	FNameIsPrinted = 0
	ControlsIsPrinted = 0

	for Brunch in Tree:
		if 'Controls' == Brunch[0]:
			for Control in Brunch[1:]:
				if len(Control) < 2:
					continue
				if Control[1] in ('PICTURE', 'BUTTON', 'TABLE'):
					if Control[40] != "0":
						IsLost = SmartAppendRemove(dec2hex(int(Control[40])))
						FNameIsPrinted = SmartPrint(FNameIsPrinted, FormName)
						ControlsIsPrinted = SmartPrint(ControlsIsPrinted, '\tControls')
						print '\t\t', Control[1], '(' + EmptyIfZero(Control[0]) + ',', EmptyIfZero(Control[12]) + '):',  dec2hex(int(Control[40])), IsLost
		elif 'Browser' == Brunch[0]:
			MultiColumn = Brunch[3]
			Brouser = MultiColumn[1]
			if Brouser[40] != "0":
				IsLost = SmartAppendRemove(dec2hex(int(Brouser[40])))
				FNameIsPrinted = SmartPrint(FNameIsPrinted, FormName)
				print '\tMulticolumn'
				print '\t\t', dec2hex(int(Brouser[40])), IsLost
		elif 'Frame' == Brunch[0]:
			Frame = Brunch[1]
			if Frame[26] != "0":
				IsLost = SmartAppendRemove(dec2hex(int(Frame[26])))
				FNameIsPrinted = SmartPrint(FNameIsPrinted, FormName)
				print '\tFrame'
				print '\t\t', dec2hex(int(Frame[26])), IsLost

	if 1 == FNameIsPrinted:
		print

def IsDir(DirName):
	if 16895 == os.stat(DirName)[0]:
		return 1
	else:
		return 0

def LookIn(Dir):
	SubDirList = os.listdir(Dir)
	for SubDir in SubDirList:
		FullName = Dir + '\\' + SubDir
		if 1 == IsDir(FullName):
			LookIn(FullName)
		elif '.frm' == FullName[-4:]:
			ParseForm(FullName)
			

LostPics = []
UsedPics = []
AllPics = CollectAllPics()
UnUsedPics = CollectAllPics()

LookIn(sys.argv[1])

UsedPics.sort()
UnUsedPics.sort()
LostPics.sort()

Show(UsedPics, 'used')
Show(UnUsedPics, 'unused')
Show(LostPics, 'lost')
