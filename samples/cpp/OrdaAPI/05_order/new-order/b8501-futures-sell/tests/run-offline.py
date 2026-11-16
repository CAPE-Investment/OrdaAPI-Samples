"""Build b8501 Debug/Release and test against an isolated fake OrdaAPI.dll.

Run with Python 3 on a machine with VS2010 v100 and .NET 4 MSBuild installed.
All generated projects, binaries and XML assertions go to a temporary directory.
The tests never load sdk/bin DLLs or connect to a server.
"""
from pathlib import Path
import os
import subprocess
import tempfile
import xml.etree.ElementTree as ET


here = Path(__file__).resolve().parent
sample = here.parent
root = here.parents[6]
msbuild = Path(os.environ.get("WINDIR", "C:/Windows")) / "Microsoft.NET/Framework/v4.0.30319/MSBuild.exe"
work = Path(tempfile.mkdtemp(prefix="orda-b8501-test-"))
print("Isolated build/test directory", work, flush=True)


def build(project, configuration="Debug", extra_properties=()):
    subprocess.run(
        [str(msbuild), str(project), "/t:Rebuild", "/nologo", "/v:quiet",
         "/p:Platform=Win32", "/p:Configuration=" + configuration, *extra_properties],
        check=True,
    )


def create_test_project(source, name, library=False):
    project = ET.Element("Project", {
        "DefaultTargets": "Build", "ToolsVersion": "4.0",
        "xmlns": "http://schemas.microsoft.com/developer/msbuild/2003",
    })
    configurations = ET.SubElement(project, "ItemGroup", Label="ProjectConfigurations")
    configuration = ET.SubElement(configurations, "ProjectConfiguration", Include="Debug|Win32")
    ET.SubElement(configuration, "Configuration").text = "Debug"
    ET.SubElement(configuration, "Platform").text = "Win32"
    ET.SubElement(project, "Import", Project=r"$(VCTargetsPath)\Microsoft.Cpp.Default.props")
    settings = ET.SubElement(project, "PropertyGroup", Label="Configuration")
    ET.SubElement(settings, "ConfigurationType").text = "DynamicLibrary" if library else "Application"
    ET.SubElement(settings, "PlatformToolset").text = "v100"
    ET.SubElement(settings, "CharacterSet").text = "MultiByte"
    ET.SubElement(project, "Import", Project=r"$(VCTargetsPath)\Microsoft.Cpp.props")
    settings = ET.SubElement(project, "PropertyGroup")
    ET.SubElement(settings, "OutDir").text = str(work) + "\\"
    ET.SubElement(settings, "IntDir").text = str(work / ("obj-" + name)) + "\\"
    ET.SubElement(settings, "TargetName").text = name
    ET.SubElement(settings, "LinkIncremental").text = "false"
    definitions = ET.SubElement(project, "ItemDefinitionGroup")
    compile_settings = ET.SubElement(definitions, "ClCompile")
    ET.SubElement(compile_settings, "AdditionalIncludeDirectories").text = ";".join(
        str(path) for path in (root / "sdk/include", root / "sdk/structs/cpp", work)
    )
    ET.SubElement(compile_settings, "PreprocessorDefinitions").text = "WIN32;_WINDOWS;_MBCS;_DEBUG"
    ET.SubElement(compile_settings, "RuntimeLibrary").text = "MultiThreadedDebugDLL"
    ET.SubElement(compile_settings, "Optimization").text = "Disabled"
    ET.SubElement(compile_settings, "WarningLevel").text = "Level3"
    ET.SubElement(compile_settings, "ExceptionHandling").text = "Sync"
    link = ET.SubElement(definitions, "Link")
    ET.SubElement(link, "SubSystem").text = "Console"
    ET.SubElement(link, "AdditionalDependencies").text = "user32.lib;gdi32.lib"
    sources = ET.SubElement(project, "ItemGroup")
    ET.SubElement(sources, "ClCompile", Include=str(source))
    ET.SubElement(project, "Import", Project=r"$(VCTargetsPath)\Microsoft.Cpp.targets")
    path = work / (name + ".vcxproj")
    ET.ElementTree(project).write(path, encoding="utf-8", xml_declaration=True)
    return path


def cpp_string(value):
    return '"' + value.replace("\\", "\\\\").replace('"', '\\"') + '"'


service = ET.fromstring((root / "sdk/bin/sys/b8501.xml").read_bytes().decode("cp949"))

import re
header = (root / "sdk/structs/cpp/b8501.h").read_bytes().decode("cp949")
blocks = re.findall(r"typedef struct\s+\w+\s*\{(.*?)\}\s*(\w+)", header, re.S)
for (body, name), block in zip(blocks, service):
    members = re.findall(r"char\s+(\w+)\s*\[\s*(\d+)\s*\]", body)
    assert sum(int(size) for member, size in members) == sum(int(item.get("size")) for item in block)
