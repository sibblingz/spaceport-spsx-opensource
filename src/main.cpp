/*  spsx 
 *  Created by Paul Miller
 *  Copyright (c) 2012 spaceport.io Inc. 
 */

#include "spsx.h"
#include "fileutil.h"
#include "perftimer.h"

#if _WIN32
#include <direct.h>
#define getcwd _getcwd 
#endif


static char szInputFilepath[2048];
static char szOutputFilepath[2048];
float fQuality = 0.999f;
bool verbose = false;

void PrintBanner()
{
    printf( "Spaceport Sound Exchange Version 1.0\n" );
    printf( "Copyright (C) spaceport.io Inc.\n" );    
}

void PrintUsage()
{
    printf( "spsx -i inputfile.mp3 -o outputfile.ogg -q 0.9\n" );
    printf( "-q Desired quality level, from -0.1 to 1.0 (lo to hi)\n" );
}


#define COMMANDLINE_EXIT        -1      // means: exit, and the Usage has already been printed
#define COMMANDLINE_NOCOMMANDS  0
#define COMMANDLINE_VALID       1
#define COMMANDLINE_INVALID     2

void PrintCommandLine( int argc, const char* argv[] )
{
    for( int i=0; i<argc; i++ )
    {
        const char* pArg = argv[i];
        if( pArg == 0 )
        {
            continue;
        }

        printf( "%s", pArg );
    }
} 

int ParseCommandLine( int argc, const char* argv[] )
{
	int result = COMMANDLINE_NOCOMMANDS;
    int validcommands = 0;
    int invalidcommands = 0;

    for( int i=1; i<argc; i++ )
    {
        const char* pArg = argv[i];
        if( pArg == 0 )
        {
            break;
        }

        switch( pArg[0] )  
        {
        case ('+'):
        case ('-'):
        //case ('/'):
            {
                
                switch( pArg[1] )
                {
                case ('?'):
                case ('h'):
                case ('H'):
                    PrintUsage();
                    result = COMMANDLINE_EXIT;
                    goto labelDoneParse;
                    i++;
                    continue;
                    break;


                case ('i'):
                case ('I'):     // input file
                    if( 0 == strcmp( "-i", pArg ) || 0 == strcmp( "-I", pArg ) )
                    {
                        const char* szParam = argv[i+1];
                        strcpy( szInputFilepath, szParam );
                        validcommands++;
                    }
                    i++;
                    continue;
                    break;
     
                case ('o'):
                case ('O'):     // output file
                    if( 0 == strcmp( "-o", pArg ) || 0 == strcmp( "-O", pArg ) )
                    {
                        const char* szParam = argv[i+1];
                        strcpy( szOutputFilepath, szParam );
                        validcommands++;
                    }
                    i++;
                    continue;
                    break;
                             
                case ('q'):
                case ('Q'):     // output file
                    if( 0 == strcmp( "-q", pArg ) || 0 == strcmp( "-Q", pArg ) )
                    {
                        const char* szParam = argv[i+1];
                        float quality = 0.999f;
                        sscanf( szParam, "%f", &quality );
                        if( quality >= -0.1f &&  quality <= 1.0 )
                        {
                            fQuality = quality;
                        }
                        validcommands++;
                    }
                    i++;
                    continue;
                    break;
						
				case ('v'):
				case ('V'):
					{
						verbose = true;
					}
					break;
                        
                        

                default:
                    printf( "I do not understand command token %d: %s\n", i, pArg );
                    invalidcommands++;			
                    break;
                }	
            }
            break;

        default:
            printf( "\nUnknown token in command line ... check token %d \n", i );
            PrintCommandLine( argc, argv );
            printf( "\n" );
            invalidcommands++;
            break;
        }
    }

	
    if( invalidcommands > 0 )
    {
       result = COMMANDLINE_INVALID; 
    }
    else if( validcommands > 0 )
    {
        result = COMMANDLINE_VALID; 
    }

labelDoneParse:
	return result;
}



int main( int argc, const char* argv[] )
{
	PerfTimer timer;
	timer.Reset();
	timer.Start();

    memset( szInputFilepath, 0, sizeof(szInputFilepath) );
    memset( szOutputFilepath, 0, sizeof(szOutputFilepath) );
    
    int parseresult = ParseCommandLine( argc, argv );
    
    if( parseresult != COMMANDLINE_EXIT )
    {
        if( strlen(szInputFilepath) > 1 && strlen(szOutputFilepath) > 1 )
        {
            if( !FileExists( szInputFilepath ) )
            {
                printf( "Error: input file %s does not exist \n", szInputFilepath );

                const int bufsize = 2048;
                char szCurrentDirectory[bufsize];
                memset( szCurrentDirectory, 0, bufsize );
                getcwd( szCurrentDirectory, bufsize );
				printf( "Current directory is %s \n", szCurrentDirectory );
            }
            else
            {
                ConvertFile( szOutputFilepath, szInputFilepath, fQuality, verbose );
            }
        }
    }

	timer.Stop();
	if(verbose)
		printf("executed in %gms\n", timer.Elapsedms());

	return 0;
}




















