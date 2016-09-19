# vim:fileencoding=cp1251
#
Version = """
ExtractProc
Version:        1.3
Authors:
    Alexander S. Gordienko <alex-go@vstu.kirov.ru>
    Alexey Dirks <adirks@ngs.ru>
"""
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

import sys, getopt, re, os, os.path, string



#########################################################################
##############      Extract functions into files      ###################
#########################################################################
def Extract(Dir, ModuleName):
        FullName = os.path.join(Dir, ModuleName)
        ModuleName = os.path.splitext(ModuleName)[0]

        dir_len = len(SrcDir) + 1
        relDir = Dir[dir_len:]
        ExtractPath = os.path.join(DestDir, relDir, ModuleName)
        relModulePath = os.path.join(relDir, ModuleName)

        if not os.path.exists(ExtractPath):
                os.makedirs(ExtractPath)

        Module = string.split(open(FullName, 'rb').read(), os.linesep)
        newModule = ''

        headRem     = re.compile(r'^\s*//\*+\s*$', re.I)
        bodyRem     = re.compile(r'(^\s*//.*$|^\s*$)', re.I)
        forvardProc = re.compile(r'^\s*(?:Функция|Процедура)\s+(?P<name>[0-9a-zA-Zа-яА-Я_]+)\s*' + \
                                r'\([^\)]*\)\s*(?:Экспорт)?\s*(?:Далее)', re.I)
        headProc    = re.compile(r'^\s*(?:Функция|Процедура)\s+(?P<name>[0-9a-zA-Zа-яА-Я_]+)', re.I)
        tailProc    = re.compile(r'^\s*(?:КонецФункции|КонецПроцедуры).*$', re.I)
    
        State = 0
        LineNum = 0
        beginScope = 0
        endScope = 0
        procName = ''
        
        if Verbose > 0:
                print unicode(relModulePath + ':', 'windows-1251')
        
        for line in Module:
                head = string.join(Module[LineNum:LineNum+10])
                if 0 == State:
                        tmp = headProc.match(line)
                        if (None == forvardProc.match(head)) and (None != tmp):
                                State = 2
                                procName = tmp.group('name')
                                beginScope = LineNum            
                        elif None != headRem.match(line):
                                State = 1
                                beginScope = LineNum            
                elif 1 == State:
                        tmp = headProc.match(line)
                        if (None == forvardProc.match(head)) and (None != tmp):
                                State = 2
                                procName = tmp.group('name')
                        elif None == bodyRem.match(line):
                                State = 0
                elif 2 == State:
                        if None != tailProc.match(line):
                                State = 0
                                newModule = newModule + string.join(Module[endScope:beginScope], os.linesep) + \
                                        os.linesep + '//###ВынесеноВ ' + procName + '.txt' + os.linesep
                                endScope = LineNum + 1

                                if Verbose > 1:
                                        print unicode('    .' + procName, 'windows-1251')
                                open(os.path.join(ExtractPath, procName + '.txt'),
                                        'wb').write(string.join(Module[beginScope:endScope], os.linesep))
                LineNum += 1            

        if Verbose > 1:
                print

        newModule = newModule + string.join(Module[endScope:], os.linesep)
        open(os.path.join(ExtractPath, ModuleName + '.1ss'), 'wb').write(newModule)

def visit(arg, dir, names):
        for name in names:
                if os.path.splitext(name)[1] in ('.1s', '.1S'):
                        Extract(dir, name)


#########################################################################
############## Assemble separate functions into single module  ##########
#########################################################################
def Assemble(Dir, ModuleName):
        dir_len = len(SrcDir) + 1
        relDir = Dir[dir_len:]

        FullModuleName = os.path.join(Dir, ModuleName)
        relName = os.path.join(relDir, ModuleName)
        ModuleName = os.path.splitext(ModuleName)[0]

        SkeletonDir = os.path.join(DestDir, relDir, ModuleName)
        SkeletonName = os.path.join(SkeletonDir, ModuleName + '.1ss')

        if not os.path.exists(SkeletonName):
                return
        
        if Verbose > 0:
                print unicode(relName + ':', 'cp1251')

        ProcPlace = re.compile(r'^//###ВынесеноВ (?P<ProcFile>\S+)\s*$', re.I)

        Module = ''
        ToWrite = 0
        Skeleton = string.split(open(SkeletonName, 'rb').read(), os.linesep)
        for line in Skeleton:
                res = ProcPlace.match(line)
                if res != None:
                        ProcFile = os.path.join(SkeletonDir, res.group('ProcFile'))
                        Module = Module + open(ProcFile, 'rb').read() + os.linesep
                        ToWrite = 1

                        relName = res.group('ProcFile')

                        if Verbose > 1:
                                print unicode('    .' + relName, 'cp1251')
                else:
                        Module = Module + line + os.linesep

        if Verbose > 1:
                print

        if ToWrite:
                Module = string.rstrip(Module) + os.linesep #уберём пустые строки в конце файла
                open(FullModuleName, 'wb').write(Module)

