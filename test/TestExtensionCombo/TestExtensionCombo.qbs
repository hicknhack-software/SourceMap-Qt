import qbs

CppApplication {
   name: "TestExtensionCombo"
   condition: !noTest

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestExtensionCombo.cpp"
   ]
}
