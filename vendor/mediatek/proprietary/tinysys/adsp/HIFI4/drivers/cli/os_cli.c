/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */


 /******************************************************************************
 *
 * http://www.FreeRTOS.org/cli
 *
 ******************************************************************************/


/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "cli.h"
#include "mtk_heap.h"
#include "mt_printf.h"


#ifndef  configINCLUDE_TRACE_RELATED_CLI_COMMANDS
	#define configINCLUDE_TRACE_RELATED_CLI_COMMANDS 0
#endif

#ifndef configINCLUDE_QUERY_HEAP_COMMAND
	#define configINCLUDE_QUERY_HEAP_COMMAND 1
#endif

/*
 * Implements the task-stats command.
 */
static BaseType_t prvTaskStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Implements the run-time-stats command.
 */
#if( configGENERATE_RUN_TIME_STATS == 1 )
	static BaseType_t prvRunTimeStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
#endif /* configGENERATE_RUN_TIME_STATS */

/*
 * Implements the "query heap" command.
 */
#if( configINCLUDE_QUERY_HEAP_COMMAND == 1 )
	static BaseType_t prvQueryHeapCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
#endif

/*
 * Implements the "trace start" and "trace stop" commands;
 */
#if( configINCLUDE_TRACE_RELATED_CLI_COMMANDS == 1 )
	static BaseType_t prvStartStopTraceCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
#endif

