
set(nany-from-c_error_has_occured   false)


macro(nmessage msg)
	if(UNIX)
		message(STATUS "[1;30m{nany-from-c}[0m  ${msg}")
	else()
		message(STATUS "{nany-from-c}  ${msg}")
	endif()

endmacro()


macro(nwarning msg)
	if(UNIX)
		message(STATUS "[1;33m{nany-from-c}  [warning][0m ${msg}")
	else()
		message(STATUS "{nany-from-c}  [WARNING] ${msg}")
	endif()
endmacro()


macro(nerror msg)
	if(UNIX)
		message(STATUS "[1;31m{nany-from-c}  [error][0m ${msg}")
	else()
		message(STATUS "{nany-from-c}  [ERROR] ${msg}")
	endif()
	set(nany-from-c_error_has_occured  true)
endmacro()



