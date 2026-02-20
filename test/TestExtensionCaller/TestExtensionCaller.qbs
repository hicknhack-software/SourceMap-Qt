import qbs

CppApplication {
   name: "TestExtensionCaller"
   condition: !project.noTest

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestExtensionCaller.cpp"
   ]
}
