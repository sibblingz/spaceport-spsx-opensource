/*
 *  perftimer_macosx.cpp
 *  Created by Paul Miller
 *  
 */

#include "perftimer.h"

#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>


// Calculate the conversion factors for seconds, milliseconds and microseconds.
// Only call this once (or when absolutely needed) since it does double precision divides.
void CalcPerfConversions( double& conversionS, double& conversionMS, double& conversionUS )
{
	mach_timebase_info_data_t info;
	mach_timebase_info( &info );
		
	if( info.denom != 0 )
	{
		double numer = info.numer;
		double denom = info.denom;

		// convert nanoseconds to microseconds
		conversionUS = numer / denom;
		conversionUS /= 1000.0;

		// convet nanoseconds to milliseconds
		conversionMS = numer / denom;
		conversionMS /= 1000000.0;

		// convert nanoseconds to seconds
		conversionS = numer / denom;
		conversionS /= 1000000000.0;	
	}	
}



/*=============================================================================
	Performance timer. 
=============================================================================*/

double PerfTimer::m_conversionS = 1;		// Conversion factor for seconds.
double PerfTimer::m_conversionMS = 1;		// Conversion factor for milliseconds.
double PerfTimer::m_conversionUS = 1;		// Conversion factor for microseconds.
perftime_t PerfTimer::m_adjust = 0;		// Adjustment time it takes to Start and Stop


void PerfTimer::GetPerfTime( uperftime_t& perftime )
{
    perftime = mach_absolute_time();
}

PerfTimer::PerfTimer()
{
	// Have conversions been initialized?
	if( m_conversionUS == m_conversionMS )
	{
		// Initialize conversions.
		CalcPerfConversions( m_conversionS, m_conversionMS, m_conversionUS ); 

		// Success?
		if( m_conversionUS == m_conversionMS )
		{
			m_conversionUS = 1; 
            m_conversionMS = 1;	 // Timer will be useless but will not cause divide by zero
		}
		
		Adjust();
	}

	Reset();
}

PerfTimer::~PerfTimer()
{
}


// Start timer
void PerfTimer::Start( bool bReset )	
{									// Start from current value or optionally from 0
    uperftime_t perftime;
    GetPerfTime( perftime );
    if( (!bReset) && (m_start < 0) )
        m_start += perftime;		// Starting with an accumulated time
    else 
        m_start = perftime;			// Starting from 0
}

// Stop timing. Use Start afterwards to continue
void PerfTimer::Stop()					
{
    if( m_start <= 0 )
    {
        return;						// Was not running
    }
    uperftime_t perftime;
    GetPerfTime( perftime );
    m_start += -perftime;			// Stopped timer keeps elapsed timer ticks as a negative 
    if( m_start < m_adjust )		// Do not overflow
        m_start -= m_adjust;		// Adjust for amount time that timer code takes to run
    else 
        m_start = 0;				// Stop must have been called directly after Start (and timer code is executed in zero time!)
}


// Split (get elapsed time, just like a stopwatch, timer does not stop)
const double PerfTimer::Elapsed()		// Returns elapsed time in seconds
{
    perftime_t time;		
    if( m_start > 0 )
    {								// Timer is still running
        uperftime_t perftime;
        GetPerfTime( perftime );
        time = (m_start - perftime) - m_adjust;
    }
    else
    {	
        time = m_start;				// Timer has stopped
    }
    //return (double)(-time) / (double)m_freq; 
    double t = (-time);
    return t * m_conversionS;
}

const double PerfTimer::Elapsedms()		// Returns elapsed time in milliseconds
{
    perftime_t time;		
    if( m_start > 0 )
    {								// Timer is still running			
        uperftime_t perftime;
        GetPerfTime( perftime );
        time = (m_start - perftime) - m_adjust;
    }
    else
    {								// Timer has stopped
        time = m_start;
    }
    //return (-time*1000.0) / (double)m_freq; 
    double t = (-time);
    return t * m_conversionMS;
}

const double PerfTimer::Elapsedus()		// Returns elapsed time in microseconds
{	
    perftime_t time;		
    if( m_start > 0 )
    {								// Timer is still running
        uperftime_t perftime;
        GetPerfTime( perftime );
        time = (m_start - perftime) - m_adjust;
    }
    else
    {								// Timer has stopped
        time = m_start;
    }
    //return (-time * 1000000.0) / (double)m_freq; 
    double t = (-time);
    return t * m_conversionUS;
}

void PerfTimer::Reset()
{
    m_start = 0; 		
}

void PerfTimer::Adjust()				// Time it takes to start and stop
{	
    m_start = 0; 
    m_adjust = 0;
    Start();						// Time a Stop
    Stop();     
    m_adjust = m_start;				// Amount of time that the timer takes 	
}

// Timer resloution
const double PerfTimer::Resolution()	// Returns timer resolution in seconds   
{	
    //double f = m_freq;
    //return 1.0 / f; 	
    return m_conversionS;
}

const double PerfTimer::Resolutionms()	// Returns timer resolution in milliseconds
{   
    //double f = m_freq;
    //return 1000.0 / f; 
    return m_conversionMS;
}

const double PerfTimer::Resolutionus()	// Returns timer resolution in microseconds
{   
    //double f = m_freq;
    //return 1000000.0 / f; 
    return m_conversionUS;
}

