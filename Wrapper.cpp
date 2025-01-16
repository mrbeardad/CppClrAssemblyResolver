#include "Wrapper.h"

#include <string>

#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace System::Reflection;

#define ASSEMBLY_NAME_REQUIRED_RESOLVE "MyManaged.Core"

static std::wstring DllDir;

static Assembly^ OnAssemblyResolve(Object^ sender, ResolveEventArgs^ args) {
    if (!args->Name->StartsWith( ASSEMBLY_NAME_REQUIRED_RESOLVE",")) {
        Console::WriteLine("Assembly not found: " + args->Name);
        return nullptr;
    }

    String^ pathToAssembly = msclr::interop::marshal_as<String^>(DllDir) + ASSEMBLY_NAME_REQUIRED_RESOLVE".dll";
    return Assembly::LoadFrom(pathToAssembly);
}

static void InitAssemblyResolveHandler(std::wstring dir)
{
    if (DllDir.empty()) {
        DllDir = dir;
        AppDomain::CurrentDomain->AssemblyResolve += gcnew ResolveEventHandler(&OnAssemblyResolve);
    }
}

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

class AssemblyResolver {
public:
    static AssemblyResolver instance;

    AssemblyResolver()
    {
        // Use __ImageBase case the code cannot be compiled as managed, thus extract some logic here
        std::wstring path(MAX_PATH, L'\0');
        ::GetModuleFileNameW((HMODULE)&__ImageBase, path.data(), path.length());
        if (path.starts_with(L"\\\\?\\")) {
            path = path.substr(4);
        }
        auto prefix = path.rfind(L'\\');
        if (prefix == std::wstring::npos) {
            path.resize(prefix + 1);
            InitAssemblyResolveHandler(path);
        }
    }
};
// Global consturctor that be called while loading
AssemblyResolver AssemblyResolver::instance = AssemblyResolver();

using namespace System;

void CallClrMethod() {
    Console::WriteLine("Hello World");
}
