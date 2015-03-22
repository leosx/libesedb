/*
 * Export functions
 *
 * Copyright (c) 2010, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <file_stream.h>
#include <memory.h>
#include <types.h>

#include "esedbtools_libcstring.h"
#include "esedbtools_libcerror.h"
#include "esedbtools_libesedb.h"
#include "esedbtools_libfdatetime.h"
#include "export.h"

/* Exports binary data
 */
void export_binary_data(
      const uint8_t *data,
      size_t data_size,
      FILE *stream )
{
	if( data != NULL )
	{
		while( data_size > 0 )
		{
			fprintf(
			 stream,
			 "%02" PRIx8 "",
			 *data );

			data      += 1;
			data_size -= 1;
		}
	}
}

/* Exports narrow character text
 * Converts \\, \t, \n, \r, \0 into their string representation
 */
void export_narrow_text(
      const char *string,
      size_t string_size,
      FILE *stream )
{
	if( string != NULL )
	{
		while( string_size > 0 )
		{
			if( *string == 0 )
			{
				if( string_size > 1 )
				{
					fprintf(
					 stream,
					 "\\0" );
				}
			}
			else if( *string == '\\' )
			{
				fprintf(
				 stream,
				 "\\\\" );
			}
			else if( *string == '\n' )
			{
				fprintf(
				 stream,
				 "\\n" );
			}
			else if( *string == '\r' )
			{
				fprintf(
				 stream,
				 "\\r" );
			}
			else if( *string == '\t' )
			{
				fprintf(
				 stream,
				 "\\t" );
			}
			else
			{
				fprintf(
				 stream,
				 "%c",
				 *string );
			}
			string      += 1;
			string_size -= 1;
		}
	}
}

/* Exports system character text
 * Converts \\, \t, \n, \r, \0 into their string representation
 */
void export_text(
      const libcstring_system_character_t *string,
      size_t string_size,
      FILE *stream )
{
	if( string != NULL )
	{
		while( string_size > 0 )
		{
			if( *string == 0 )
			{
				if( string_size > 1 )
				{
					fprintf(
					 stream,
					 "\\0" );
				}
			}
			else if( *string == (libcstring_system_character_t) '\\' )
			{
				fprintf(
				 stream,
				 "\\\\" );
			}
			else if( *string == (libcstring_system_character_t) '\n' )
			{
				fprintf(
				 stream,
				 "\\n" );
			}
			else if( *string == (libcstring_system_character_t) '\r' )
			{
				fprintf(
				 stream,
				 "\\r" );
			}
			else if( *string == (libcstring_system_character_t) '\t' )
			{
				fprintf(
				 stream,
				 "\\t" );
			}
			else
			{
				fprintf(
				 stream,
				 "%" PRIc_LIBCSTRING_SYSTEM "",
				 *string );
			}
			string      += 1;
			string_size -= 1;
		}
	}
}

/* Exports a filetime value
 * Returns 1 if successful or -1 on error
 */
