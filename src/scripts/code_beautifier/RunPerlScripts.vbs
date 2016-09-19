$NAME Запускатель Перловых скриптов

Sub CodeBeautifier()
	Set Doc = CommonScripts.GetTextDocIfOpened(0)
	If Doc Is Nothing Then Exit Sub
	Set Pos = CommonScripts.GetDocumentPosition(Doc)
	Doc.Text = Scripts("CodeBeautifier").Beautify_OpenConf(Doc.Text)
	Pos.Restore
End Sub

' Процедура инициализации скрипта
Private Sub Init()
    Set c = Nothing
    On Error Resume Next
    Set c = CreateObject("OpenConf.CommonServices")
    On Error GoTo 0
    If c Is Nothing Then
        Message "Не могу создать объект OpenConf.CommonServices", mRedErr
        Message "Скрипт " & SelfScript.Name & " не загружен", mInformation
        Scripts.UnLoad SelfScript.Name
		Exit Sub
    End If
    c.SetConfig(Configurator)
	SelfScript.AddNamedItem "CommonScripts", c, False
End Sub
 
Init ' При загрузке скрипта выполняем инициализацию
