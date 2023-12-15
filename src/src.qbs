import NameUtils
import qbs.FileInfo

StaticLibrary {
    name: "SourceMap"

    property bool noTargetNameCompiler: false
    property bool noTargetNameBuildVariant: false
    property bool noLibInstallDirTargetOs: false
    property bool noLibInstallDirArchitecture: false
    property string installPrefix: ""

    targetName: NameUtils.libTargetName(cpp, qbs, "sourcemap", noTargetNameCompiler, noTargetNameBuildVariant)
    version: "1.0.0"

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion: "c++20"
    cpp.includePaths: [ "./" ]

    Depends { name: "Qt.core" }

    Export {
        Depends { name: "cpp" }
        cpp.cxxLanguageVersion: "c++20"
        cpp.includePaths: [ "./" ]

        Depends { name: "Qt.core" }
    }

    files: [
        "SourceMap/Data.h",
        "SourceMap/Data_impl.h",
        "SourceMap/Entry.h",
        "SourceMap/Extension/Base.h",
        "SourceMap/Extension/Caller.cpp",
        "SourceMap/Extension/Caller.h",
        "SourceMap/Extension/Caller_impl.h",
        "SourceMap/Extension/Interpolation.cpp",
        "SourceMap/Extension/Interpolation.h",
        "SourceMap/Extension/Interpolation_impl.h",
        "SourceMap/Extensions.h",
        "SourceMap/FilePosition.h",
        "SourceMap/intern/Base64VLQ.cpp",
        "SourceMap/intern/Base64VLQ.h",
        "SourceMap/Mapping.h",
        "SourceMap/Mapping_impl.h",
        "SourceMap/meta/tuple_without_void.h",
        "SourceMap/Position.h",
        "SourceMap/RevisionThree.cpp",
        "SourceMap/RevisionThree.h",
        "SourceMap/RevisionThree_impl.h",
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

    Group {
        name: "Install"
        qbs.install: true
        qbs.installPrefix: product.installPrefix

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
            qbs.installDir: FileInfo.joinPaths("lib", NameUtils.libDir(product.cpp, product.qbs, product.noLibInstallDirTargetOs, product.noLibInstallDirArchitecture))
        }
    }

    Group {
        name: "Other files"
        prefix: "../"
        files: [
            "CHANGES",
            "LICENSE",
            "NOTICE",
            "README.md",
        ]
    }
}
