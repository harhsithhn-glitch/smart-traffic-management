#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <chrono>

using namespace std;

mutex mtx;

/* ---------------- CLASS ---------------- */
class Intersection {
public:
    int id;
    int vehicles;
    bool emergency;
    int greenTime;

    Intersection(int i, int v, bool e) {
        id = i;
        vehicles = v;
        emergency = e;
        greenTime = 0;
    }

    void calculateGreenTime() {
        if (emergency)
            greenTime = 60;
        else if (vehicles > 50)
            greenTime = 45;
        else if (vehicles > 20)
            greenTime = 30;
        else
            greenTime = 20;
    }

    void runSignal() {
        mtx.lock();
        cout << "\n🚦 Intersection " << id << endl;
        cout << "Vehicles: " << vehicles << endl;
        cout << "Emergency: " << (emergency ? "YES" : "NO") << endl;
        cout << "GREEN signal for " << greenTime << " seconds\n";
        mtx.unlock();

        this_thread::sleep_for(chrono::seconds(1));
    }

    void writeReport(ofstream &file) {
        file << "Intersection " << id << "\n";
        file << "Vehicles: " << vehicles << "\n";
        file << "Emergency: " << (emergency ? "YES" : "NO") << "\n";
        file << "Green Time: " << greenTime << " sec\n\n";
    }
};

/* ---------------- MAIN ---------------- */
int main() {
    int n;
    cout << "Enter number of intersections: ";
    cin >> n;

    vector<Intersection> intersections;

    for (int i = 1; i <= n; i++) {
        int v, e;
        cout << "\nIntersection " << i << endl;
        cout << "Enter number of vehicles: ";
        cin >> v;
        cout << "Emergency vehicle present? (1 = Yes, 0 = No): ";
        cin >> e;

        intersections.emplace_back(i, v, e);
        intersections.back().calculateGreenTime();
    }

    cout << "\n--- SIGNAL SIMULATION STARTED ---\n";

    vector<thread> threads;
    for (auto &in : intersections) {
        threads.push_back(thread(&Intersection::runSignal, &in));
    }

    for (auto &t : threads)
        t.join();

    ofstream report("traffic_report.txt");
    report << "SMART TRAFFIC MANAGEMENT REPORT\n\n";

    for (auto &in : intersections)
        in.writeReport(report);

    report.close();

    cout << "\n📄 Report saved as traffic_report.txt\n";
    cout << "✅ Simulation complete\n";

    return 0;
}
