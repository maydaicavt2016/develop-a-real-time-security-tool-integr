#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <algorithm>

using namespace std;

// Enum for security tool types
enum class ToolType { FIREWALL, INTRUSION_DETECTION, ANTI_VIRUS };

// Struct for security tool
struct SecurityTool {
    ToolType type;
    string name;
    bool enabled;

    SecurityTool(ToolType t, string n) : type(t), name(n), enabled(false) {}
};

// Class for real-time security tool integrator
class RealTimeSecurityToolIntegrator {
private:
    vector<SecurityTool> tools;
    mutex mtx;
    condition_variable cv;
    bool running;

    void runTool(const SecurityTool& tool) {
        cout << "Running " << tool.name << "...\n";
        // Simulate tool execution
        this_thread::sleep_for(chrono::seconds(2));
        cout << "Completed " << tool.name << "...\n";
    }

public:
    RealTimeSecurityToolIntegrator() : running(true) {}

    void addTool(ToolType type, string name) {
        unique_lock<mutex> lock(mtx);
        tools.emplace_back(type, name);
    }

    void start() {
        thread t([this] {
            while (running) {
                unique_lock<mutex> lock(mtx);
                cv.wait(lock, [this] { return !tools.empty() || !running; });
                while (!tools.empty()) {
                    SecurityTool tool = tools.back();
                    tools.pop_back();
                    lock.unlock();
                    runTool(tool);
                    lock.lock();
                }
            }
        });
        t.detach();
    }

    void stop() {
        running = false;
        cv.notify_one();
    }

    void enableTool(ToolType type) {
        unique_lock<mutex> lock(mtx);
        for (auto& tool : tools) {
            if (tool.type == type) {
                tool.enabled = true;
            }
        }
    }

    void disableTool(ToolType type) {
        unique_lock<mutex> lock(mtx);
        for (auto& tool : tools) {
            if (tool.type == type) {
                tool.enabled = false;
            }
        }
    }
};

int main() {
    RealTimeSecurityToolIntegrator integrator;

    integrator.addTool(ToolType::FIREWALL, "Firewall X");
    integrator.addTool(ToolType::INTRUSION_DETECTION, "IDS Y");
    integrator.addTool(ToolType::ANTI_VIRUS, "AV Z");

    integrator.start();

    this_thread::sleep_for(chrono::seconds(5));

    integrator.enableTool(ToolType::FIREWALL);
    integrator.disableTool(ToolType::INTRUSION_DETECTION);

    this_thread::sleep_for(chrono::seconds(5));

    integrator.stop();

    return 0;
}