#include "../include/interface.hpp"

void Interface::display() {
    system("cls");
    while (true) {
        std::cout << "Select option (type number and press enter):\n";
        std::cout << "1. View scene\n";
        std::cout << "2. View model\n";
        std::cout << "3. Exit\n\n";

        std::cout << "> ";

        int option;
        while (std::cin >> option) {
            if (std::cin.fail()) {
                std::cout << "Only integers allowed\n\n>";
                continue;
            }

            switch (option) {
                case 1:
                    chooseScene();
                    break;
                case 2:
                    chooseModel();
                    break;
                case 3:
                    return;
                default:
                    std::cout << "Option not available\n\n>";
                    break;
            }

            break;
        }
    }
}

void Interface::chooseScene() {
    system("cls");
    std::cout << "Select prefered scene/option (type number and press enter):\n";
    for (int i = 0; i < scenes.size(); ++i) {
        std::cout << i << ". " << "Test\n";
    }
    std::cout << scenes.size() << ". " << "Exit\n\n";

    std::cout << "> ";

    int option;
    while (std::cin >> option) {
        if (std::cin.fail()) {
            std::cout << "Only integers allowed\n\n>";
            continue;
        }

        if (option < 0 || option >= scenes.size()) {
            std::cout << "Option not available\n\n>";
            continue;
        }

        if (option == 0) {
            system("cls");
            return;
        }

        std::cout << "Drawing scene..\n";
        system("cls");
        return;
    }
}

void Interface::chooseModel() {
    system("cls");
    std::cout << "Enter file name/path (must be inside and relative to the assets folder), or send empty string to exit:\n\n";

    std::cout << "> ";

    std::string modelPath;
    std::cin >> modelPath;

    if (modelPath.empty()) {
        system("cls");
        return;
    }
}