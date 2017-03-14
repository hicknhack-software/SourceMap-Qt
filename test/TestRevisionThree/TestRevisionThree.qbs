import qbs

CppApplication {
   name: "TestRevisionThree"
   condition: !parent.noTests

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestRevisionThree.cpp"
   ]
}
