#include "Connection.h"
#include "Pipe.h"
#include "CS.h"
#include "Utils.h"
#include "Logger.h"
#include "vector"
#include <stack>
#include <unordered_set>
#include <queue>

int Connection::nextId = 1;
std::unordered_map<int, Connection> Connection::connections;

Connection::Connection(int id, int pipeId, int stationId1, int stationId2)
    : id(id), pipeId(pipeId), stationId1(stationId1), stationId2(stationId2) {
    logger.log("������ ����� ������ Connection � ID: " + std::to_string(id));
}

std::unordered_map<int, std::vector<int>> Connection::adjListOut;
std::unordered_map<int, std::vector<int>> Connection::adjListIn;
std::unordered_map<int, int> Connection::pipeCapacity;

void Connection::addConnection() {
    logger.log("���������� ������ ����������.");

    int stationId1 = -1, stationId2 = -1;
    int desiredDiameter = 0;

    while (true) {
        std::cout << "������� ID ������� ������� ��� ���������� (��� 0 ��� ������): ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "0") {
            std::cout << "������ ���������� ����������.\n";
            logger.log("������ ���������� ���������� �������������.");
            return;
        }

        stationId1 = numberOrDefault(input);
        if (CompressorStation::stations.find(stationId1) == CompressorStation::stations.end()) {
            std::cout << "������� � ����� ID �� �������.\n";
            logger.log("������: ������� � ID " + std::to_string(stationId1) + " �� �������.");
            continue;
        }

        break;
    }

    while (true) {
        std::cout << "������� ID �������� ������� ��� ���������� (��� 0 ��� ������): ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "0") {
            std::cout << "������ ���������� ����������.\n";
            logger.log("������ ���������� ���������� �������������.");
            return;
        }

        stationId2 = numberOrDefault(input);
        if (CompressorStation::stations.find(stationId2) == CompressorStation::stations.end()) {
            std::cout << "������� � ����� ID �� �������.\n";
            logger.log("������: ������� � ID " + std::to_string(stationId2) + " �� �������.");
            continue;
        }

        if (stationId1 == stationId2) {
            std::cout << "������ ��������� ������� ���� � �����. �������� ������ �������.\n";
            logger.log("������: ������� ��������� ������� � ����� �����.");
            continue;
        }

        break;
    }

    while (true) {
        while (true) {
            std::vector<int> acceptableDiameters = { 500, 700, 1000, 1400 };

            std::cout << "������� ������� ����� ��� ���������� (500, 700, 1000, 1400 ��) ��� 0 ��� ������: ";
            desiredDiameter = inputInRange<int>("", 0, 1400);

            if (desiredDiameter == 0) {
                std::cout << "������ ���������� ����������.\n";
                logger.log("������ ���������� ���������� �������������.");
                return;
            }

            if (std::find(acceptableDiameters.begin(), acceptableDiameters.end(), desiredDiameter) == acceptableDiameters.end()) {
                std::cout << "������������ �������. �������� �� 500, 700, 1000, 1400 ��.\n";
                continue;
            }

            break;
        }

        while (true) {
            int pipeId = -1;
            for (const auto& [id, pipe] : Pipe::pipes) {
                bool pipeInUse = false;
                for (const auto& [connId, conn] : connections) {
                    if (conn.getPipeId() == id) {
                        pipeInUse = true;
                        break;
                    }
                }
                if (!pipeInUse && pipe.getDiameter() == desiredDiameter && !pipe.getStatus()) {
                    pipeId = id;
                    break;
                }
            }

            if (pipeId == -1) {
                std::cout << "��� ��������� ���� � ��������� " << desiredDiameter << " ��.\n";
                std::cout << "�������� ��������:\n";
                std::cout << "1 - ������� ����� �����\n";
                std::cout << "2 - ������� ������ �������\n";
                std::cout << "0 - ������\n";
                int choice = inputInRange<int>("", 0, 2);
                if (choice == 1) {
                    std::cout << "�������� ����� �����.\n";
                    Pipe::addPipeWithDiameter(desiredDiameter);
                    continue;
                }
                else if (choice == 2) {
                    break;
                }
                else {
                    std::cout << "���������� ���������� ��������.\n";
                    logger.log("���������� ���������� �������� ������������� ��� ���������� ���������� �����.");
                    return;
                }
            }
            else {
                std::cout << "�� ����������� ������� ���������� ����� �������� " << stationId1 << " � �������� " << stationId2
                    << " � �������������� ����� " << pipeId << " (�������: " << desiredDiameter << " ��).\n";
                std::cout << "����������� �������� (1 - ��, 0 - ������): ";
                int confirm = inputInRange<int>("", 0, 1);
                if (confirm == 1) {
                    Connection conn(nextId++, pipeId, stationId1, stationId2);
                    connections[conn.getId()] = conn;

                    adjListOut[stationId1].push_back(stationId2);
                    adjListIn[stationId2].push_back(stationId1);

                    std::cout << "���������� ������� ��������� � ID: " << conn.getId() << std::endl;
                    logger.log("��������� ����� ���������� � ID: " + std::to_string(conn.getId()));

                    return;
                }
                else {
                    std::cout << "���������� ���������� �������� �������������.\n";
                    logger.log("���������� ���������� �������� ������������� �� ����� �������������.");
                    return;
                }
            }
        }
    }
}

