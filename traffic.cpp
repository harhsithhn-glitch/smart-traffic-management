#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <mutex>
#include <string>

using namespace std;

mutex mtx;

class Intersection {
    int id;
    int vehicles;
    bool emergency;
    int greenTime;

public:
    Intersection(int i, int v, bool e)
        : id(i), vehicles(v), emergency(e), greenTime(0) {}

    void calculateGreenTime() {
        if (emergency)
            greenTime = 60;
        else if (vehicles > 70)
            greenTime = 45;
        else if (vehicles > 30)
            greenTime = 35;
        else
            greenTime = 25;
    }

    void runSignal() {
        calculateGreenTime();

        lock_guard<mutex> lock(mtx);
        cout << "\nIntersection " << id << endl;
        cout << "Vehicles   : " << vehicles << endl;
        cout << "Emergency  : " << (emergency ? "YES" : "NO") << endl;
        cout << "Green Time : " << greenTime << " seconds" << endl;
    }

    void writeReport(ofstream &out) {
        out << "Intersection " << id << "\n";
        out << "Vehicles   : " << vehicles << "\n";
        out << "Emergency  : " << (emergency ? "YES" : "NO") << "\n";
        out << "Green Time : " << greenTime << " seconds\n";
        out << "------------------------------\n";
    }
};

int main(int argc, char* argv[]) {
    vector<Intersection> intersections;
    bool autoMode = false;

    if (argc > 1 && string(argv[1]) == "--auto") {
        autoMode = true;
    }

    if (autoMode) {
        ifstream file("config.txt");
        if (!file) {
            cout << "Error: config.txt not found\n";
            return 1;
        }

        int n;
        file >> n;

        for (int i = 1; i <= n; i++) {
            int v, e;
            file >> v >> e;
            intersections.emplace_back(i, v, e == 1);
        }

        file.close();
        cout << "Loaded data from config.txt\n";

    } else {
        int n;
        cout << "Enter number of intersections: ";
        cin >> n;

        for (int i = 1; i <= n; i++) {
            int v, e;
            cout << "\nIntersection " << i << endl;
            cout << "Enter number of vehicles: ";
            cin >> v;
            cout << "Emergency vehicle present? (1 = Yes, 0 = No): ";
            cin >> e;

            intersections.emplace_back(i, v, e == 1);
        }
    }

    cout << "\n--- SIGNAL SIMULATION STARTED ---\n";

    vector<thread> threads;
    for (auto &in : intersections) {
        threads.emplace_back(&Intersection::runSignal, &in);
    }

    for (auto &t : threads) {
        t.join();
    }

    ofstream report("traffic_report.txt");
    report << "SMART TRAFFIC MANAGEMENT REPORT\n";
    report << "===============================\n\n";

    for (auto &in : intersections) {
        in.writeReport(report);
    }

    report.close();

    cout << "\nReport saved as traffic_report.txt\n";
    cout << "Simulation complete\n";

    return 0;
}