static BaseType_t prvIORead( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvIOWrite( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvListCommands( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
#ifdef CFG_DYNAMIC_DEBUG
static BaseType_t prvSetLogLevel( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvGetLogLevel( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
#endif

/* Structure that defines the "task-stats" command line command.  This generates
a table that gives information on each task in the system. */
static const CLI_Command_Definition_t xTaskStats =
{
	"task-stats", /* The command string to type. */
	"\r\ntask-stats:\r\n Displays a table showing the state of each FreeRTOS task\r\n",
	prvTaskStatsCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

#if( configGENERATE_RUN_TIME_STATS == 1 )
	/* Structure that defines the "run-time-stats" command line command.   This
	generates a table that shows how much run time each task has */
	static const CLI_Command_Definition_t xRunTimeStats =
	{
		"run-time-stats", /* The command string to type. */
		"\r\nrun-time-stats:\r\n Displays a table showing how much processing time each FreeRTOS task has used\r\n",
		prvRunTimeStatsCommand, /* The function to run. */
		0 /* No parameters are expected. */
	};
#endif /* configGENERATE_RUN_TIME_STATS */

#if( configINCLUDE_QUERY_HEAP_COMMAND == 1 )
	/* Structure that defines the "query_heap" command line command. */
	static const CLI_Command_Definition_t xQueryHeap =
	{
		"query-heap",
		"\r\nquery-heap:\r\n Displays the free heap space, and minimum ever free heap space.\r\n",
		prvQueryHeapCommand, /* The function to run. */
		0 /* The user can enter any number of commands. */
	};
#endif /* configQUERY_HEAP_COMMAND */

#if configINCLUDE_TRACE_RELATED_CLI_COMMANDS == 1
	/* Structure that defines the "trace" command line command.  This takes a single
	parameter, which can be either "start" or "stop". */
	static const CLI_Command_Definition_t xStartStopTrace =
	{
		"trace",
		"\r\ntrace [start | stop]:\r\n Starts or stops a trace recording for viewing in FreeRTOS+Trace\r\n",
		prvStartStopTraceCommand, /* The function to run. */
		1 /* One parameter is expected.  Valid values are "start" and "stop". */
	};
#endif /* configINCLUDE_TRACE_RELATED_CLI_COMMANDS */

static const CLI_Command_Definition_t xIORead =
{
	"read",
	"\r\nread <address> <length>: \r\n Read Address (Need 4bytes align, hex value)\r\n",
	prvIORead,
	2
};

static const CLI_Command_Definition_t xIOWrite =
{
	"write",
	"\r\nwrite <address> <value>: \r\n Write Address (Need 4bytes align, hex value)\r\n",
	prvIOWrite,
	2
};

static const CLI_Command_Definition_t xListCommands =
{
	"ls",
	"\r\nls:\r\n list all available commands\r\n",
	prvListCommands,
	0
};

#ifdef CFG_DYNAMIC_DEBUG
static const CLI_Command_Definition_t xSetLogLevel =
{
	"setloglevel",
	"\r\nsetloglevel <level>:\r\n Set log level\r\n" \
	" - 0: disable log\r\n" \
	" - 1: error log\r\n" \
	" - 2: info log\r\n" \
	" - 3: warning log\r\n" \
	" - 4: debug log\r\n",
	prvSetLogLevel,
	1
};

static const CLI_Command_Definition_t xGetLogLevel =
{
	"getloglevel",
	"\r\ngetloglevel:\r\n Get log level\r\n" \
	" - 0: disable log\r\n" \
	" - 1: error log\r\n" \
	" - 2: info log\r\n" \
	" - 3: warning log\r\n" \
	" - 4: debug log\r\n",
	prvGetLogLevel,
	0
};
#endif

/*-----------------------------------------------------------*/

NORMAL_SECTION_FUNC void vRegisterOSCLICommands( void )
{
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand( &xTaskStats );

	#if( configGENERATE_RUN_TIME_STATS == 1 )
	{
		FreeRTOS_CLIRegisterCommand( &xRunTimeStats );
	}
	#endif

	#if( configINCLUDE_QUERY_HEAP_COMMAND == 1 )
	{
		FreeRTOS_CLIRegisterCommand( &xQueryHeap );
	}
	#endif

	#if( configINCLUDE_TRACE_RELATED_CLI_COMMANDS == 1 )
	{
		FreeRTOS_CLIRegisterCommand( &xStartStopTrace );
	}
	#endif
	FreeRTOS_CLIRegisterCommand( &xIORead );
	FreeRTOS_CLIRegisterCommand( &xIOWrite );
	FreeRTOS_CLIRegisterCommand( &xListCommands );

	#ifdef CFG_DYNAMIC_DEBUG
	FreeRTOS_CLIRegisterCommand( &xSetLogLevel);
	FreeRTOS_CLIRegisterCommand( &xGetLogLevel);
	#endif
}
/*-----------------------------------------------------------*/

static BaseType_t prvTaskStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *const pcHeader = "     State   Priority  Stack    #\r\n************************************************\r\n";
BaseType_t xSpacePadding;
char *pcBuffer = NULL, *pcBufferStart = NULL;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;

	pcBuffer = (char *)pvPortMalloc( ( uxTaskGetNumberOfTasks() + 2 ) * 100);
	if (!pcBuffer)
	{
		FreeRTOS_CLIPutString("allocate pcBuffer fail\r\n");
		return pdFALSE;
	}
	pcBufferStart = pcBuffer;

	/* Generate a table of task stats. */
	strcpy( pcBuffer, "Task" );
	pcBuffer += strlen( pcBuffer );

	/* Minus three for the null terminator and half the number of characters in
	"Task" so the column lines up with the centre of the heading. */
	configASSERT( configMAX_TASK_NAME_LEN > 3 );
	for( xSpacePadding = strlen( "Task" ); xSpacePadding < ( configMAX_TASK_NAME_LEN - 3 ); xSpacePadding++ )
	{
		/* Add a space to align columns after the task's name. */
		*pcBuffer = ' ';
		pcBuffer++;

		/* Ensure always terminated. */
		*pcBuffer = 0x00;
	}
	strcpy( pcBuffer, pcHeader );
	vTaskList( pcBuffer + strlen( pcHeader ) );
	FreeRTOS_CLIPutString( pcBuffer );

	vPortFree(pcBufferStart);

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}
/*-----------------------------------------------------------*/

#if( configINCLUDE_QUERY_HEAP_COMMAND == 1 )
	static BaseType_t prvQueryHeapCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
	{
		/* Remove compile time warnings about unused parameters, and check the
		write buffer is not NULL.  NOTE - for simplicity, this example assumes the
		write buffer length is adequate, so does not check for buffer overflows. */
		( void ) pcCommandString;

		FreeRTOS_CLIPrintf( "Heap(default):\r\n" );
		FreeRTOS_CLIPrintf( "\tFree: %d\r\n", ( int ) xPortGetFreeHeapSize() );
		FreeRTOS_CLIPrintf( "\tTotal: %d\r\n", configTOTAL_HEAP_SIZE );
		FreeRTOS_CLIPrintf( "\tMinimumEverFree: %d\r\n",
			( int ) xPortGetMinimumEverFreeHeapSize() );
#if defined(CFG_MTK_HEAP_SUPPORT)
		MTK_vDumpHeapStatus();
#endif
		/* There is no more data to return after this single string, so return
		pdFALSE. */
		return pdFALSE;
	}

#endif /* configINCLUDE_QUERY_HEAP */
/*-----------------------------------------------------------*/

#if( configGENERATE_RUN_TIME_STATS == 1 )

	static BaseType_t prvRunTimeStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
	{
	const char * const pcHeader = "  Abs Time      % Time\r\n****************************************\r\n";
	BaseType_t xSpacePadding;
	char *pcBuffer = NULL;

		/* Remove compile time warnings about unused parameters, and check the
		write buffer is not NULL.  NOTE - for simplicity, this example assumes the
		write buffer length is adequate, so does not check for buffer overflows. */
		( void ) pcCommandString;

		if( pcWriteBuffer == NULL )
		{
			pcBuffer = (char *)pvPortMalloc(xWriteBufferLen);
			pcWriteBuffer = pcBuffer;
		}

		/* Generate a table of task stats. */
		strcpy( pcWriteBuffer, "Task" );
		pcWriteBuffer += strlen( pcWriteBuffer );

		/* Pad the string "task" with however many bytes necessary to make it the
		length of a task name.  Minus three for the null terminator and half the
		number of characters in	"Task" so the column lines up with the centre of
		the heading. */
		for( xSpacePadding = strlen( "Task" ); xSpacePadding < ( configMAX_TASK_NAME_LEN - 3 ); xSpacePadding++ )
		{
			/* Add a space to align columns after the task's name. */
			*pcWriteBuffer = ' ';
			pcWriteBuffer++;

			/* Ensure always terminated. */
			*pcWriteBuffer = 0x00;
		}

		strcpy( pcWriteBuffer, pcHeader );
		vTaskGetRunTimeStats( pcWriteBuffer + strlen( pcHeader ) );

		FreeRTOS_CLIPutString( pcWriteBuffer );

		if (pcBuffer != NULL)
		{
			vPortFree( pcBuffer );
			pcBuffer = NULL;
		}
		/* There is no more data to return after this single string, so return
		pdFALSE. */
		return pdFALSE;
	}

#endif /* configGENERATE_RUN_TIME_STATS */
/*-----------------------------------------------------------*/

#if configINCLUDE_TRACE_RELATED_CLI_COMMANDS == 1

	static BaseType_t prvStartStopTraceCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
	{
	const char *pcParameter;
	BaseType_t lParameterStringLength;

		/* Remove compile time warnings about unused parameters, and check the
		write buffer is not NULL.  NOTE - for simplicity, this example assumes the
		write buffer length is adequate, so does not check for buffer overflows. */
		( void ) pcCommandString;
		( void ) xWriteBufferLen;
		configASSERT( pcWriteBuffer );

		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							1,						/* Return the first parameter. */
							&lParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* There are only two valid parameter values. */
		if( strncmp( pcParameter, "start", strlen( "start" ) ) == 0 )
		{
			/* Start or restart the trace. */
			vTraceStop();
			vTraceClear();
			vTraceStart();

			sprintf( pcWriteBuffer, "Trace recording (re)started.\r\n" );
		}
		else if( strncmp( pcParameter, "stop", strlen( "stop" ) ) == 0 )
		{
			/* End the trace, if one is running. */
			vTraceStop();
			sprintf( pcWriteBuffer, "Stopping trace recording.\r\n" );
		}
		else
		{
			sprintf( pcWriteBuffer, "Valid parameters are 'start' and 'stop'.\r\n" );
		}

		/* There is no more data to return after this single string, so return
		pdFALSE. */
		return pdFALSE;
	}

#endif /* configINCLUDE_TRACE_RELATED_CLI_COMMANDS */
/*-----------------------------------------------------------*/

static BaseType_t prvIORead( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcAddress, *pcLength;
BaseType_t xParameterStringLength;
unsigned int uAddress, uLength, uValue;
int i;

	( void ) pcCommandString;

	/* Obtain the parameter string. */
	pcAddress = FreeRTOS_CLIGetParameter( pcCommandString, 1, &xParameterStringLength);
	configASSERT( pcAddress );
	pcLength = FreeRTOS_CLIGetParameter( pcCommandString, 2, &xParameterStringLength);
	configASSERT( pcLength );

	if( mt_str2ul(pcAddress, &uAddress) != 0 )
	{
		FreeRTOS_CLIPutString( "unknown input address\r\n" );
		return pdFALSE;
	}
	if( mt_str2ul(pcLength, &uLength) != 0 )
	{
		FreeRTOS_CLIPutString( "unknown input length\r\n" );
		return pdFALSE;
	}

	if( uLength % 4 != 0 )
	{
		FreeRTOS_CLIPutString( "length needs 4bytes align\r\n" );
		return pdFALSE;
	}
	for( i = 0; i < uLength / 4; i++ )
	{
		uValue = *((volatile unsigned int *)uAddress + i);
		FreeRTOS_CLIPrintf( "0x%08x = 0x%08x\r\n", (uAddress + i * 4), uValue );
	}
	return pdFALSE;
}

static BaseType_t prvIOWrite( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcParamAddr, *pcParamWriteVal;
BaseType_t xParamAddrStringLength, xParamWriteValStringLength;
unsigned int uAddress, uWriteVal, uReadVal;

	( void ) pcCommandString;

	pcParamAddr = FreeRTOS_CLIGetParameter( pcCommandString, 1, &xParamAddrStringLength);
	pcParamWriteVal = FreeRTOS_CLIGetParameter( pcCommandString, 2, &xParamWriteValStringLength);

	if( mt_str2ul(pcParamAddr, &uAddress) != 0 )
	{
		FreeRTOS_CLIPutString( "unknown input address\r\n" );
		return pdFALSE;
	}
	if( mt_str2ul(pcParamWriteVal, &uWriteVal) != 0 )
	{
		FreeRTOS_CLIPutString( "unknown value\r\n" );
		return pdFALSE;
	}
	*((volatile unsigned int *)uAddress) = uWriteVal;
	uReadVal = *((volatile unsigned int *)uAddress);
	FreeRTOS_CLIPrintf( "0x%x = 0x%x\r\n", uAddress, uReadVal );
	return pdFALSE;
}
/*-----------------------------------------------------------*/

static BaseType_t prvListCommands( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const CLI_Definition_List_Item_t * pxCommand = NULL, *pxCommandsList;

	pxCommandsList = FreeRTOS_CLIGetCommandsList();
	for( pxCommand = pxCommandsList; pxCommand != NULL; pxCommand = pxCommand->pxNext )
	{
		FreeRTOS_CLIPrintf("%s\r\n", pxCommand->pxCommandLineDefinition->pcCommand);
	}
	return pdFALSE;
}
/*-----------------------------------------------------------*/

#ifdef CFG_DYNAMIC_DEBUG
static BaseType_t prvSetLogLevel( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *pcLogLevel;
BaseType_t xParamLogLevelStringLength;
unsigned int uLevel;

	( void ) pcCommandString;

	pcLogLevel = FreeRTOS_CLIGetParameter( pcCommandString, 1, &xParamLogLevelStringLength);

	if( mt_str2ul(pcLogLevel, &uLevel) != 0 )
	{
		FreeRTOS_CLIPutString( "unknown input address\r\n" );
		return pdFALSE;
	}

	set_loglevel(uLevel);
	FreeRTOS_CLIPrintf( "Set log level: %d\r\n", get_loglevel() );
	return pdFALSE;
}
/*-----------------------------------------------------------*/

static BaseType_t prvGetLogLevel( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{

	( void ) pcCommandString;

	FreeRTOS_CLIPrintf( "Get log level: %d\r\n", get_loglevel() );
	return pdFALSE;
}
#endif
