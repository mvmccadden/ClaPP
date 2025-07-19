/*
 *  \author  Manoel McCadden
 *  \date    07-18-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    Clarity_IO.cpp
 *
 *  \brief
 *    An interface for handling file io, message tracing/logging
 *    , and generic output
*/

#include "clapp_includes/pch.h"

#include "clapp_includes/Clarity_IO.h"

using namespace std;

namespace ClaPP
{
struct TraceLogging
{
  // Used to store the default buffer of cout
  streambuf *coutBuffer;
  File file;
  SEVERITY minimumSeverity;
} static logger;

FILE_ERR OpenFile(File &file, const char *filePath)
{
  if(file.is_open())
  {
    return FILE_IS_ALREADY_OPEN;
  }

  file.open(filePath);
  if(!file.is_open())
  {
    return FILE_CANNOT_BE_OPENED;
  }

  return FILE_NO_ERR;
}

FILE_ERR OpenFile(File &file, const string &filePath)
{
  // Explicitly defined as part of ClaPP to help the compiler decipher
  // what is being called more easily since we are overloading the functions
  return ClaPP::OpenFile(file, filePath.c_str());
}

FILE_ERR WriteToFile(File &file, const char *message)
{
  if(!file.is_open())
  {
    return FILE_IS_NOT_OPEN;
  }

  file << message << std::endl;

  return FILE_NO_ERR;
}

FILE_ERR WriteToFile(File &file, const std::string &message)
{
  return ClaPP::WriteToFile(file, message.c_str());
}

FILE_ERR ReadLineFromFile(File &file, std::string &line)
{
  if(!file.is_open())
  {
    return FILE_IS_NOT_OPEN;
  }
  if(file.eof())
  {
    return FILE_NOT_FOUND;
  }
  
  // Remove any existing data from the line, then read into it
  line = "";
  std::getline(file, line);

  return FILE_NO_ERR;
}

FILE_ERR ReadWordFromFile(File &file, std::string &word)
{
  if(!file.is_open())
  {
    return FILE_IS_NOT_OPEN;
  }
  if(file.eof())
  {
    return FILE_NOT_FOUND;
  }
  
  // Reset the given word to ensure it is only storing the word
  word = "";
  file >> word;
  
  // Check that the file read correctly
  if(!file.good())
  {
    return FILE_FAILED_TO_READ;
  }

  return FILE_NO_ERR;
}

FILE_ERR ConvertFileToString(File &file, string &string)
{
  if(!file.is_open())
  {
    return FILE_IS_NOT_OPEN;
  }
  if(file.eof())
  {
    return FILE_NOT_FOUND;
  }

  std::stringstream stream; 
  stream << file.rdbuf();
  string = stream.str();

  if(string.empty())
  {
    return FILE_FAILED_TO_READ;
  }

  return FILE_NO_ERR;
}

FILE_ERR CloseFile(File &file)
{
  if(!file.is_open())
  {
    return FILE_IS_NOT_OPEN;
  }

  file.close();
  return FILE_NO_ERR;
}

void Message(const char* message, const SEVERITY &severtiy)
{
  std::cout << message << std::endl;
}

void Message(const string &message, const SEVERITY &severity)
{
  ClaPP::Message(message.c_str(), severity);
}

void SetSeverityLevel(const SEVERITY &severityLevel)
{

}

void ErrMessage(const char *message, const ERRORCODE &errCode)
{
  if(logger.file.is_open())
  {
    cout << "ERROR: " << errCode << ", " << message << endl;
  }

  cerr << "ERROR: " << errCode << ", " << message << endl;
}

void ErrMessage(const string &message, const ERRORCODE &errCode)
{
  ClaPP::ErrMessage(message.c_str(), errCode);
}

FILE_ERR SetTraceFile(const string &filePath)
{
  FILE_ERR err = ClaPP::OpenFile(logger.file, filePath);
  if(err == ClaPP::FILE_NO_ERR)
  {
    // Save the cout buffer for restoration later and then
    // set the cout buffer to send to the trace file's buffer
    logger.coutBuffer = cout.rdbuf(logger.file.rdbuf());
  }

  return err;
}

FILE_ERR CloseTraceFile()
{
  FILE_ERR err = ClaPP::CloseFile(logger.file);
  if(err == ClaPP::FILE_NO_ERR)
  { 
    // Reset cout to it's original buffer on file close
    cout.rdbuf(logger.coutBuffer);
  }

  return err;
}
};
