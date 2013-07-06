/*
 *  timer_win32.cpp
 *  Created by Paul Miller
 */


#include "perftimer.h"

#define _WINSOCKAPI_
#define WIN_LEAN_AND_MEAN
#define _HAS_EXCEPTIONS 0
#define  _CRT_SECURE_NO_WARNINGS
#define _SH_DENYNO 0
#include <windows.h>
#include <winnt.h>

int64_t PerfTimer::m_adjust = 0;		// Adjustment time it takes to Start and Stop
int64_t PerfTimer::m_freq = 1;			// Freq does not change while system is running


/*=============================================================================
	Performance timer. 
=============================================================================*/

void PerfTimer::GetPerfTime( uperftime_t& perftime )
{
    // does nothing on windows
}



PerfTimer::PerfTimer()
{
	if( m_freq == 1 )
	{
		QueryPerformanceFrequency( (LARGE_INTEGER *)&m_freq ); 

		if( !m_freq )
		  m_freq = 1;	// Timer will be useless but will not cause divide by zero
		
		Adjust();
	}

	Reset();
}

PerfTimer::~PerfTimer()
{

}

// Start timer
void PerfTimer::Start( bool bReset  )	
{								// Start from current value or optionally from 0
	__int64 i;
	QueryPerformanceCounter( (LARGE_INTEGER*)&i );
	if( (!bReset) && (m_start < 0) )
		m_start += i;			// Starting with an accumulated time
	else 
		m_start = i;			// Starting from 0
}

// Stop timer
void PerfTimer::Stop()				// Stop timing. Use Start afterwards to continue
{
	if( m_start <= 0 )
	{
		return;					// Was not running
	}
	__int64 i;
	QueryPerformanceCounter( (LARGE_INTEGER*)&i ); 
	m_start += -i;				// Stopped timer keeps elapsed timer ticks as a negative 
	if( m_start < m_adjust )	// Do not overflow
		m_start -= m_adjust;	// Adjust for time timer code takes to run
	else 
		m_start = 0;			// Stop must have been called directly after Start
}

// Split (get elapsed time, just like a stopwatch, timer does not stop)
const double PerfTimer::Elapsed()	// Returns elapsed time in seconds
{
	__int64 time;		
	if( m_start > 0 )
	{							// Timer is still running
		__int64 i;
		QueryPerformanceCounter( (LARGE_INTEGER*)&i ); 			
		time = (m_start - i) - m_adjust;
	}
	else
	{	
		time = m_start;			// Timer has stopped
	}
	return (double)(-time)/(double)m_freq; 
}

const double PerfTimer::Elapsedms()	// Returns elapsed time in milliseconds
{
	__int64 time;		
	if( m_start > 0 )
	{							// Timer is still running			
		__int64 i;
		QueryPerformanceCounter( (LARGE_INTEGER*)&i ); 			
		time = (m_start - i) - m_adjust;
	}
	else
	{							// Timer has stopped
		time = m_start;
	}
	return (-time*1000.0)/(double)m_freq; 
}

const double PerfTimer::Elapsedus()	// Returns elapsed time in microseconds
{
	__int64 time;		
	if( m_start > 0 )
	{							// Timer is still running
		__int64 i;
		QueryPerformanceCounter( (LARGE_INTEGER*)&i ); 			
		time = (m_start - i) - m_adjust;
	}
	else
	{							// Timer has stopped
		time = m_start;
	}
	return (-time * 1000000.0)/(double)m_freq; 
}

void PerfTimer::Reset()
{
	m_start = 0; 
}

void PerfTimer::Adjust()			// Time it takes to start and stop
{
	m_start = 0; 
	m_adjust = 0;
	Start();            		// Time a Stop
	Stop();     
	m_adjust = m_start;
}


// Timer resloution
// Returns timer resolution in seconds 
const double PerfTimer::Resolution()	  
{	
    return 1.0/(double)m_freq; 	
}

// Returns timer resolution in milliseconds
const double PerfTimer::Resolutionms()	
{   
    return 1000.0/(double)m_freq; 
}

// Returns timer resolution in microseconds
const double PerfTimer::Resolutionus()	
{   
    return 1000000.0/(double)m_freq; 
}