void Connection::displayAllConnections() {
    logger.log("����� ���� ����������.");
    if (connections.empty()) {
        std::cout << "��� ���������� ��� �����������.\n";
    }
    else {
        for (const auto& [id, conn] : connections) {
            conn.writeToConsole();
        }
    }
}

void Connection::deleteConnectionsWithPipe(int pipeId) {
    logger.log("�������� ����������, ��������� � ������ ID: " + std::to_string(pipeId));

    std::vector<int> connectionsToDelete;
    for (const auto& [id, conn] : connections) {
        if (conn.getPipeId() == pipeId) {
            connectionsToDelete.push_back(id);
        }
    }

    for (int id : connectionsToDelete) {
        connections.erase(id);
        logger.log("������� ���������� � ID: " + std::to_string(id));
    }

    for (auto& [key, neighbors] : adjListOut) {
        neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), pipeId), neighbors.end());
    }

    for (auto& [key, neighbors] : adjListIn) {
        neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), pipeId), neighbors.end());
    }

    logger.log("�������� ����� � ID: " + std::to_string(pipeId) + " ���������.");
    std::cout << "����� � ��� ��������� ���������� ������� �������.\n";
}

void Connection::deleteConnectionsWithStation(int stationId) {
    logger.log("�������� ���������� �� �������� ID: " + std::to_string(stationId));

    std::vector<int> connectionsToDelete;
    for (const auto& [id, conn] : connections) {
        if (conn.getStationId1() == stationId || conn.getStationId2() == stationId) {
            connectionsToDelete.push_back(id);
        }
    }
    for (int id : connectionsToDelete) {
        connections.erase(id);
        logger.log("������� ���������� � ID: " + std::to_string(id));
    }

    for (auto& [key, neighbors] : adjListOut) {
        neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), stationId), neighbors.end());
    }
    adjListOut.erase(stationId);

    for (auto& [key, neighbors] : adjListIn) {
        neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), stationId), neighbors.end());
    }
    adjListIn.erase(stationId);

    std::cout << "������� � ��� ��������� ���������� ������� �������.\n";
}

void Connection::writeToConsole() const {
    std::cout << "���������� ID: " << id << "\n";
    std::cout << "����� ID: " << pipeId << "\n";
    std::cout << "������� 1 ID: " << stationId1 << "\n";
    std::cout << "������� 2 ID: " << stationId2 << "\n";
    auto it = Pipe::pipes.find(pipeId);
    if (it != Pipe::pipes.end()) {
        const Pipe& pipe = it->second;
        std::cout << "������� �����: " << pipe.getDiameter() << " ��\n";
        std::cout << "����� �����: " << pipe.getLength() << " �\n";
    }
    else {
        std::cout << "����� � ID " << pipeId << " �� �������.\n";
    }
}