print("XML/Header block lengths verified offline", flush=True)

codes = ET.fromstring((root / "sdk/bin/sys/codes.xml").read_bytes().decode("cp949"))
checks = ["void VerifyXmlDefaults(OrderInput &input)", "{"]
for index, field in enumerate(service.find("INBLOCK")):
    name, width = field.get("name"), int(field.get("size"))
    default = field.get("default")
    if default is not None:
        checks.append(f"    assert(memcmp(input.{name}, {cpp_string(default)}, {width}) == 0);")
checks += ["}", "void VerifyXmlCodes(OrderInput &input)", "{"]
for index, field in enumerate(service.find("INBLOCK")):
    reference = field.get("coderef")
    if reference != "PB_FORDTYPE":
        continue  # memegb is intentionally fixed to the XML buy default.
    entries = list(codes.find("CODE[@id='" + reference + "']"))
    checks.append(f"    assert(SendMessage(g_inputs[{index}], CB_GETCOUNT, 0, 0) == {len(entries)});")
    for selected, entry in enumerate(entries):
        checks += [
            f"    SendMessage(g_inputs[{index}], CB_SETCURSEL, {selected}, 0);",
            "    assert(BuildInput(input));",
            f"    assert(memcmp(input.{field.get('name')}, {cpp_string(entry.get('v'))}, {field.get('size')}) == 0);",
            f"    wchar_t label{selected}[256] = {{0}};",
            f"    SendMessageW(g_inputs[{index}], CB_GETLBTEXT, {selected}, (LPARAM)label{selected});",
            f"    assert(wcscmp(label{selected}, L{cpp_string(entry.get('d') + ' (' + entry.get('v') + ')')}) == 0);",
        ]
    checks.append(f"    SendMessage(g_inputs[{index}], CB_SETCURSEL, 0, 0);")
checks.append("}")
checks += ['void VerifyExpectedBuffer(const OrderInput &input, const char *price, const char *quantity)', '{', '    char expected[224];', "    memset(expected, ' ', sizeof(expected));"]
checks.append('    memcpy(expected + 0, "00001", 5);')
checks.append('    memcpy(expected + 5, "40", 2);')
checks.append('    memcpy(expected + 7, "ACCOUNT-TEST", 12);')
checks.append('    memcpy(expected + 27, "secret", 6);')
checks.append('    memcpy(expected + 35, "FUTURES-TEST", 12);')
checks.append('    memcpy(expected + 67, "1", 1);')
checks.append('    memcpy(expected + 68, "00", 2);')
checks.append('    memcpy(expected + 70, input.hogagb, 2);')
checks.append('    memcpy(expected + 72, "03", 2);')
checks.append('    memcpy(expected + 74, price, 13);')
checks.append('    memcpy(expected + 87, quantity, 16);')
checks.append('    memcpy(expected + 103, "11", 2);')
checks.append('    memcpy(expected + 105, "      ", 6);')
checks.append('    memcpy(expected + 111, "                    ", 20);')
checks.append('    memcpy(expected + 131, "0000000000", 10);')
checks.append('    memcpy(expected + 141, "0000000000", 10);')
checks.append('    memcpy(expected + 151, "0000000000", 10);')
checks.append('    memcpy(expected + 161, "0000000000", 10);')
checks.append('    memcpy(expected + 171, "0000000000", 10);')
checks.append('    memcpy(expected + 181, "            ", 12);')
checks.append('    memcpy(expected + 193, "         ", 9);')
checks.append('    memcpy(expected + 202, "            ", 12);')
checks.append('    memcpy(expected + 214, "000", 3);')
checks.append('    memcpy(expected + 217, "0000000", 7);')
checks += ["    assert(sizeof(input) == sizeof(expected));", "    assert(memcmp(&input, expected, sizeof(expected)) == 0);", "}"]
(work / "xml-checks.h").write_text("\n".join(checks), encoding="utf-8-sig")

solution = sample / "OrdaAPI-b8501-FuturesSell.sln"
for configuration in ("Debug", "Release"):
    output = work / configuration
    output.mkdir()
    build(solution, configuration, (
        "/p:OutDir=" + str(output) + "\\",
        "/p:IntDir=" + str(output / "obj") + "\\",
    ))
    print(configuration, "Win32 v100 MultiByte BUILD PASS", flush=True)
build(create_test_project(here / "fake-api.cpp", "OrdaAPI", library=True))
build(create_test_project(here / "harness.cpp", "b8501-harness"))
subprocess.run([str(work / "b8501-harness.exe")], cwd=work, check=True, timeout=30)
print("ALL b8501 OFFLINE CHECKS PASS", flush=True)
