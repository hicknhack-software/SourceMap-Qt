import qbs

CppApplication {
   name: "TestExtensionCombo"
   condition: !parent.noTests

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestExtensionCombo.cpp"
   ]
}