int export_filetime(
     libesedb_record_t *record,
     int record_value_entry,
     uint8_t byte_order,
     FILE *record_file_stream,
     libcerror_error_t **error )
{
	libcstring_system_character_t filetime_string[ 32 ];

	libfdatetime_filetime_t *filetime = NULL;
	uint8_t *value_data               = NULL;
	static char *function             = "export_filetime";
	size_t value_data_size            = 0;
	uint32_t column_type              = 0;
	uint8_t value_data_flags          = 0;
	int result                        = 0;

	if( record == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid record.",
		 function );

		return( -1 );
	}
	if( record_file_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid record file stream.",
		 function );

		return( -1 );
	}
	if( libesedb_record_get_column_type(
	     record,
	     record_value_entry,
	     &column_type,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve column type of value: %d.",
		 function,
		 record_value_entry );

		goto on_error;
	}
	if( ( column_type != LIBESEDB_COLUMN_TYPE_BINARY_DATA )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_CURRENCY )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_INTEGER_64BIT_SIGNED ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 "",
		 function,
		 column_type );

		goto on_error;
	}
	if( libesedb_record_get_value_data_size(
	     record,
	     record_value_entry,
	     &value_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value: %d data size.",
		 function,
		 record_value_entry );

		goto on_error;
	}
	if( value_data_size > 0 )
	{
		value_data = (uint8_t *) memory_allocate(
		                          sizeof( uint8_t ) * value_data_size );

		if( value_data == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create value data.",
			 function );

			goto on_error;
		}
		if( libesedb_record_get_value_data(
		     record,
		     record_value_entry,
		     value_data,
		     value_data_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value: %d data.",
			 function,
			 record_value_entry );

			goto on_error;
		}
	}
	if( libesedb_record_get_value_data_flags(
	     record,
	     record_value_entry,
	     &value_data_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value: %d data flags.",
		 function,
		 record_value_entry );

		goto on_error;
	}
	if( ( value_data_flags & ~( LIBESEDB_VALUE_FLAG_VARIABLE_SIZE ) ) == 0 )
	{
		if( value_data != NULL )
		{
			if( value_data_size != 8 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported value data size: %" PRIzd "",
				 function,
				 value_data_size );

				goto on_error;
			}
			if( libfdatetime_filetime_initialize(
			     &filetime,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
				 "%s: unable to create filetime.",
				 function );

				goto on_error;
			}
			if( libfdatetime_filetime_copy_from_byte_stream(
			     filetime,
			     value_data,
			     value_data_size,
			     byte_order,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy byte stream to filetime.",
				 function );

				goto on_error;
			}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libfdatetime_filetime_copy_to_utf16_string(
			          filetime,
			          (uint16_t *) filetime_string,
			          32,
			          LIBFDATETIME_STRING_FORMAT_TYPE_CTIME | LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_NANO_SECONDS,
			          error );
#else
			result = libfdatetime_filetime_copy_to_utf8_string(
			          filetime,
			          (uint8_t *) filetime_string,
			          32,
			          LIBFDATETIME_STRING_FORMAT_TYPE_CTIME | LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_NANO_SECONDS,
			          error );
#endif
			if( result != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy filetime to string.",
				 function );

				goto on_error;
			}
			if( libfdatetime_filetime_free(
			     &filetime,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free filetime.",
				 function );

				goto on_error;
			}
			fprintf(
			 record_file_stream,
			 "%" PRIs_LIBCSTRING_SYSTEM "",
			 filetime_string );
		}
	}
	else
	{
		export_binary_data(
		 value_data,
		 value_data_size,
		 record_file_stream );
	}
	memory_free(
	 value_data );

	return( 1 );

on_error:
	if( filetime != NULL )
	{
		libfdatetime_filetime_free(
		 &filetime,
		 NULL );
	}
	if( value_data != NULL )
	{
		memory_free(
		 value_data );
	}
	return( -1 );
}

/* Retrieves the long value data from a long value
 * Returns 1 if successful or -1 on error
 */
int export_get_long_value_data(
     libesedb_long_value_t *long_value,
     uint8_t **long_value_data,
     size_t *long_value_data_size,
     libcerror_error_t **error )
{
	static char *function              = "export_get_long_value_data";
	size64_t safe_long_value_data_size = 0;

	if( long_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid long value.",
		 function );

		return( -1 );
	}
	if( long_value_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid long value data.",
		 function );

		return( -1 );
	}
	if( *long_value_data != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid long value data value already set.",
		 function );

		return( -1 );
	}
	if( long_value_data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid long value data size.",
		 function );

		return( -1 );
	}
	if( libesedb_long_value_get_data_size(
	     long_value,
	     &safe_long_value_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve long value data size.",
		 function );

		goto on_error;
	}
	if( safe_long_value_data_size > (size64_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid long value data size value exceeds maximum.",
		 function );

		goto on_error;
	}
	if( safe_long_value_data_size == 0 )
	{
		*long_value_data      = NULL;
		*long_value_data_size = 0;
	}
	*long_value_data = (uint8_t *) memory_allocate(
	                                sizeof( uint8_t ) * (size_t) safe_long_value_data_size );

	if( *long_value_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create long value data.",
		 function );

		goto on_error;
	}
	if( libesedb_long_value_get_data(
	     long_value,
	     *long_value_data,
	     (size_t) safe_long_value_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve long value data.",
		 function );

		goto on_error;
	}
	*long_value_data_size = (size_t) safe_long_value_data_size;

	return( 1 );

on_error:
	if( long_value_data != NULL )
	{
		memory_free(
		 long_value_data );
	}
	return( -1 );
}

