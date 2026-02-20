import qbs

CppApplication {
   name: "TestExtensionInterpolation"
   condition: !project.noTest

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestExtensionInterpolation.cpp"
   ]
}
