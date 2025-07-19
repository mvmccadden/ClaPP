/*
 *  \author  Manoel McCadden
 *  \date    07-19-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    clapp_ut_fileio.cpp
 *
 *  \brief
 *    An implementation file used to define what file io unit tests
 *    are like
*/

#include "../clapp_includes/pch.h"

#include "clapp_ut_file.h"

#include "../clapp_includes/Clarity_IO.h"

namespace ClaPP_UnitTests
{
UNIT_TEST_STATUS TestFile_OpenClose()
{
  std::string filePath = "clapput.txt";
  ClaPP::File file;
  
  assert(ClaPP::OpenFile(file, filePath) == ClaPP::FILE_NO_ERR);
  assert(ClaPP::CloseFile(file) == ClaPP::FILE_NO_ERR);

  return true;
}
UNIT_TEST_STATUS TestFile_ReadWrite_Word()
{
  std::string testWord = "Test";
  std::string filePath = "clapput.txt";
  ClaPP::File file;
  
  ClaPP::OpenFile(file, filePath);
  ClaPP::WriteToFile(file, testWord);
  ClaPP::CloseFile(file);

  // Ensure that data is properly writen and save during file close/open

  ClaPP::OpenFile(file, filePath);

  std::string str;
  ClaPP::ReadWordFromFile(file, str);
  assert(str == testWord);

  ClaPP::CloseFile(file);

  return true;
}
UNIT_TEST_STATUS TestFile_ReadWrite_Line()
{
  std::string testLine = "This is a test line.";
  std::string filePath = "clapput.txt";
  ClaPP::File file;
  
  ClaPP::OpenFile(file, filePath);
  ClaPP::WriteToFile(file, testLine);
  ClaPP::CloseFile(file);

  // Ensure that data is properly writen and save during file close/open

  ClaPP::OpenFile(file, filePath);

  std::string str;
  ClaPP::ReadLineFromFile(file, str);
  assert(str == testLine);

  ClaPP::CloseFile(file);

  return true;
}
UNIT_TEST_STATUS TestFile_ReadWrite_WordLine()
{
  std::string testWord = "Test";
  std::string testLine = "This is a test line.";
  std::string filePath = "clapput.txt";
  ClaPP::File file;
  
  ClaPP::OpenFile(file, filePath);

  ClaPP::WriteToFile(file, testWord);
  ClaPP::WriteToFile(file, testLine);

  ClaPP::CloseFile(file);

  // Ensure that data is properly writen and save during file close/open
  
  ClaPP::OpenFile(file, filePath);

  std::string str;

  ClaPP::ReadWordFromFile(file, str);
  assert(str == testWord);

  ClaPP::ReadLineFromFile(file, str);
  assert(str == testLine);

  ClaPP::CloseFile(file);

  return true;
}
UNIT_TEST_STATUS TestFile_ReadWrite_LineWord()
{
  std::string testWord = "Test";
  std::string testLine = "This is a test line.";
  std::string filePath = "clapput.txt";
  ClaPP::File file;
  
  ClaPP::OpenFile(file, filePath);

  ClaPP::WriteToFile(file, testLine);
  ClaPP::WriteToFile(file, testWord);

  ClaPP::CloseFile(file);

  // Ensure that data is properly writen and save during file close/open
  
  ClaPP::OpenFile(file, filePath);

  std::string str;

  ClaPP::ReadLineFromFile(file, str);
  assert(str == testLine);

  ClaPP::ReadWordFromFile(file, str);
  assert(str == testWord);

  ClaPP::CloseFile(file);

  return true;
}
UNIT_TEST_STATUS TestFile_ReadWrite_LineWord_NoClose()
{
  std::string testWord = "Test";
  std::string testLine = "This is a test line.";
  std::string filePath = "clapput.txt";
  ClaPP::File file;
  
  ClaPP::OpenFile(file, filePath);

  // Write to the file then immedietly read from it
  ClaPP::WriteToFile(file, testLine);
  ClaPP::WriteToFile(file, testWord);

  std::string str;

  ClaPP::ReadLineFromFile(file, str);
  assert(str == testLine);

  ClaPP::ReadWordFromFile(file, str);
  assert(str == testWord);

  ClaPP::CloseFile(file);

  return true;
}
UNIT_TEST_STATUS TestFile_ReadWrite_ManyWords()
{
  std::string testWord = "Test";
  std::string testWordTwo = "TestTwo";
  std::string testWordThree = "TestThree";
  std::string filePath = "clapput.txt";
  ClaPP::File file;
  
  ClaPP::OpenFile(file, filePath);

  ClaPP::WriteToFile(file, testWord);
  ClaPP::WriteToFile(file, testWordTwo);
  ClaPP::WriteToFile(file, testWordThree);

  ClaPP::CloseFile(file);

  ClaPP::OpenFile(file, filePath);

  std::string str;

  ClaPP::ReadWordFromFile(file, str);
  assert(str == testWord);
  ClaPP::ReadWordFromFile(file, str);
  assert(str == testWordTwo);
  ClaPP::ReadWordFromFile(file, str);
  assert(str == testWordThree);

  ClaPP::CloseFile(file);

  return true;
}
UNIT_TEST_STATUS TestFile_ReadWrite_ManyLines()
{
  std::string testLine = "Test Line One";
  std::string testLineTwo = "Test Line Two";
  std::string testLineThree = "Test Line Three";
  std::string filePath = "clapput.txt";
  ClaPP::File file;
  
  ClaPP::OpenFile(file, filePath);

  ClaPP::WriteToFile(file, testLine);
  ClaPP::WriteToFile(file, testLineTwo);
  ClaPP::WriteToFile(file, testLineThree);

  ClaPP::CloseFile(file);

  ClaPP::OpenFile(file, filePath);

  std::string str;

  ClaPP::ReadLineFromFile(file, str);
  assert(str == testLine);
  ClaPP::ReadLineFromFile(file, str);
  assert(str == testLineTwo);
  ClaPP::ReadLineFromFile(file, str);
  assert(str == testLineThree);

  ClaPP::CloseFile(file);

  return true;
}
UNIT_TEST_STATUS TestFile_ReadWrite_WordsInLine()
{
  std::string testWord = "Test";
  std::string testWordTwo = "TestTwo";
  std::string testWordThree = "TestThree";
  std::string filePath = "clapput.txt";
  ClaPP::File file;
  
  ClaPP::OpenFile(file, filePath);

  // Store all words into one single string so that they are inputed
  // into a single line (with space between to distinguish words)
  ClaPP::WriteToFile(file, testWord + " " + testWordTwo + " " + testWordThree);

  ClaPP::CloseFile(file);

  ClaPP::OpenFile(file, filePath);

  std::string str;

  // Then read them individually one by one
  ClaPP::ReadWordFromFile(file, str);
  assert(str == testWord);
  ClaPP::ReadWordFromFile(file, str);
  assert(str == testWordTwo);
  ClaPP::ReadWordFromFile(file, str);
  assert(str == testWordThree);

  ClaPP::CloseFile(file);

  return true;
}
UNIT_TEST_STATUS TestFile_ReadWrite_LineMidLine()
{
  std::string testWord = "Test";
  std::string testWordTwo = "TestTwo";
  std::string testWordThree = "TestThree";
  std::string filePath = "clapput.txt";
  ClaPP::File file;
  
  ClaPP::OpenFile(file, filePath);

  // Store all words into one single string so that they are inputed
  // into a single line (with space between to distinguish words)
  ClaPP::WriteToFile(file, testWord + " " + testWordTwo + " " + testWordThree);

  ClaPP::CloseFile(file);

  ClaPP::OpenFile(file, filePath);

  std::string str;

  // Then read a single word
  ClaPP::ReadWordFromFile(file, str);
  assert(str == testWord);
  // Finally the rest of the line
  ClaPP::ReadWordFromFile(file, str);
  assert(str == testWordTwo + " " + testWordThree);

  ClaPP::CloseFile(file);

  return true;
}
}
