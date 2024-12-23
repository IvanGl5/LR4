#include "Pipe.h"
#include <iostream>
#include "Utils.h"
#include "Logger.h"
#include "Connection.h"

std::vector<std::string> getMenuOptionsPipe = {
        "�����",
        "�������� ��� �����",
        "�������� �����",
        "������������� �����",
        "������� �����",
        "����� ����",
        "�������� ��������������"
};
std::ostream& operator<<(std::ostream& out, const Pipe& pipe) {
    out << "\nPipe ID : " << pipe.getId()
        << "\nPipe name: " << pipe.name
        << "\nPipe length: " << pipe.length
        << "\nPipe diameter: " << pipe.diameter
        << "\nRepair status: " << (pipe.repairStatus ? "Yes" : "No");
    return out;
}

void Pipe::printPipeInfo(std::ostream& out) {
    logger.log("������ ���������� � ���� ������.");
    for (const auto& [id, pipe] : pipes) {
        out << pipe;
    }
}

int Pipe::nextId = 1;
std::unordered_map<int, Pipe> Pipe::pipes;

Pipe::Pipe(int id, const std::string& name, double length, int diameter, bool repairStatus)
    : id(id), name(name), length(length), diameter(diameter), repairStatus(repairStatus) {
    logger.log("������ ����� ������ Pipe � ID: " + std::to_string(id));
}

void Pipe::readFromConsole() {
    logger.log("���� � ������� readFromConsole()");
    std::cout << "������� �������� �����: ";
    std::getline(std::cin, name);
    length = inputInRange<int>("������� ����� ����� (� ��): ", 0.1, 10000);
    diameter = inputInRange<int>("������� ������� ����� (� ��): ", 10, 10000);
    repairStatus = false;
    logger.log("��������� ������ ������ ����� �� �������.");
}

void Pipe::writeToConsole() const {
    logger.log("����� ���������� ����� � �������)");
    if (!name.empty()) {
        std::cout << "\nID �����: " << id << std::endl;
        std::cout << "�������� �����: " << name << std::endl;
        std::cout << "����� ����� (� ��): " << length << std::endl;
        std::cout << "������� ����� (� ��): " << diameter << std::endl;
        std::cout << "��������� ������: " << (repairStatus ? "��" : "���") << std::endl;
    }
    else {
        std::cout << "����� �� �������." << std::endl;
    }
}

void Pipe::editRepairStatus() {
    repairStatus = !repairStatus;
    std::cout << "��������� ������ ������� ��: " << (repairStatus ? "��" : "���") << std::endl;
    logger.log("������ ������� ����� �������.");
}

void Pipe::addPipe() {
    Pipe pipe;
    pipe.setId(nextId++);
    pipe.readFromConsole();
    pipes[pipe.getId()] = pipe;
    logger.log("��������� ����� ����� � ID: " + std::to_string(pipe.getId()));
}

void Pipe::addPipeWithDiameter(int givenDiameter) {
    logger.log("�������� ����� ����� � �������� ���������.");

    if (givenDiameter < 10 || givenDiameter > 10000) {
        std::cerr << "������: ������� ����� ������ ���� � ��������� �� 10 �� 10000 ��.\n";
        logger.log("������: ������� ������������ �������.");
        return;
    }

    Pipe pipe;
    pipe.setId(nextId++);
    pipe.diameter = givenDiameter;

    std::cout << "������� �������� �����: ";
    std::getline(std::cin, pipe.name);
    pipe.length = inputInRange<int>("������� ����� ����� (� ��): ", 0.1, 10000);
    pipe.repairStatus = false;

    pipes[pipe.getId()] = pipe;

    std::cout << "����� ������� �������.\n";
    logger.log("��������� ����� ����� � ID: " + std::to_string(pipe.getId()) + " � ���������: " + std::to_string(givenDiameter) + " ��.");
}

void Pipe::displayAll() {
    logger.log("����� ���� ����");
    std::cout << "\n--- �������� ��� ����� ---\n";
    for (const auto& [id, pipe] : pipes) {
        pipe.writeToConsole();
    }
}

void Pipe::editPipeById() {
    int pipeId;
    std::cout << "������� ID ����� ��� ��������������: ";
    std::cin >> pipeId;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    auto it = pipes.find(pipeId);
    if (it != pipes.end()) {
        it->second.editRepairStatus();
        std::cout << "����� � ID " << pipeId << " ������� ���������������.\n";
        logger.log("��������������� ����� � ID: " + std::to_string(pipeId));
    }
    else {
        std::cout << "����� � ����� ID �� �������.\n";
        logger.log("����� � ID " + std::to_string(pipeId) + " �� �������.");
    }
}

void Pipe::displayPipes(const std::vector<Pipe>& pipes) {
    if (pipes.empty()) {
        std::cout << "��� ����, ��������������� �������� ������.\n";
        logger.log("��� ����, ��������������� �������� ������.");
    }
    else {
        for (const auto& pipe : pipes) {
            pipe.writeToConsole();
        }
    }
}

