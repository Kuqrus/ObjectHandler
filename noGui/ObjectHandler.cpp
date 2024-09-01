#include "ObjectList.h"

using namespace std::chrono;
int main()
{
    setlocale(0, "ru-RU.UTF-8");
    system("chcp 1251 > nul");

    std::string fileName = "objects.txt";
    //ObjectsList ol(fileName);
    ObjectsList ol("objects_ansiSingle.txt");
    ol.groupAndSort(BY_TYPE);
    ol.saveFile("out_fromAnsi.txt");
}