
#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <afx.h>
#include "Strings.h"
#include "DLL\GComp.h"

void Msg(int MinimalVerbosityLevel, char *msg, ...);
void SystemErrorMessage();
void PrintTime();
void SendInfoMessage(TInfoMessageType MsgType, LPARAM lParam);

bool IsValidFileName(GCString& String);
bool FileExist(LPCSTR FName);
long GetFileSize(GCString& FileName);
bool DirectoryExist(LPCSTR DirName);

void DumbCreateDirectory(char *dir);
//Our CreateDirectory will create all parent dirs, if they not exist
void CreateDirectory(GCString& DirName);

CStringArray* SplitPath(GCString& Path);

void CopyFile(FILE* dest, FILE* src);

//Файлы, пропускаемые при обходе каталога
bool IsIgnoredFile(LPCSTR FileName);
bool IsIgnoredFolder(LPCSTR FileName);
bool FilesExist(LPCSTR Mask);

int MangleFileName(GCString& str);
GCString Suffix(GCString& str);

#endif
