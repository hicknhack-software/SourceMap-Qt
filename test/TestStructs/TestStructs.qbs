CppApplication {
   name: "TestStructs"

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestStructs.cpp"
   ]
}
