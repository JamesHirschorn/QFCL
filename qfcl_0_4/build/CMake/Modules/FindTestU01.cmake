find_path( TESTU01_ROOT_DIR 
           NAMES testu01/TestU01.h
           PATHS ${TESTU01_ROOT} $ENV{TESTU01_ROOT} )
		   
set( ERROR_MESSAGE "Cannot find the TestU01 installation. Please set TESTU01_ROOT to the root directory containing TESTU01." )

mark_as_advanced( TESTU01_ROOT_DIR )
if( TESTU01_ROOT_DIR STREQUAL TESTU01_ROOT_DIR-NOTFOUND )
	message( SEND_ERROR ${ERROR_MESSAGE} )
else()
	set( TESTU01_FOUND TRUE )
endif()

set( TestU01_INCLUDE_DIRS ${TESTU01_ROOT_DIR}/include )
set( TestU01_LIBRARY_DIRS ${TESTU01_ROOT_DIR}/lib )

