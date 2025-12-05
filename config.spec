x64:
	setg "%ASSEMBLY_PATH" "bin/Rubeus.exe"
	setg "%ARGS" "asktgt, /user:SOMEUSER, /domain:somedomain.local, /password:secretpass"
	
	next "%ARGS": pack $ARG0 "Z" %_
	next "%ARGS": pack $ARG1 "Z" %_
	next "%ARGS": pack $ARG2 "Z" %_
	next "%ARGS": pack $ARG3 "Z" %_
