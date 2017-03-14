import qbs

CppApplication {
   name: "TestMapping"
   condition: !parent.noTests

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestMapping.cpp"
   ]
}