void Connection::deleteConnection() {
    logger.log("�������� ����������.");

    if (connections.empty()) {
        std::cout << "��� ���������� ��� ��������.\n";
        logger.log("��� ���������� ��� ��������.");
        return;
    }

    std::cout << "������ ��������� ����������:\n";
    for (const auto& [id, conn] : connections) {
        std::cout << "ID ����������: " << id << ", ����� ID: " << conn.getPipeId()
            << ", ������� 1 ID: " << conn.getStationId1()
            << ", ������� 2 ID: " << conn.getStationId2() << "\n";
    }

    std::cout << "������� ID ���������� ��� �������� (��� 0 ��� ������): ";
    int connectionId = inputInRange<int>("", 0, nextId - 1);

    if (connectionId == 0) {
        std::cout << "������ �������� ����������.\n";
        logger.log("������ �������� ���������� �������������.");
        return;
    }

    auto it = connections.find(connectionId);
    if (it != connections.end()) {
        connections.erase(it);
        std::cout << "���������� � ID " << connectionId << " ������� �������.\n";
        logger.log("���������� � ID " + std::to_string(connectionId) + " �������.");
    }
    else {
        std::cout << "���������� � ����� ID �� �������.\n";
        logger.log("���������� � ID " + std::to_string(connectionId) + " �� �������.");
    }
}

void Connection::dfsReachableFromStart(int station, const std::unordered_map<int, std::vector<int>>& adjListOut, std::unordered_set<int>& visited) {
    visited.insert(station);
    if (adjListOut.find(station) != adjListOut.end()) {
        for (int next : adjListOut.at(station)) {
            if (visited.find(next) == visited.end()) {
                dfsReachableFromStart(next, adjListOut, visited);
            }
        }
    }
}

void dfsReachableToEnd(int station, const std::unordered_map<int, std::vector<int>>& adjListIn, std::unordered_set<int>& visited) {
    visited.insert(station);
    if (adjListIn.find(station) != adjListIn.end()) {
        for (int prev : adjListIn.at(station)) {
            if (visited.find(prev) == visited.end()) {
                dfsReachableToEnd(prev, adjListIn, visited);
            }
        }
    }
}

bool topologicalSortUtil(int station,
    const std::unordered_map<int, std::vector<int>>& adjList,
    std::unordered_set<int>& visited,
    std::unordered_set<int>& recursionStack,
    std::vector<int>& result) {
    visited.insert(station);
    recursionStack.insert(station);

    if (adjList.find(station) != adjList.end()) {
        for (int next : adjList.at(station)) {
            if (visited.find(next) == visited.end()) {
                if (!topologicalSortUtil(next, adjList, visited, recursionStack, result)) {
                    return false;
                }
            }
            else if (recursionStack.find(next) != recursionStack.end()) {
                return false;
            }
        }
    }

    recursionStack.erase(station);
    result.push_back(station);
    return true;
}

void Connection::topologicalSortMenu() {
    logger.log("������� ���� �������������� ���������� �������.");

    if (CompressorStation::stations.empty()) {
        std::cout << "��� ������� ��� �������������� ����������.\n";
        logger.log("������: ��� �������.");
        return;
    }

    int startStation, endStation;
    while (true) {
        std::cout << "������� ID ��������� ������� (��� 0 ��� ������): ";
        std::string input;
        std::getline(std::cin, input);
        startStation = numberOrDefault(input);
        if (startStation == 0) {
            std::cout << "������ �������������� ����������.\n";
            logger.log("������ �������������� ���������� �������������.");
            return;
        }
        if (CompressorStation::stations.find(startStation) == CompressorStation::stations.end()) {
            std::cout << "������� �� �������. ��������� ����.\n";
            continue;
        }
        break;
    }

    while (true) {
        std::cout << "������� ID �������� ������� (��� 0 ��� ������): ";
        std::string input;
        std::getline(std::cin, input);
        endStation = numberOrDefault(input);
        if (endStation == 0) {
            std::cout << "������ �������������� ����������.\n";
            logger.log("������ �������������� ���������� �������������.");
            return;
        }
        if (CompressorStation::stations.find(endStation) == CompressorStation::stations.end()) {
            std::cout << "������� �� �������. ��������� ����.\n";
            continue;
        }
        break;
    }

    topologicalSort(startStation, endStation);
}

