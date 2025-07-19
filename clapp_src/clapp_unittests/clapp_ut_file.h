/*
 *  \author  Manoel McCadden
 *  \date    07-19-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    clapp_ut_file.h
 *
 *  \brief
 *    An interface used to store all file based unit tests for ClaPP
*/
#pragma once

namespace ClaPP_UnitTests
{
typedef bool UNIT_TEST_STATUS;

/*!
 *  Open and close a file to ensure that both actions do not have any
 *  errors.
 */
UNIT_TEST_STATUS TestFile_OpenClose();
/*!
 *  Open, write a word, then close the file. Reopen the file and read
 *  the word.
 */
UNIT_TEST_STATUS TestFile_ReadWrite_Word();
/*!
 *  Open, write a line, then close the file. Reopen the file and read
 *  the line.
 */
UNIT_TEST_STATUS TestFile_ReadWrite_Line();
/*!
 *  Open, write a word and a line, then close the file. Reopen the file 
 *  and read both the word and line b2b
 */
UNIT_TEST_STATUS TestFile_ReadWrite_WordLine();
/*!
 *  Open, write a line and a word , then close the file. Reopen the file 
 *  and read both the line and word b2b. (differnt order of wordline)
 */
UNIT_TEST_STATUS TestFile_ReadWrite_LineWord();
/*!
 *  Open, write a word and a line, then read the word and line, then
 *  close the file.
 *
 *  Tests if reading immediatly after writing without closing works
 */
UNIT_TEST_STATUS TestFile_ReadWrite_LineWord_NoClose();

// TODO: Finish the following functions for unit tests

/*!
 *  Open, writes a couple words, closes the file, then reads all 
 *  written words.
 */
UNIT_TEST_STATUS TestFile_ReadWrite_ManyWords();
/*!
 *  Open, writes a couple lines , closes the file, then reads all 
 *  written lines.
 */
UNIT_TEST_STATUS TestFile_ReadWrite_ManyLines();
/*!
 *  Open, writes a lines, closes the file, then reads all 
 *  words in the line.
 */
UNIT_TEST_STATUS TestFile_ReadWrite_WordsInLine();
/*!
 *  Open, writes a lines, closes the file, then reads a word 
 *  then the rest of the line.
 */
UNIT_TEST_STATUS TestFile_ReadWrite_LineMidLine();
}
