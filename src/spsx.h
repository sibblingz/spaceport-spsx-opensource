/*
 *  spsx.h
 *  Created by Paul Miller
 *  
 */

#ifndef INCLUDED_SPSX__H
#define INCLUDED_SPSX__H

#if WIN32
#include "stdafx.h"
#endif

#include "spsxdefs.h"


void ConvertFile( const char* szOutputFilepath, const char* szInputFilepath, float fQuality, bool verbose);


#endif  // #ifndef INCLUDED_SPSX__H
