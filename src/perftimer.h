/*
 *  perftimer.h
 *  Created by Paul Miller
 *  
 * 
 */

#ifndef INCLUDED_PERFTIMER__H
#define INCLUDED_PERFTIMER__H

#include "cstdtypes.h"


/*=============================================================================
	Performance timer. This works like any stop watch. 
	You can "Start" and "Stop" the timer, get a "Split", and "Reset".
	Results can be in seconds, milliseconds, or microseconds.
=============================================================================*/

typedef uint64_t uperftime_t;	
typedef int64_t	perftime_t;


class PerfTimer
{
protected:
	// internal function
	void GetPerfTime( uperftime_t& perftime );
    
public:
	PerfTimer();
	~PerfTimer();

	// Start timer
	void Start( bool bReset = false );	

   // Stop timing. Use Start afterwards to continue
	void Stop();					

	// Split (get elapsed time, just like a stopwatch, timer does not stop)
    // Returns elapsed time in seconds
	const double Elapsed();

    // Returns elapsed time in milliseconds
	const double Elapsedms();
    
    // Returns elapsed time in microseconds
	const double Elapsedus();
    
	void Reset();

    // Time it takes to start and stop
	void Adjust();
    
	// Timer resloution
    // Returns timer resolution in seconds
	const double Resolution();
    
    // Returns timer resolution in milliseconds
	const double Resolutionms();
    
    // Returns timer resolution in microseconds
	const double Resolutionus();
    

// Data
protected:
	perftime_t m_start;
    
#if defined(__APPLE__)    
	static perftime_t m_adjust;			// Adjustment time it takes to Start and Stop
	static double m_conversionS;		// Conversion factor for seconds.
	static double m_conversionMS;		// Conversion factor for milliseconds.
	static double m_conversionUS;		// Conversion factor for microseconds.

#elif defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)
	static int64_t m_adjust;		// Adjustment time it takes to Start and Stop
	static int64_t m_freq;			// Freq does not change while system is running
#endif    
};


#endif  // #ifndef INCLUDED_PERFTIMER__H
