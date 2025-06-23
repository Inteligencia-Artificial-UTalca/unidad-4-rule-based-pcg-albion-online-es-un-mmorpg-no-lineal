#include <iostream>
#include <vector>
#include <random>   // For random number generation
#include <chrono> 
#include <thread>
// For seeding the random number generator

// Define Map as a vector of vectors of integers.
// You can change 'int' to whatever type best represents your cells (e.g., char, bool).
using Map = std::vector<std::vector<int>>;

/**
 * @brief Prints the map (matrix) to the console.
 * @param map The map to print.
 */
void printMap(const Map& map) {
    std::cout << "--- Current Map ---" << std::endl;
    for (const auto& row : map) {
        for (int cell : row) std::cout << (cell ? '.' : '#') << ' ';
        std::cout << std::endl;
    }
    std::cout << "-------------------" << std::endl;
}

/**
 * @brief Function to implement the Cellular Automata logic.
 * It should take a map and return the updated map after one iteration.
 * @param currentMap The map in its current state.
 * @param W Width of the map.
 * @param H Height of the map.
 * @param R Radius of the neighbor window (e.g., 1 for 3x3, 2 for 5x5).
 * @param U Threshold to decide if the current cell becomes 1 or 0.
 * @return The map after applying the cellular automata rules.
 */

Map cellularAutomata(const Map& currentMap, int W, int H, int R, double U) {
    // 1. Creamos newMap como copia de currentMap
    Map newMap = currentMap;

    // TODO: IMPLEMENTATION GOES HERE for the cellular automata logic.
    // Iterate over each cell and apply the transition rules.
    // Remember that updates should be based on the 'currentMap' state
    // and applied to the 'newMap' to avoid race conditions within the same iteration.
    
    // 2. Recorremos cada celda de currentMap
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            int count = 0;

            // 3. Contamos vecinos en la ventana de radio R (excluimos la celda central)
            for (int dx = -R; dx <= R; ++dx) {
                for (int dy = -R; dy <= R; ++dy) {
                    if (dx == 0 && dy == 0) continue;        // saltamos la propia celda
                    int ni = i + dx, nj = j + dy;

                    if (ni >= 0 && ni < H && nj >= 0 && nj < W) {
                        // dentro del mapa, sumamos el valor (0 o 1)
                        count += currentMap[ni][nj];
                    } else {
                        // fuera de límites: tratamos como PARED (1)
                        count += 1;
                    }
                }
            }

            // 4. Aplicamos umbral U: si vecinos ≥ U → 1, sino → 0
            newMap[i][j] = (count >= U) ? 1 : 0;
        }
    }

    return newMap;
}

/**
 * @brief Function to implement the Drunk Agent logic.
 * It should take a map and parameters controlling the agent's behavior,
 * then return the updated map after the agent performs its actions.
 *
 * @param currentMap The map in its current state.
 * @param W Width of the map.
 * @param H Height of the map.
 * @param J The number of times the agent "walks" (initiates a path).
 * @param I The number of steps the agent takes per "walk".
 * @param roomSizeX Max width of rooms the agent can generate.
 * @param roomSizeY Max height of rooms the agent can generate.
 * @param probGenerateRoom Probability (0.0 to 1.0) of generating a room at each step.
 * @param probIncreaseRoom If no room is generated, this value increases probGenerateRoom.
 * @param probChangeDirection Probability (0.0 to 1.0) of changing direction at each step.
 * @param probIncreaseChange If direction is not changed, this value increases probChangeDirection.
 * @param agentX Current X position of the agent (updated by reference).
 * @param agentY Current Y position of the agent (updated by reference).
 * @return The map after the agent's movements and actions.
 */

int clamp(int v, int lo, int hi) { return std::max(lo, std::min(v, hi)); }

