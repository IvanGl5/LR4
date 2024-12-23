#include <iostream>
#include "Pipe.h"
#include "CS.h"
#include "DataManager.h"
#include "Utils.h"
#include "Logger.h"
#include "Connection.h"

std::vector<std::string> getMenuOptions = {
    "�����",
    "���� �����",
    "���� �������",
    "���� ����������",
    "�������� ��� ����� � �������",
    "��������� � ����",
    "��������� �� �����"
};

int main() {
    setlocale(LC_ALL, "Russian");
    std::string command;
    long value;
    std::string filename;
    Logger logger("/Users/Ivan/Documents/GitHub/LR4/lab4/lab4/log.txt");

    while (true) {
        displayMenu(getMenuOptions);
        getline(std::cin, command);

        value = numberOrDefault(command);

        switch (value) {
        case 1:
            logger.log("���� � ���� �����.");
            Pipe::pipeSubMenu();
            break;
        case 2:
            logger.log("���� � ���� �������.");
            CompressorStation::stationSubMenu();
            break;
        case 3:
            logger.log("���� � ���� ����������.");
            Connection::connectionSubMenu();
            break;
        case 4:
            logger.log("����� ���� ���� � �������.");
            CompressorStation::displayAll();
            Pipe::displayAll();
            break;
        case 5:
            filename = DataManager::getFullPathFromCurrentDirectory();
            logger.log("���������� ������ � ����: " + filename);
            DataManager::saveToFile(Pipe::pipes, CompressorStation::stations, Connection::connections, filename);
            break;
        case 6:
            filename = DataManager::getFullPathFromCurrentDirectory();
            logger.log("�������� ������ �� �����: " + filename);
            DataManager::loadFromFile(Pipe::pipes, CompressorStation::stations, Connection::connections, filename);
            break;
        case 0:
            logger.log("��������� ���������.");
            std::cout << "����� �� ���������.\n";
            return 0;
        default:
            logger.log("�������� �����: " + command);
            std::cout << "�������� �����. ���������� �����.\n";
            break;
        }
    }
}