CppApplication {
   name: "TestExtensionCaller"

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestExtensionCaller.cpp"
   ]
}
