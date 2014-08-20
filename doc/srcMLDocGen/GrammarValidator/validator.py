#!/usr/bin/python

import os, sys, string, os.path, subprocess, glob, re
import lxml.etree as ET


srcMLGrammar = "Grammar/srcML.rnc"
convertedGrammar = "srcML.rng"
trang = "trang"
uriToPrefix = {
    "http://www.sdml.info/srcML/operator": "op",
    "http://www.sdml.info/srcML/src": "src",
    "http://www.sdml.info/srcML/cpp": "cpp",
    "http://www.sdml.info/srcML/literal": "lit",
    "http://www.sdml.info/srcML/modifier": "type",
}


trangProcess = subprocess.Popen([trang, "-I", "rnc", "-O", "rng", srcMLGrammar, convertedGrammar])
trangProcess.wait()

grammar = ET.RelaxNG(file=convertedGrammar)

objectiveCExt = re.compile(r".*\.m\.xml$")


out = open("ValidationStatus.txt", "w")
passCount = 0
failCount = 0
faildToLoadCount = 0
index = 0
failedFileList = []

testXMLLocation = "TestXML"
xmlFiles = glob.glob(os.path.join(testXMLLocation, "*.xml"))
print "Processing: ", testXMLLocation
for xmlFile in xmlFiles:
    if (index % 1000) == 0:
        print "Processed", index
    try:
        if grammar(ET.parse(xmlFile)):
            out.write("Passed: {0}\n".format(xmlFile))
            passCount += 1
        else:
            out.write("Failed: {0}\n".format(xmlFile))
            failCount += 1
            failedFileList.append(xmlFile)
    except Exception as e:
        print "Caught Exception: ", e
        out.write("Failed (Exception): {0}\n".format(xmlFile))
        faildToLoadCount += 1
    index += 1
finalTotals = "Totals\n    Passed: {0}\n    Failed: {1}\n    Failed To Load: {3}\n    Total: {2}".format(passCount, failCount, index, faildToLoadCount)
print finalTotals



testSuiteLocation = "../../../test/suite"
xmlFiles = glob.glob(os.path.join(testSuiteLocation, "*.xml"))
print "Processing: ", testSuiteLocation
for xmlFile in xmlFiles:
    if objectiveCExt.match(xmlFile) != None:
        continue
    if (index % 1000) == 0:
        print "Processed", index
    try:
        doc = ET.parse(xmlFile)
        if grammar(doc):
            out.write("Passed: {0}\n".format(xmlFile))
            passCount += 1
        else:

            troubleElements = doc.xpath("/src:unit/src:unit/child::*|/src:unit/child::*", namespaces={"src": "http://www.sdml.info/srcML/src"})
            out.write("Failed: {0}\n".format(xmlFile))
            failCount += 1
            troubleElemFormat = ", ".join(set([("<" + (x.prefix + ":" if x.prefix != None else "") + ET.QName(x).localname + ">") for x in troubleElements]))
            failedFileList.append(xmlFile +"  " + troubleElemFormat)
    except Exception as e:
        print "Caught Exception: ", e
        out.write("Failed (Exception): {0}\n".format(xmlFile))
        faildToLoadCount += 1
    index += 1
finalTotals = "Totals\n    Passed: {0}\n    Failed: {1}\n    Failed To Load: {3}\n    Total: {2}".format(passCount, failCount, index, faildToLoadCount)
print finalTotals
out.write(finalTotals + "\n")

srcMLExec = "srcml"

def processSourceCode(directory, globExpr, srcMLLanguage):
    global out
    global passCount
    global failCount
    global faildToLoadCount
    global index
    global failedFileList
    print "processing: ", directory
    sourceFiles = glob.glob(os.path.join(directory, globExpr))
    for srcFile in sourceFiles:
        srcMLProc = subprocess.Popen([srcMLExec, "--language", srcMLLanguage, srcFile], stdout=subprocess.PIPE)
        try:
            if grammar(ET.ElementTree(ET.fromstringlist(srcMLProc.stdout.readlines()))):
                out.write("Passed: {0}\n".format(srcFile))
                passCount += 1
            else:
                out.write("Failed: {0}\n".format(srcFile))
                failCount += 1
                failedFileList.append(srcFile)
        except Exception as e:
            print "Caught Exception: ", e
            out.write("Failed (Exception): {0}\n".format(srcFile))
            faildToLoadCount += 1
        index += 1
    finalTotals = "Totals\n    Passed: {0}\n    Failed: {1}\n    Failed To Load: {3}\n    Total: {2}".format(passCount, failCount, index, faildToLoadCount)
    print finalTotals

processSourceCode("../DocData/C", "*.c", "C")
processSourceCode("../DocData/CPreProc", "*.(cpp|c|txt)", "C++")
processSourceCode("../DocData/CPlusPlus", "*.cpp", "C++")
processSourceCode("../DocData/CS", "*.cs", "C#")
processSourceCode("../DocData/Java", "*.java", "Java")

out.write("\n\nFailed Files\n\n")
for f in failedFileList:
    out.write("{0}\n".format(f))
out.close()