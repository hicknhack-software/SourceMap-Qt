CppApplication {
   name: "TestExtensionInterpolation"

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestExtensionInterpolation.cpp"
   ]
}
