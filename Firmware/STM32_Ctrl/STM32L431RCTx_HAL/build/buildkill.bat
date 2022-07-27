:: uVision.tempkill

:: Listing Files
del *.cod /s
del *.htm /s
del *.i /s
del *.lst /s
::del *.map /s
del *.map.old /s
del *.map.view /s
del *.m51 /s
del *.m66 /s
del *.scr /s

:: Object Files
::del *.axf /s
del *.b[0-2][0-9] /s
del *.b3[0-1] /s
del *.crf /s
del *.d /s
::del *.elf /s
del *.lib /s
del *.o /s
del *.obj /s
del *.sbr /s

:: Firmware Files
::del *.bin /s
::del *.h86 /s
::del *.hex /s

:: Build Files
::del *.bat /s
del *._ac /s
del *._ia /s
del *.__i /s
del *._ii /s

:: Debugger Files
del *.ini /s

:: Other Files
del *.build_log.htm /s
del *.cdb /s
del *.cprj /s
del *.dep /s
del *.ic /s
del *.lin /s
del *.lnp /s
del *.orc /s
del *.pack /s
del *.pdsc /s
del *.plg /s
del *.sct /s
del *.scvd /s
del *.uvl /s
del *.uvla /s
del *.uvtsk /s
del *.sfd /s
del *.sfr /s
del *.svd.xml /s
del *.svd /s
del *.xml /s
del *.xsd /s

:: Miscellaneous
del ref.json /s
del *.log /s
del *.mk /s
del *.params /s
del *.params.old /s
del *.bak /s
del *.ddk /s
del *.edk /s
del *.mpf /s
del *.mpj /s
del *.omf /s
del *.rpt /s
del *.s19 /s
del *.tmp /s
del *.tra /s
del *.fed /s
del *.l1p /s
del *.l2p /s
del *.iex /s

for /f "delims=" %%a in ('dir /ad /b /s %~dp0^|sort /r') do (
   rd "%%a">nul 2>nul
)

exit
