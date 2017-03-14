import qbs
import qbs.FileInfo

Project {
    property bool noTests: true

    StaticLibrary {
        name: "SourceMap"
        targetName: name + (qbs.buildVariant.contains("debug") ? "-d" :"")
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


        Depends { name: "Qt"; submodules: ["core"] }

        Depends { name: "cpp" }
        cpp.includePaths: [ "src/" ]

        Group {
            name: "Install Main Header"
            fileTagsFilter: ["main-header"]

            qbs.install: true
            qbs.installDir: FileInfo.joinPaths("include", product.name)
        }

        Group {
            name: "Install Extension Header"
            fileTagsFilter: ["extension-header"]

            qbs.install: true
            qbs.installDir: FileInfo.joinPaths("include", product.name, "Extension")
        }

        Group {
            name: "Install Meta Header"
            fileTagsFilter: ["meta-header"]

            qbs.install: true
            qbs.installDir: FileInfo.joinPaths("include", product.name, "meta")
        }

        Group {
            name: "Install Library"
            fileTagsFilter: "staticlibrary"

            qbs.install: true
            qbs.installDir: FileInfo.joinPaths("lib")
        }

        Export {
            Depends { name: "cpp" }
            cpp.includePaths: [ "src/" ]
        }
    }

    references: [
        "test/test.qbs",
    ]

    Product {
        name: "Other Files"

        files: [
            "CHANGES",
            "LICENSE",
            "NOTICE",
            "README.md",
        ]
    }
}