Map drunkAgent(const Map& currentMap, int W, int H, int J, int I,
               int roomW, int roomH,
               double probRoom, double probIncRoom,
               double probDir, double probIncDir,
               int& agentX, int& agentY) {
    Map newMap = currentMap;
    // TODO: IMPLEMENTATION GOES HERE for the Drunk Agent logic.
    // The agent should move randomly.
    // You'll need a random number generator.
    // Consider:
    // - How the agent moves (possible steps).
    // - What it does if it encounters a border or an obstacle (if applicable).
    // - How it modifies the map (e.g., leaving a trail, creating rooms, etc.).
    // - Use the provided parameters (J, I, roomSizeX, roomSizeY, probabilities)
    //   to control its behavior.
    
    std::mt19937 rng((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dist01(0.0, 1.0);
    std::uniform_int_distribution<int> dirDist(0, 3); // 4 directions

    int dir = dirDist(rng);
    double pR = probRoom, pD = probDir;


    for (int walk = 0; walk < J; ++walk) {
        for (int step = 0; step < I; ++step) {
            // Marca la celda actual como pasillo
            newMap[agentX][agentY] = 1;

            // Con probabilidad pD, cambia dirección
            if (dist01(rng) < pD) {
                dir = dirDist(rng);
                pD = probDir; // reinicia probabilidad de cambio de dirección
            } else {
                pD = std::min(1.0, pD + probIncDir);
            }

            // Calcula nueva posición
            int newX = agentX;
            int newY = agentY;
            switch (dir) {
                case 0: newX--; break; // arriba
                case 1: newY++; break; // derecha
                case 2: newX++; break; // abajo
                case 3: newY--; break; // izquierda
            }

            // Si está dentro del mapa, se mueve
            if (newX >= 0 && newX < H && newY >= 0 && newY < W) {
                agentX = newX;
                agentY = newY;
            } else {
                // Si se sale del mapa, cambia dirección aleatoriamente
                dir = dirDist(rng);
                continue;
            }
        }

        // Al final del camino, con probabilidad pR genera habitación
        if (dist01(rng) < pR) {
            int startX = clamp(agentX - roomH / 2, 0, H - 1);
            int endX = clamp(agentX + roomH / 2, 0, H - 1);
            int startY = clamp(agentY - roomW / 2, 0, W - 1);
            int endY = clamp(agentY + roomW / 2, 0, W - 1);

            for (int x = startX; x <= endX; ++x) {
                for (int y = startY; y <= endY; ++y) {
                    newMap[x][y] = 1;
                }
            }

            // Reinicia probabilidad de generar cuarto
            pR = probRoom;
        } else {
            pR = std::min(1.0, pR + probIncRoom);
        }
    }

    return newMap;
}

int main() {
    std::cout << "--- CELLULAR AUTOMATA AND DRUNK AGENT SIMULATION ---" << std::endl;

    // --- Initial Map Configuration ---
    int mapRows = 20;
    int mapCols = 30;
    Map myMap(mapRows, std::vector<int>(mapCols, 0)); // Map initialized with zeros

    // Inicializar con valores aleatorios 0 o 1
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> bin(0,1);
    for (auto& row : myMap)
        for (int& cell : row)
            cell = bin(rng);

    // TODO: IMPLEMENTATION GOES HERE: Initialize the map with some pattern or initial state.
    // For example, you might set some cells to 1 for the cellular automata
    // or place the drunk agent at a specific position.

    // Drunk Agent's initial position
    std::uniform_int_distribution<int> randRow(0, mapRows-1);
    std::uniform_int_distribution<int> randCol(0, mapCols-1);
    int drunkAgentX = randRow(rng);
    int drunkAgentY = randCol(rng);

    // If your agent modifies the map at start, you could do it here:
    // myMap[drunkAgentX][drunkAgentY] = 2; // Assuming '2' represents the agent

    std::cout << "\nInitial map state:" << std::endl;
    printMap(myMap);

    // --- Simulation Parameters ---
    int numIterations = 3; // Number of simulation steps

    // Cellular Automata Parameters
    int ca_W = mapCols;
    int ca_H = mapRows;
    int ca_R = 1;      // Radius of neighbor window
    double ca_U = 5; // Threshold

    // Drunk Agent Parameters
    int da_W = mapCols;
    int da_H = mapRows;
    int da_J = 5;      // Number of "walks"
    int da_I = 10;     // Steps per walk
    int da_roomX = 5;
    int da_roomY = 3;
    double da_probGenRoom = 0.1;
    double da_probIncRoom = 0.05;
    double da_probChangeDir = 0.2;
    double da_probIncChange = 0.03;


    // --- Main Simulation Loop ---
    for (int iteration = 0; iteration < numIterations; ++iteration) {
        std::cout << "\n--- Iteration " << iteration + 1 << " ---" << std::endl;

        // TODO: IMPLEMENTATION GOES HERE: Call the Cellular Automata and/or Drunk Agent functions.
        // The order of calls will depend on how you want them to interact.

        // Example: First the cellular automata, then the agent

        myMap = cellularAutomata(myMap, ca_W, ca_H, ca_R, ca_U);

        std::cout << "CellularAutomata \n\n";
        printMap(myMap);

        myMap = drunkAgent(myMap, mapCols, mapRows, da_J, da_I, da_roomX, da_roomY,
                    da_probGenRoom, da_probIncRoom, da_probChangeDir, da_probIncChange, drunkAgentX, drunkAgentY);

        std::cout << "\nDrunkAgent\n";
        printMap(myMap);
        

        // You can add a delay to visualize the simulation step by step
        // For std::this_thread::sleep_for
        // For std::chrono::milliseconds
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "\n--- Simulation Finished ---" << std::endl;
    return 0;
}
