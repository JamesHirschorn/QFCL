# FindQuantLib.cmake
#
# CMake module for find_package
#
# Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
#
# Use, modification and distribution are subject to 
# the BOOST Software License, Version 1.0.
# (See accompanying file LICENSE.txt)
# --------------------------------------------------------------

find_path( QUANTLIB_ROOT_DIR 
           NAMES ql/quantlib.hpp
           PATHS ${QUANTLIB_ROOT} $ENV{QUANTLIB_ROOT} )
#find_package( QuantLib )

if( NOT QUANTLIB_ROOT_DIR STREQUAL QUANTLIB_ROOT_DIR-NOTFOUND )
    include_directories( ${QUANTLIB_ROOT_DIR} )
	set( QuantLib_LIBRARY_DIR "${QUANTLIB_ROOT_DIR}/lib" )
	link_directories( ${QuantLib_LIBRARY_DIR} )
	set( QuantLib_LIBRARIES optimized ${QuantLib_LIBRARY_DIR}/QuantLib-vc110-mt.lib debug ${QuantLib_LIBRARY_DIR}/QuantLib-vc110-mt-gd.lib )
	#message( "QuantLib_LIBRARIES:" ${QuantLib_LIBRARIES} )
else()
	set( ERROR_MESSAGE "Cannot find the QuantLib installation. Please set QUANTLIB_ROOT to the root directory containing QuantLib." )
	message( SEND_ERROR ${ERROR_MESSAGE} )
endif()
mark_as_advanced( QUANTLIB_ROOT_DIR )

set( QuantLib_INCLUDE_DIRS ${QUANTLIB_ROOT_DIR} )
set( QuantLib_LIB_DIR "${QUANTLIB_ROOT_DIR}/lib" )