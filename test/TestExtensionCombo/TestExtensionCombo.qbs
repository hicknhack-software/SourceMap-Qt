CppApplication {
   name: "TestExtensionCombo"
   condition: !project.noTest

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMap" }
   Depends { name: "Qt.testlib" }

   files: [
       "TestExtensionCombo.cpp"
   ]
}
