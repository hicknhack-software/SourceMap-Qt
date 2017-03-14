import qbs

CppApplication {
   name: "TestExtensionCaller"
   condition: !parent.noTests

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestExtensionCaller.cpp"
   ]
}
