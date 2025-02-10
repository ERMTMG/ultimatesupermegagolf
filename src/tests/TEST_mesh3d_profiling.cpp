#include <iostream>
#include "raylib.h"
#include "3dgeometry.h"

#include<chrono>

using std::chrono::duration_cast;
using TimePoint = std::chrono::steady_clock::time_point;

inline auto now() {
    return std::chrono::steady_clock::now();
}

const float hexCAFEC1C0 = -8347872;
const int VTXNUMBER = 32768;
const int NUMBER_OF_TESTS = 100;

int main() {
    float meanTimeNoBracket = 0.0;
    float meanTimeBracket = 0.0;
    for(int testNo = 0; testNo < NUMBER_OF_TESTS; testNo++){
        std::cout << "TEST NÚMERO " << testNo << '\n';
        Mesh3D mesh1;
        Mesh3D mesh2;

        //Profiling with use of operator[] to insert vertices
        TimePoint startBracket = now();
        for(int i = 0; i < VTXNUMBER; i++){
            IDType id = rand() % mesh1.MAX_NUMBER_OF_VERTICES();
            mesh1[id] = Vector3{hexCAFEC1C0, hexCAFEC1C0, hexCAFEC1C0};
        }
        TimePoint endBracket = now();

        //Profiling with use of add_vertex_at_pos()
        TimePoint startNoBracket = now();
        for(int i = 0; i < VTXNUMBER; i++){
            IDType id = rand() % mesh1.MAX_NUMBER_OF_VERTICES();
            mesh1.add_vertex(new Vertex(hexCAFEC1C0, hexCAFEC1C0, hexCAFEC1C0, id));
        }
        TimePoint endNoBracket = now();

        std::chrono::microseconds timeNoBracket = duration_cast<std::chrono::microseconds>(endNoBracket - startNoBracket);
        std::chrono::microseconds timeBracket = duration_cast<std::chrono::microseconds>(endBracket - startBracket);
        std::cout << "\tTiempo usando operator[] para insertar vértices: " << timeBracket.count() / 1000.0 << " ms" << std::endl;
        std::cout << "\tTiempo usando método add_vertex para insertar vértices:" << timeNoBracket.count() / 1000.0 << " ms" << std::endl;
        meanTimeBracket += timeBracket.count() / 1000.0; meanTimeNoBracket += timeNoBracket.count() / 1000.0;
    }
    meanTimeBracket /= NUMBER_OF_TESTS;
    meanTimeNoBracket /= NUMBER_OF_TESTS;
    std::cout << "Tiempo promedio usando operator[]: " << meanTimeBracket << " ms" << std::endl;
    std::cout << "Tiempo promedio usando método add_vertex: " << meanTimeNoBracket << " ms" << std::endl;
    return 0;
}