void Connection::topologicalSort(int startStation, int endStation) {
    logger.log("������ �������������� ���������� �� �������� ������: " + std::to_string(startStation) +
        " � �������� �����: " + std::to_string(endStation));

    std::unordered_set<int> reachableFromStart;
    dfsReachableFromStart(startStation, adjListOut, reachableFromStart);

    std::unordered_set<int> reachableToEnd;
    dfsReachableToEnd(endStation, adjListIn, reachableToEnd);

    std::unordered_set<int> subgraphStations;
    for (int st : reachableFromStart) {
        if (reachableToEnd.find(st) != reachableToEnd.end()) {
            subgraphStations.insert(st);
        }
    }

    if (subgraphStations.empty()) {
        std::cout << "�� ������� ����� �� ������� " << startStation << " � ������� " << endStation << ".\n";
        logger.log("�������������� ����������: ���� �� �������.");
        return;
    }

    std::unordered_map<int, std::vector<int>> subgraph;
    for (int st : subgraphStations) {
        if (adjListOut.find(st) != adjListOut.end()) {
            std::vector<int> filtered;
            for (int nxt : adjListOut.at(st)) {
                if (subgraphStations.find(nxt) != subgraphStations.end()) {
                    filtered.push_back(nxt);
                }
            }
            if (!filtered.empty()) {
                subgraph[st] = filtered;
            }
        }
    }

    std::unordered_set<int> visited;
    std::unordered_set<int> recursionStack;
    std::vector<int> result;

    for (int st : subgraphStations) {
        if (visited.find(st) == visited.end()) {
            if (!topologicalSortUtil(st, subgraph, visited, recursionStack, result)) {
                std::cout << "�������������� ���������� ����������, ��������� ���� � ��������� ��������.\n";
                logger.log("�������������� ����������: ��������� ����.");
                return;
            }
        }
    }

    std::reverse(result.begin(), result.end());

    std::cout << "������������� ��������������� ������� ������� (������� �� " << startStation << " �� " << endStation << "):\n";
    for (int st : result) {
        std::cout << st << " ";
    }
    std::cout << "\n";

    logger.log("�������������� ���������� ������� ���������.");
}

double calculatePipePerformance(const Pipe& pipe) {
    if (pipe.getStatus()) {
        return 0.0;
    }
    double diameter = pipe.getDiameter() * 0.001;
    double length = pipe.getLength();

    if (length == 0) {
        throw std::runtime_error("����� ����� �� ����� ���� ����� 0.");
    }

    return std::sqrt(std::pow(diameter, 5) / length * 1000.0) * 1.8;
}

 double Connection::fordFulkerson(int source, int sink) {
    std::unordered_map<int, std::unordered_map<int, int>> residualCapacity;
    for (const auto& [connectionId, conn] : connections) {
        int pipeId = conn.getPipeId();
        int startStation = conn.getStationId1();
        int endStation = conn.getStationId2();

        auto pipeIt = Pipe::pipes.find(pipeId);
        if (pipeIt != Pipe::pipes.end() && !pipeIt->second.getStatus()) {
            int capacity = static_cast<int>(calculatePipePerformance(pipeIt->second));
            residualCapacity[startStation][endStation] = capacity;
        }
    }

    int maxFlow = 0;
    std::unordered_map<int, int> parent;

    auto bfs = [&](int s, int t) -> bool {
        std::unordered_set<int> visited;
        std::queue<int> queue;

        queue.push(s);
        visited.insert(s);
        parent.clear();

        while (!queue.empty()) {
            int current = queue.front();
            queue.pop();

            for (const auto& [neighbor, capacity] : residualCapacity[current]) {
                if (visited.find(neighbor) == visited.end() && capacity > 0) {
                    queue.push(neighbor);
                    visited.insert(neighbor);
                    parent[neighbor] = current;

                    if (neighbor == t) {
                        return true;
                    }
                }
            }
        }
        return false;
        };

    while (bfs(source, sink)) {
        int pathFlow = INT_MAX;

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            pathFlow = std::min(pathFlow, residualCapacity[u][v]);
        }

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            residualCapacity[u][v] -= pathFlow;
            residualCapacity[v][u] += pathFlow;
        }

        maxFlow += pathFlow;
    }

    return maxFlow;
}