#------------------------------------------------------------------------
def visit_assemble(arg, dir, names):
        for name in names:
                if os.path.splitext(name)[1] in ('.1s', '.1S'):
                        Assemble(dir, name)

def main():
        global SrcDir, DestDir, Verbose
        
        try:
                opts, args = getopt.getopt(sys.argv[1:],
                                        'hEVemp:f:b:sv',
                                        ['help', 'examples', 
                                        'version', 'extract',
                                        'make', 'extract_path=',
                                        'file=', 'base_path=',
                                        'silent', 'verbose'])
        except getopt.GetoptError:
                usage()
                sys.exit(2)

        global fnModule, ExtractPath

        Action = 0    
        SrcDir = ''
        DestDir = ''
        fnModule = ''
        ProcessAll = 1
        Verbose = 1
    
        for o, a in opts:
                if o in ('-h', '--help'):
                        usage()
                        sys.exit()
                elif o in ('-E', '--examples'):
                        examples()
                elif o in ('-V', '--version'):
                        version()
                elif o in ('-e', '--extract'):
                        Action = 1
                elif o in ('-m', '--make'):
                        Action = 2
                elif o in ('-p', '--extract_path'):
                        DestDir = os.path.abspath(a)
                elif o in ('-f', '--file'):
                        fnModule = os.path.abspath(a)
                        ProcessAll = 0
                elif o in ('-b', '--base_path'):
                        SrcDir = os.path.abspath(a)
                        ProcessAll = 1
                elif o in ('-s', '--silent'):
                        if Verbose < 2:
                                Verbose = 0
                elif o in ('-v', '--verbose'):
                        Verbose = 2
                else:
                        usage()
                        sys.exit(2)

        if ProcessAll == 0:
                (SrcDir, fnModule) = os.path.split(fnModule)

        if SrcDir == '':
                SrcDir = r'.\Src'
        if DestDir == '':
                DestDir = SrcDir

        if Action == 1:
                if ProcessAll == 1:
                        os.path.walk(SrcDir, visit, '')
                else:
                        Extract(SrcDir, fnModule)
        elif Action == 2:
                if ProcessAll == 1:
                        os.path.walk(SrcDir, visit_assemble, '')
                else:          
                        Assemble(SrcDir, fnModule)
        else:
                usage()
                sys.exit(2)

def usage():
        print ur"""
Запуск: ExtractProc.py {-h|-E|-V|-e|-m} 
                       [-b <base_path> | [-f <file>] [-p <extract_path>]] 
                       [-s|-v] 

Аргументы:
    -h | --help         - эта помощь
    -E | --examples     - показать примеры использования
    -V | --version      - отображение версии скрипта и выход
    -e | --extract      - извлечь процедуры/функции из модуля
    -m | --make         - внести процедуры/функции в модуль
    -b | --base_path    - путь к распакованной gcomp конфигурации
                            (обрабатываются все *.1s)
    -f | --file         - файл модуля
    -p | --extract_path - путь для извлечения
    -s | --silent       - "тихий" режим (данный режим перекрывается --verbose)
    -v | --verbose      - расширенный вывод
"""

def examples():
        print ur"""
Примеры:
  
  Извлечь модуль '.\Src\Документы\Аккредитив\МодульФормы.1s' в папку по 
  умолчанию ('.\Src\Документы\Аккредитив\МодульФормы\'):
    ExtractProc.py -e -f .\Src\Документы\Аккредитив\МодульФормы.1s

  Извлечь модуль '.\Src\ГлобальныйМодуль.1s' в папку '.\Src\ГлобальныйМодуль\':
    ExtractProc.py -e -f .\Src\ГлобальныйМодуль.1s -p .\Src\ГлобальныйМодуль
  
  Извлечь все модули распакованной конфигурации расположенной в папке по 
  умолчанию ('.\Src\'):
    ExtractProc.py -e
    
  Извлечь все модули распакованной конфигурации расположенной в
  папке '.\SrcDir\':
    ExtractProc.py -e -b .\SrcDir\

  Извлечь все модули распакованной конфигурации расположенной в
  папке '.\SrcDir\' в папку c:\tmp\SrcDirExt\:
    ExtractProc.py -e -b .\SrcDir\ -p c:\tmp\SrcDirExt\

  Внесение делается аналогично:
    ExtractProc.py -m
"""
        sys.exit()

def version():
        print Version
        sys.exit()

if __name__ == '__main__':
        main()
