#include "ObjectList.h"

using namespace std::chrono;
int main()
{
    setlocale(0, "ru-RU.UTF-8");
    system("chcp 1251 > nul");

    std::string fileName;
    ObjectsList ol;
    int act = -1;
    int selectedSort = 0;
    while (act != 0)
    {
        std::cout << "1) Open file\n";
        std::cout << "2) Save file\n";
        std::cout << "3) Group objects\n";
        std::cout << "4) Add object\n";
        std::cout << "0) Exit\n";
        std::cin >> act;
        system("cls");
        switch (act)
        {
        case 1:
            std::cout << "Opening file...\n";
            std::cout << "Enter file name: ";
            std::cin >> fileName;
            if (!ol.readFile(fileName))
            {
                std::cout << "Error!\n";
                std::cout << "File didn't opened!\n";
                system("pause");
            }
            
            break;
        case 2:
            std::cout << "Saving file...\n";
            std::cout << "Enter file name: ";
            std::cin >> fileName;
            if (!ol.saveFile(fileName))
            {
                std::cout << "Error!\n";
                std::cout << "Couldn't create file!\n";
                system("pause");
            }
            break;
        case 3:            
            std::cout << "Select group and sort method:\n";
            std::cout << "1) By distance\n";
            std::cout << "2) By name\n";
            std::cout << "3) By type\n";
            std::cout << "4) By creation time\n";
            std::cin >> selectedSort;
            ol.groupAndSort(static_cast<SORTSTATE>(selectedSort));
            break;
        case 4:
            ol.addObject();
            break;
        case 0:
            break;
        default:
            break;
        }
        system("cls");
    }
}