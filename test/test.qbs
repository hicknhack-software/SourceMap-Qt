import qbs

Project {
    AutotestRunner {
        name: "SourceMap autotest-runner"
    }

    references: [
        "TestExtensionCaller/TestExtensionCaller.qbs",
        "TestExtensionCombo/TestExtensionCombo.qbs",
        "TestExtensionInterpolation/TestExtensionInterpolation.qbs",
        "TestMapping/TestMapping.qbs",
        "TestRevisionThree/TestRevisionThree.qbs",
        "TestStructs/TestStructs.qbs",
    ]
}
