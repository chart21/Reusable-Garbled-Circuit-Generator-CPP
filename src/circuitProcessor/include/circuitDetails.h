#include "circuitStructs.h"
#include <vector>
#include <iostream>
#include <thread>

void getPrevofEachWire(BristolCircuit* circuit, uint_fast64_t* parents);
void getPrevofEachWireT(TransformedCircuit* circuit, uint_fast64_t* parents);
void getSuccessorsOfEachWire(CircuitDetails details, uint_fast64_t* parents, std::vector<uint_fast64_t>* successors);

void getLevelOfGates(BristolCircuit* circuit, uint_fast64_t* levels);
uint_fast64_t getLevelOfGates(TransformedCircuit* circuit, uint_fast64_t* levels);

void getCircuitbyLevels(TransformedCircuit* circuit, uint_fast64_t* levels, std::vector<TransformedGate>* circuitByLevels);

void getPrevofEachWireThread(BristolCircuit* circuit, uint_fast64_t* parents, uint_fast64_t id, uint_fast64_t numThreads);
void getPrevofEachWireMT(BristolCircuit* circuit, uint_fast64_t* parents, uint_fast64_t numThreads);

void getPrevofEachWireThreadTransformed(TransformedCircuit* circuit, uint_fast64_t* parents, uint_fast64_t id, uint_fast64_t numThreads);
void getPrevofEachWireMTTransformed(TransformedCircuit* circuit, uint_fast64_t* parents, uint_fast64_t numThreads);