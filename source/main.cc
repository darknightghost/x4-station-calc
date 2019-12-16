#include <cstdlib>

#include <QtWidgets/QApplication>

#include <common.h>
#include <global.h>
#include <locale/string_table.h>

int main(int argc, char* argv[])
{
    int exitCode;
    int fakeArgc = 1;
    char* fakeArgv[] = {argv[0], NULL};
    QApplication app(fakeArgc, fakeArgv);

    /// Initialize
    if(Global::initialize(argc, argv, exitCode) == nullptr) {
        return exitCode;
    }

    if(StringTable::initialize() == nullptr) {
        return 1;
    }

    return 0;
}