std::vector<Pipe> Pipe::findPipes() {
    int command;
    std::cout << "�������� �������� ������:\n";
    std::cout << "1 - ����� �� ��������\n";
    std::cout << "2 - ����� �� ������� �������\n";

    command = inputInRange<int>("������� ����� ��������: ", 1, 2);

    std::vector<Pipe> results;
    if (command == 1) {
        std::string name;
        std::cout << "������� �������� �����: ";
        std::getline(std::cin, name);
        for (const auto& [id, pipe] : pipes) {
            if (pipe.name.find(name) != std::string::npos) {
                results.push_back(pipe);
            }
        }
        logger.log("����� ���� �� �������� ��������.");
    }
    else if (command == 2) {
        bool inRepair;
        std::cout << "������� 1, ���� ������ ����� ����� � �������, 0 - ���� ���: ";
        std::cin >> inRepair;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        for (const auto& [id, pipe] : pipes) {
            if (pipe.repairStatus == inRepair) {
                results.push_back(pipe);
            }
        }
        logger.log("����� ���� �� ������� ������� ��������.");
    }

    displayPipes(results);
    return results;
}

void Pipe::deletePipeMenu() {
    int pipeId;
    std::cout << "������� ID ����� ��� ��������: ";
    std::cin >> pipeId;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    auto it = pipes.find(pipeId);
    if (it != pipes.end()) {
        pipes.erase(it);
        std::cout << "����� � ID " << pipeId << " ������� �������.\n";
        Connection::deleteConnectionsWithPipe(pipeId);
        logger.log("������� ����� � ID: " + std::to_string(pipeId));
    }
    else {
        std::cout << "����� � ����� ID �� �������.\n";
        logger.log("����� � ID " + std::to_string(pipeId) + " �� �������.");
    }
}

void Pipe::selectPipesForBatchEditOrDelete() {
    if (pipes.empty()) {
        std::cout << "��� ��������� ���� ��� ������.\n";
        logger.log("��� ��������� ���� ��� ������.");
        return;
    }

    std::cout << "������������ �����:\n";
    for (const auto& [id, pipe] : pipes) {
        std::cout << "ID: " << id << " - ";
        pipe.writeToConsole();
    }

    std::cout << "������� ID ����, ������� ������ ������� ��� �������������� ��� ��������, ����� ������: ";
    std::string input;
    std::getline(std::cin, input);
    std::stringstream ss(input);
    std::vector<int> selectedIds;
    int id;

    while (ss >> id) {
        if (pipes.find(id) != pipes.end()) {
            selectedIds.push_back(id);
        }
        else {
            std::cout << "����� � ID " << id << " �� �������.\n";
            logger.log("����� � ID " + std::to_string(id) + " �� �������.");
        }
    }

    std::vector<Pipe> selectedPipes;
    for (int id : selectedIds) {
        selectedPipes.push_back(pipes[id]);
    }

    if (!selectedPipes.empty()) {
        batchEditOrDeleteMenu(selectedPipes);
    }
    else {
        std::cout << "�� ����� ����� �� ������� ��� ���������.\n";
        logger.log("�� ����� ����� �� ������� ��� ���������.");
    }
}

void Pipe::batchEditOrDeleteMenu(const std::vector<Pipe>& pipesToEdit) {
    if (pipesToEdit.empty()) {
        std::cout << "��� ���� ��� �������������� ��� ��������.\n";
        return;
    }

    std::cout << "�������� ��������:\n";
    std::cout << "1 - ������������� ��� ��������� �����\n";
    std::cout << "2 - ������� ��� ��������� �����\n";

    int choice = inputInRange<int>("������� ����� ��������: ", 1, 2);

    if (choice == 1) {
        for (auto& pipe : pipesToEdit) {
            pipes[pipe.getId()].editRepairStatus();
        }
        std::cout << "��� ��������� ����� ������� ���������������.\n";
    }
    else if (choice == 2) {
        for (const auto& pipe : pipesToEdit) {
            pipes.erase(pipe.getId());
        }
        std::cout << "��� ��������� ����� ������� �������.\n";
    }
}

int Pipe::pipeSubMenu() {
    logger.log("���� � ������� �����.");
    std::string command;
    long value;
    while (true) {
        displayMenu(getMenuOptionsPipe);
        getline(std::cin, command);

        value = numberOrDefault(command);

        switch (value) {
        case 1:
            logger.log("����� ���� ����.");
            displayAll();
            break;
        case 2:
            logger.log("���������� ����� �����.");
            addPipe();
            break;
        case 3: {
            logger.log("�������������� ����� �� ID.");
            editPipeById();
            break;
        }
        case 4: {
            logger.log("�������� ����� �� ID.");
            deletePipeMenu();
            break;
        }
        case 5: {
            logger.log("����� ����.");
            findPipes();
            break;
        }
        case 6: {
            logger.log("����� ���� ��� ��������� �������������� ��� ��������.");
            selectPipesForBatchEditOrDelete();
            break;
        case 0:
            logger.log("����� �� ������� �����.");
            std::cout << "����� �� ���������.\n";
            return 0;
        }
        default:
            logger.log("�������� �������: " + command);
            std::cout << "�������� �������. ���������� �����.\n";
            break;
        }
    }
}