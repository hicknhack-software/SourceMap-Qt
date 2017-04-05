import CrossPlatformUtils
import qbs
import qbs.FileInfo

Project {
    // Don't compile the unit tests
    property bool noTest: (parent  && parent.noTest !== undefined) ? parent.noTest : false

    // Don't append the compiler signature to the target name e. g. "mylib{-msvc18.40629}-d.lib"
    property bool noTargetNameCompiler: (parent && parent.noTargetNameCompiler !== undefined) ? parent.noTargetNameCompiler : false
    // Don't append the build variant to the target name e. g. "mylib-msvc18.40629{-d}.lib"
    property bool noTargetNameBuildVariant: (parent && parent.noTargetNameBuildVariant !== undefined) ? parent.noTargetNameBuildVariant : false
    // Don't put the target os in the lib install directory e. g. "lib/{windows}-x86_64/"
    property bool noLibInstallDirTargetOs: (parent && parent.noLibInstallDirTargetOs !== undefined) ? parent.noLibInstallDirTargetOs : false
    // Don't put the architecture in the lib install directory e. g. "lib/windows{-x86_64}/"
    property bool noLibInstallDirArchitecture: (parent && parent.noLibInstallDirArchitecture !== undefined) ? parent.noLibInstallDirArchitecture : false

    // Relative path where to install all library relevant files like header and libraries.
    property string installPrefix: (parent && parent.installPrefix !== undefined) ? parent.installPrefix : ""

    minimumQbsVersion: "1.6"

    qbsSearchPaths: [
        "qbs/",
    ]

    StaticLibrary {
        name: "SourceMap"
        targetName: CrossPlatformUtils.libTargetName(qbs, cpp, "sourcemap", noTargetNameCompiler, noTargetNameBuildVariant)
        version: "1.0.0"

        files: [
            "src/SourceMap/Data.h",
            "src/SourceMap/Data_impl.h",
            "src/SourceMap/Entry.h",
            "src/SourceMap/Extension/Base.h",
            "src/SourceMap/Extension/Caller.cpp",
            "src/SourceMap/Extension/Caller.h",
            "src/SourceMap/Extension/Caller_impl.h",
            "src/SourceMap/Extension/Interpolation.cpp",
            "src/SourceMap/Extension/Interpolation.h",
            "src/SourceMap/Extension/Interpolation_impl.h",
            "src/SourceMap/Extensions.h",
            "src/SourceMap/FilePosition.h",
            "src/SourceMap/intern/Base64VLQ.cpp",
            "src/SourceMap/intern/Base64VLQ.h",
            "src/SourceMap/Mapping.h",
            "src/SourceMap/Mapping_impl.h",
            "src/SourceMap/meta/tuple_index_of.h",
            "src/SourceMap/meta/tuple_without_void.h",
            "src/SourceMap/Position.h",
            "src/SourceMap/RevisionThree.cpp",
            "src/SourceMap/RevisionThree.h",
            "src/SourceMap/RevisionThree_impl.h",
        ]

        // We need multiple FileTaggers to tag header files for installation, because
        // we need subdirecties, these are not supported in qbs at the moment
        FileTagger {
            patterns: [
                "Data.h",
                "Data_impl.h",
                "Entry.h",
                "Extensions.h",
                "FilePosition.h",
                "Mapping.h",
                "Mapping_impl.h",
                "Position.h",
                "RevisionThree.h",
                "RevisionThree_impl.h",
            ]

            fileTags: [ "main-header"]
        }

        FileTagger {
            patterns: [
                "Base.h",
                "Caller.cpp",
                "Caller.h",
                "Caller_impl.h",
                "Interpolation.cpp",
                "Interpolation.h",
                "Interpolation_impl.h",
            ]

            fileTags: [ "extension-header"]
        }

        FileTagger {
            patterns: [
                "tuple_index_of.h",
                "tuple_without_void.h",
            ]

            fileTags: [ "meta-header"]
        }


        Depends { name: "Qt.core" }
        Depends { name: "cpp" }
        cpp.includePaths: [ "src/" ]
        Properties {
            condition: qbs.toolchain.contains("gcc")
            cpp.cxxFlags: ["-std=c++11"]
        }

        Group {
            name: "Install"
            qbs.install: true
            qbs.installPrefix: project.installPrefix

            Group {
                name: "Main header"
                fileTagsFilter: "main-header"
                qbs.installDir: FileInfo.joinPaths("include", product.name)
            }

            Group {
                name: "Extension header"
                fileTagsFilter: "extension-header"
                qbs.installDir: FileInfo.joinPaths("include", product.name, "Extension")
            }

            Group {
                name: "Meta header"
                fileTagsFilter: "meta-header"
                qbs.installDir: FileInfo.joinPaths("include", product.name, "meta")
            }

            Group {
                name: "Install library"
                fileTagsFilter: "staticlibrary"
                qbs.installDir: FileInfo.joinPaths("lib", CrossPlatformUtils.libDir(qbs, cpp, product.noLibInstallDirTargetOs, product.noLibInstallDirArchitecture))
            }
        }

        Group {
            name: "Other files"

            files: [
                "CHANGES",
                "LICENSE",
                "NOTICE",
                "README.md",
            ]
        }

        Export {
            Depends { name: "Qt.core" }
            Depends { name: "cpp" }
            cpp.includePaths: [ "src/" ]
            Properties {
                condition: qbs.toolchain.contains("gcc")
                cpp.cxxFlags: ["-std=c++11"]
            }
        }
    }

    references: [
        "test/test.qbs",
    ]
}
