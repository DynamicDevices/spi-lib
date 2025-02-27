#!/usr/bin/env python3
"""
================================================================================
 file {0}
 version {1}
 Perform list of PDL sources  checks: 1) ASCI symbols only 2) No <TAB> symbol
 3) No specific words
================================================================================
"""
################################################################################
# \copyright
# Copyright 2017-2021, Cypress Semiconductor Corporation.  All rights reserved.
# You may use this file only in accordance with the license, terms, conditions,
# disclaimers, and limitations in the end user license agreement accompanying
# the software package with which this file was provided.
#
# Release notes:
#    v1.1.6:  2021-08-09 Suppress encoding check for defined extensions.
#    v1.1.5:  2021-07-12 Fixed unicode handling issues
#    v1.1.4:  2021-04-28 Avoid recursive scan of the asset root directory
#    v1.1.3:  2020-08-04 Add excludeDirs parameter to the config file
#    v1.1.2:  2020-02-26 Add config file with next parameters:
#                            folderList, typeFileToCheck, wordToCheck
#                        Remove ability to check version templates
#    v1.1.1:  2018-04-03 Update ignore list to all FreeRTOS version
#             2017-06-23 Fix error with false warning generation
#             2017-11-14 CDT 289954: Add check of XXX, FIXME, TODO
#    v1.0.0:  2017-04-25 First version:
################################################################################


__version__ = "1.1.6"
__author__  = "Valeriy Klymenko <Valeriy.Klymenko@cypress.com>"
__author__  = "Nazar Babiak <Nazar.Babiak@cypress.com>"


import os
from collections import Counter
import logging
import re
import test_syntax_config as config #config file


## Status codes
PASS    = 0
FAIL    = 1
status = {PASS    :': PASS',
          FAIL    :': FAIL'}


# Functions
################################################################################
#    Check that no file has other symbol than ASCI
#    \param pdlPath path to the file
#    \return list lines that fails check
################################################################################
def CheckEncoding(filePath):
    found = False
    failedLines = list()
    pdlFile = open(filePath, 'r', encoding='utf-8')
    tempStr = pdlFile.read()
    try:
        tempStr.encode('ascii')
    except UnicodeEncodeError:
        found = True
        pdlFile.seek(0,0)
        lineNum = 0
        for checkLine in pdlFile.readlines():
            lineNum = lineNum + 1
            try:
                checkLine.encode('ascii')
            except UnicodeEncodeError:
                failedLines.append('[{}]:{}'.format(lineNum, checkLine))
    return found, failedLines

################################################################################
#    This function check that file does not have tabs \t
#    \param filePath full path to file to check
#    \return amount of TAB in lines
################################################################################
def CheckTabs(filePath):
    with open(filePath, 'r', encoding='utf-8') as checkFile:
        fileText = checkFile.read()
    count = Counter(fileText)
    return count["\t"]

################################################################################
#    Set up logging option for module run
#    \param fileName - name and path to log file
#    \return log handler
################################################################################
def ciLogSet(fileName):
    # set up logging to file - see previous section for more details
    logging.basicConfig(level=logging.DEBUG,
                        format='%(asctime)s %(name)-12s %(levelname)-8s %(message)s',
                        datefmt='%m-%d %H:%M',
                        filename=fileName,
                        filemode='w')
    # define a Handler which writes INFO messages or higher to the sys.stderr
    console = logging.StreamHandler()
    console.setLevel(logging.INFO)
    # set a format which is simpler for console use
    formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
    # tell the handler to use this format
    console.setFormatter(formatter)
    # add the handler to the root logger
    ciLog = logging.getLogger('pdl_check')
    ciLog.addHandler(console)

    return ciLog

################################################################################
#    Check if file hase one of the words from list
#    \param fileName - name and path to the checking file
#    \param vocabulary - list of words
#    \return
#        - found - True/False,
#        - dictinary of worda with list: 0 - total number of lines with
#           founded words, 1..N lines with words
################################################################################
def CheckWordPatterns(fileName, vocabulary):
    found = False
    with open(fileName, 'r', encoding='utf-8') as f:
        results = {word:[0] for word in vocabulary}
        for num, line in enumerate(f, start=1):
            for word in vocabulary:
                if word in re.split('\W+', line.upper()): #Search at least one word in line
                    results[word].append(num)
                    results[word][0] = results[word][0] + 1
                    found = True
    return found, results

################################################################################
#    Main module routine:
#    1) Read all files
#    2) Check tab symbol in files
#    3) Check that there is no non ASCI symbols in files
#    4) Check if  files does not have words XXX, FIXME, TODO CDT 289954
#
#    \param ciLog   - logging object
#
#    \return log handler
################################################################################
def Run(ciLog):
    res = PASS
    allFileList = list()
    asciiFileList = list()

    # change work directory from built to root directory
    os.chdir(os.path.join(os.path.dirname(__file__), '..'))

    # create file list for checking
    for folder in config.folderList:
        if folder == ".":
            # special case: root directory scan is always non-recursive
            fileNames = os.listdir(folder)
            for fileName in fileNames:
                fileExt = os.path.splitext(fileName)[1]
                if fileExt in config.typeFileToCheck:
                    allFileList.append(fileName)
                    if not fileExt in config.typeFileToAcceptNonAscii:
                        asciiFileList.append(fileName)
        else:
            for dirPath, dirNames, fileNames in os.walk(folder, topdown=True):
                dirNames[:] = [dirName for dirName in dirNames if not dirName in config.excludeDirs]
                for fileName in fileNames:
                    filePath = os.path.join(dirPath, fileName)
                    fileExt = os.path.splitext(filePath)[1]
                    if fileExt in config.typeFileToCheck:
                        allFileList.append(filePath)
                        if not fileExt in config.typeFileToAcceptNonAscii:
                            asciiFileList.append(filePath)

    # Uncomment to debug the file discovery issues
    #print(allFileList)
    #print(asciiFileList)

    ciLog.info("\n\n<<<<<<<<<<          Check tabs          >>>>>>>>>>")
    for filePath in allFileList:
        tabNumber = CheckTabs(filePath)
        if (tabNumber):
            ciLog.warning("{0} has {1} tabs ".format(filePath, tabNumber))
            res = FAIL

    ciLog.info("\n\n<<<<<<<<<<        Check Encoding        >>>>>>>>>>")
    for filePath in asciiFileList:
        isFound, lineList = CheckEncoding(filePath)
        if (isFound):
            ciLog.warning("Found not ANSI symbols in file {0}:".format(filePath))
            for failedLine in lineList:
                print(failedLine.strip(' \n\r\t'))
            res = FAIL

    ciLog.info("\n\n<<<<<<<<<<      Check word patterns     >>>>>>>>>>")
    for filePath in allFileList:
        foundRes = dict()
        isFound, foundRes = CheckWordPatterns(filePath, config.wordToCheck)
        if(isFound):
            for searchWord in foundRes.keys():
                if foundRes[searchWord][0] != 0:
                    ciLog.warning("In {2}: Found word {0} in lines:{1}:".format(searchWord, foundRes[searchWord][1:], filePath))
            res = FAIL

    return res


################################################################################
def main():
    print(__doc__.format(__file__, __version__))

    os.makedirs("output", exist_ok=True)
    ciLog = ciLogSet("output/test_syntax_log.txt")
    res = Run(ciLog)
    ciLog.info("\n\nCheck status {} ".format(status[res]))
    if (res != 0):
        exit(1)

if __name__ == '__main__':
    main()
