import qbs

CppApplication {
   name: "TestMapping"
   condition: !project.noTest

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestMapping.cpp"
   ]
}