int Connection::findPipeByStations(int station1, int station2) {
    for (const auto& [connectionId, conn] : connections) {
        if ((conn.getStationId1() == station1 && conn.getStationId2() == station2) ||
            (conn.getStationId1() == station2 && conn.getStationId2() == station1)) {
            return conn.getPipeId();
        }
    }
    throw std::runtime_error("����� ����� ���������� ��������� �� �������.");
}

std::vector<int> Connection::dijkstra(int start, int end) {
    std::unordered_map<int, double> dist;
    std::unordered_map<int, int> prev;
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> pq;

    for (const auto& station : CompressorStation::stations) {
        dist[station.first] = std::numeric_limits<double>::infinity();
    }
    dist[start] = 0.0;
    pq.push({ 0.0, start });

    while (!pq.empty()) {
        int current = pq.top().second;
        double currentDist = pq.top().first;
        pq.pop();

        if (current == end) break;

        for (int neighbor : adjListOut[current]) {
            int pipeId = findPipeByStations(current, neighbor);
            const auto& pipe = Pipe::pipes.at(pipeId);

            double weight = pipe.getStatus() ? std::numeric_limits<double>::infinity() : pipe.getLength();

            if (dist[current] + weight < dist[neighbor]) {
                dist[neighbor] = dist[current] + weight;
                prev[neighbor] = current;
                pq.push({ dist[neighbor], neighbor });
            }
        }
    }

    std::vector<int> path;
    if (dist[end] == std::numeric_limits<double>::infinity()) {
        std::cout << "���� �� ������� " << start << " � ������� " << end << " ����������.\n";
        return path;
    }

    for (int at = end; at != start; at = prev[at]) {
        path.push_back(at);
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());

    std::cout << "����� ����� ����: " << dist[end] << " ��\n";
    return path;
}

void Connection::handleMaxFlow() {
    int source, sink;
    std::cout << "������� ��������� �������: ";
    if (!(std::cin >> source)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "������: ������� ���������� �����.\n";
        return;
    }
    std::cout << "������� �������� �������: ";
    if (!(std::cin >> sink)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "������: ������� ���������� �����.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    try {
        double maxFlow = fordFulkerson(source, sink);
        std::cout << "������������ �����: " << maxFlow << "\n";
    }
    catch (const std::exception& e) {
        std::cout << "������ ��� ������� ������������� ������: " << e.what() << "\n";
    }
}

void Connection::handleShortestPath() {
    int start, end;
    std::cout << "������� ��������� �������: ";
    if (!(std::cin >> start)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "������: ������� ���������� �����.\n";
        return;
    }
    std::cout << "������� �������� �������: ";
    if (!(std::cin >> end)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "������: ������� ���������� �����.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    try {
        auto path = dijkstra(start, end);
        if (path.empty()) {
            std::cout << "���� �� ������.\n";
        }
        else {
            std::cout << "���������� ����: ";
            for (int station : path) std::cout << station << " ";
            std::cout << "\n";
        }
    }
    catch (const std::exception& e) {
        std::cout << "������ ��� ������� ����������� ����: " << e.what() << "\n";
    }
}

void Connection::connectionSubMenu() {
    std::vector<std::string> getMenuOptionsConnection = {
        "�����",
        "�������� ��� ����������",
        "�������� ����������",
        "�������� ���������� ",
        "�������������� ���������� �������",
        "���������� ������������ �����",
        "����� ���������� ���� ����� ���������"
    };

    std::string command;
    long value;
    logger.log("���� � ������� ����������.");
    while (true) {
        displayMenu(getMenuOptionsConnection);
        std::getline(std::cin, command);
        value = numberOrDefault(command);

        switch (value) {
        case 1:
            displayAllConnections();
            break;
        case 2:
            addConnection();
            break;
        case 3:
            deleteConnection();
            break;
        case 4:
            logger.log("������ �������������� ���������� �������.");
            topologicalSortMenu();
            break;
        case 5: {
            handleMaxFlow();
            break;
        }
        case 6: {
            handleShortestPath();
            break;
        }
        case 0:
            logger.log("����� �� ������� ����������.");
            std::cout << "����� �� ���� ������ � ������������.\n";
            return;
        default:
            logger.log("������: �������� ����� � ���� ����������.");
            std::cout << "�������� �����. ���������� �����.\n";
            break;
        }
    }
}