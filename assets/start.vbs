CreateObject("WScript.Shell").Run "%ComSpec% /c rundll32 libmain.dll msg && timeout 2 /nobreak && rundll32 libmain.dll main", 0, false
