/*
 *  \author  Manoel McCadden
 *  \date    05-03-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    Clarity_IO.h
 *
 *  \brief
 *    An interface for handling file io, message tracing/logging
 *    , and generic output
*/
#pragma once

// Since it is a c file we must make that clear to avoid
// unnecessary compiling errors
#include <climits>
#include <fstream>
#include <functional>
#include <iostream>
#include <cstdint>
#include <mutex>
#include <string>
#include <sstream>
#include <type_traits>

namespace ClaPP
{
typedef std::fstream File;

enum FILE_ERR
{
  FILE_NO_ERR = 0
  , FILE_NOT_FOUND
  , FILE_CANNOT_BE_OPENED
  , FILE_IS_ALREADY_OPEN
  , FILE_FAILED_TO_CLOSE
  , FILE_IS_NOT_OPEN
  , FILE_FAILED_TO_WRITE
  , FILE_FAILED_TO_READ
  , FILE_UNKOWN_ERR
};

/*!
 *  A function used to open files at a given path.
 *
 *  \param file
 *    Takes an unopened/uninitalized file and updates it with a file opened
 *    from the given path
 *  \param filePath
 *    The file path of the file being opened
 *
 *  \returns
 *    A file error result. Will return FILE_NO_ERR if none is found
 */
FILE_ERR OpenFile(File &file, const char *filePath);
/*!
 *  A function used to open files at a given path.
 *
 *  \param file
 *    Takes an unopened/uninitalized file and updates it with a file opened
 *    from the given path
 *  \param filePath
 *    The file path of the file being opened
 *
 *  \returns
 *    A file error result. Will return FILE_NO_ERR if none is found
 */
FILE_ERR OpenFile(File &file, const std::string &filePath);

/*!
 *  Writes a given message to the given file if it is open.
 *
 *  \param file
 *    The file being written to
 *  \param message
 *    The message being written to the end of the file
 *
 *  \returns
 *    A file error result. Will return FILE_NO_ERR if none is found
 */
FILE_ERR WriteToFile(File &file, const char *message);
/*!
 *  Writes a given message to the given file if it is open.
 *
 *  \param file
 *    The file being written to
 *  \param message
 *    The message being written to the end of the file
 *
 *  \returns
 *    A file error result. Will return FILE_NO_ERR if none is found
 */
FILE_ERR WriteToFile(File &file, const std::string &message);

/*!
 *  Reads the next line from a given file.
 *
 *  \param file
 *    The file that is being read
 *  \param line
 *    The string that will be updated with the read line
 *
 *  \returns
 *    A file error result. Will return FILE_NO_ERR if none is found
 */
FILE_ERR ReadLineFromFile(File &file, std::string &line);
/*!
 *  Reads the next word from a given file
 *
 *  \param file
 *    The file that is being read
 *  \param word
 *    The string that will be updated with the read word
 *
 *  \returns
 *    A file error result. Will return FILE_NO_ERR if none is found
 */
FILE_ERR ReadWordFromFile(File &file, std::string &word);

/*!
 *  Template used to read numbers 
 *  (integral or floating point types) from a given file.
 *
 *  \param file
 *    The file being read
 *  \param type
 *    The number being read into
 *
 *  \returns
 *    A file error result. Will return FILE_NO_ERR if none is found
 */
template<typename T>
FILE_ERR ReadTypeFromFile(File &file, T &type)
{
  static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value
    , "Non integral or floating point type inputed for reading");

  if(!file.is_open())
  {
    return FILE_IS_NOT_OPEN;
  }
  if(file.eof())
  {
    return FILE_NOT_FOUND;
  }

  file >> type;

  if(!file.good())
  {
    return FILE_FAILED_TO_READ;
  }

  return FILE_NO_ERR;
}

/*!
 *  Converts a given file to a string
 *
 *  \param file
 *    The file being converted
 *  \param string
 *    The string that will updated with the file's contents
 *
 *  \returns
 *    A file error result. Will return FILE_NO_ERR if none is found
 */
FILE_ERR ConvertFileToString(File &file, std::string &string);

/*!
 *  Closes a given file
 *
 *  \param file
 *    The file being closed
 *
 *  \returns
 *    A file error result. Will return FILE_NO_ERR if none is found
 */
FILE_ERR CloseFile(File &file);

enum SEVERITY
{
  SEVERITY_ALL = 0
  , SEVERITY_INFO
  , SEVERITY_WARNING
  , SEVERITY_MINOR
  , SEVERITY_MAJOR
  , SEVERITY_CRITICAL
};

/*!
 *  Prints a non-critical message out to the console
 *
 *  \param message
 *    The message being printed out to the console
 *  \param severity
 *    The severity of the message
 */
void Message(const char* message, const SEVERITY &severity);
/*!
 *  Prints a non-critical message out to the console
 *
 *  \param message
 *    The message being printed out to the console
 *  \param severity
 *    The severity of the message
 */
void Message(const std::string &message, const SEVERITY &severity);

/*!
 *  Sets the minimum severity level required for a message to be 
 *  printed.
 *
 *  \param severityLevel
 *    The minimum severityLevel required for messages to be printed
 */
void SetSeverityLevel(const SEVERITY &severityLevel);

enum ERRORCODE
{
  EC_DEFAULT = 0 //! Default undefined error
  , EC_MEMORY = 73 //! A memory related error
  , EC_LUA = 99 //! A lua related error
  , EC_INPUT = 17 //! An input related error
  , EC_LIBRARY = 11 //! A library related error
  , EC_GRAPHICS = 32 //! A graphics related error
  , EC_SHADERPROGRAM = 36 //! A shader related error
  , EC_VERTEXSHADER = 35 //! A shader related error
  , EC_FRAGMENTSHADER = 34 //! A shader related error
  , EC_GENERICSHADER = 34 //! A shader related error
  , EC_ECS //! An error with the ECS system
  , EC_PHYSICS //! An error with the physics system
};

/*!
 *  Prints an error message and error code to the console and any given 
 *  trace file. All errors are assumed to be critical and will use the 
 *  highest severity and always be printed to both
 *
 *  \param message
 *    The error messgae being printed
 *  \param errCode
 *    The error code to be printed
 */
void ErrMessage(const char *message, const ERRORCODE &errCode);
/*!
 *  Prints an error message and error code to the console and any given 
 *  trace file. All errors are assumed to be critical and will use the 
 *  highest severity and always be printed to both
 *
 *  \param message
 *    The error messgae being printed
 *  \param errCode
 *    The error code to be printed
 */
void ErrMessage(const std::string &message, const ERRORCODE &errCode);

/*!
 *  Sets the trace file that will take the output of all errors and messages.
 *  Will not override the currently open trace file if another is requested so
 *  it must be explicitly closed by the user.
 *  Clears the file on open to ensure only trace contents are viewable.
 *
 *  \param filepath
 *    The path to trace file that will be used for writting
 */
FILE_ERR SetTraceFile(const std::string &filePath);

/*!
 *  Closes the currently opened trace file if it exists.
 */
FILE_ERR CloseTraceFile();

}
