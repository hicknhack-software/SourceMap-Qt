CppApplication {
   name: "TestRevisionThree"

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestRevisionThree.cpp"
   ]
}
