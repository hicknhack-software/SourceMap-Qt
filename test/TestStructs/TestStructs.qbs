import qbs

CppApplication {
   name: "TestStructs"
   condition: !project.noTest

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestStructs.cpp"
   ]
}
