import qbs

Project {
    name: "Test"

    AutotestRunner { }

    references: [
        "TestExtensionCaller/TestExtensionCaller.qbs",
        "TestExtensionCombo/TestExtensionCombo.qbs",
        "TestExtensionInterpolation/TestExtensionInterpolation.qbs",
        "TestMapping/TestMapping.qbs",
        "TestRevisionThree/TestRevisionThree.qbs",
        "TestStructs/TestStructs.qbs",
    ]
}
