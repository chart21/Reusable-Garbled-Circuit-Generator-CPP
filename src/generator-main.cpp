#include <iostream>
//#include "circuitProcessor/include/circuitStructs.h"

#include "circuitProcessor/include/circuitReader.h"
#include "circuitProcessor/include/circuitEvaluator.h"
//#include <./circuitProcessor/include/circuitStructs.h>
#include "circuitProcessor/include/helperFunctions.h"
#include "circuitProcessor/include/circuitFlipper.h"
#include "circuitProcessor/include/circuitObfuscator.h"
#include "circuitProcessor/include/circuitDetails.h"
#include "circuitProcessor/include/circuitIntegrityBreaker.h"
#include "circuitProcessor/include/circuitWriter.h"
#include "circuitProcessor/include/leakagePredictor.h"

//#include <emp-tool/emp-tool.h>
#include "programs/include/mult3.h"


#include <chrono>

#define time_S t1 = startClock();
#define time_E stopClock(t1);















template<typename F, typename... Args>
void funcTime(F func, Args&&... args){
    std::chrono::high_resolution_clock::time_point t1 = 
        std::chrono::high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now()-t1).count();
        
    std::cout << time << __func__ << '\n';
}



int main(int argc, char *argv[])
{
    generateCircuitRAM();
    uint_fast64_t numThreads = 1;
    uint_fast64_t timeSleep = 300;
    
    std::string circuitName = "adder64";
    if (argc > 1)
    {
        circuitName = argv[1]; 
    }
    std::string filepath = "/home/christopher/Documents/C-RGCG/src/circuits/" +circuitName;

    std::string circuitFormat = "bristol";
    if (argc > 4)
    {
        circuitFormat = argv[4];
    }

    if (argc > 5)
    {
        numThreads = std::stoul(argv[5]);
    }

    if (argc > 6)
    {
        timeSleep = std::stoul(argv[6]);
    }

    auto details = importBristolCircuitDetails(filepath + ".txt", circuitFormat);

   
    auto bristolCircuit = importBristolCircuitExNotForLeakagePrediction(filepath+ ".txt", details);
    
    
    if(numThreads == 1)
    {
           
    auto parents = new uint_fast64_t[bristolCircuit->details.numWires * 2]();    
    getPrevofEachWire(bristolCircuit, parents);
    auto po = new bool[bristolCircuit->details.numWires];    
    funcTime(getPotentiallyObfuscatedGates,bristolCircuit, po);
    funcTime(getPotentiallyIntegrityBreakingGatesFromOutput,bristolCircuit->details, po, parents);
    

    int poc = - bristolCircuit->details.bitlengthInputA;
    for (auto i = 0; i < bristolCircuit->details.numWires; i++)
    {
        poc+= po[i];
    }
    std::cout << "potentially obfuscated and integrity breaking" << poc << '\n';

    delete[] po;    
    delete[] bristolCircuit->gates;
    delete[] bristolCircuit;      
     

    auto circuit = importBristolCircuitExNot(filepath+ ".txt", details);
    bool* inputA = new bool[circuit->details.bitlengthInputA];
    bool* inputB = new bool[circuit->details.bitlengthInputB];
    generateRandomInput(details.bitlengthInputA, inputA);
    generateRandomInput(details.bitlengthInputB, inputB);
    
    if (argc > 2)
    {        
        uint_fast64_t a = std::stoul(argv[2]);
        converIntToBoolArr(a, circuit->details.bitlengthInputA, inputA);
    }
    if (argc > 3)
    {
        uint_fast64_t b = std::stoul(argv[3]);
        converIntToBoolArr(b, circuit->details.bitlengthInputB, inputB);
    }  
  

    bool* output = new bool[circuit->details.numOutputs * circuit->details.bitlengthOutputs];
    funcTime(evaluateTransformedCircuit,circuit, inputA, inputB, output);


    int inA = convertBoolArrToInt(inputA, details.bitlengthInputA);
    int inB = convertBoolArrToInt(inputB, details.bitlengthInputB);
    int iout = convertBoolArrToInt(output, details.bitlengthOutputs);
    std::cout << "inA" << inA <<"\n";    
    std::cout << "inB" << inB <<"\n"; 
    std::cout << "iout" << iout <<"\n"; 

    
    bool* obfuscatedValArr = new bool[details.bitlengthInputA];
    bool* flipped = new bool[details.numWires];     

    obfuscateInput(inputA,obfuscatedValArr,flipped,circuit->details);
    funcTime(getFlippedCircuitWithoutOutputsN,circuit, flipped);
    //funcTime(getFlippedCircuitWithoutOutputsMT,circuit, flipped, 7, 200);
    delete[] flipped;

    bool *isObfuscated = new bool[circuit->details.numWires]();    
    funcTime(moreEfficientObfuscationArr,circuit, obfuscatedValArr, isObfuscated);
    
 
    uint_fast64_t counter = 0;
    for(auto i = circuit->details.bitlengthInputA+circuit->details.bitlengthInputB; i < circuit->details.numWires;i++)
    {
        
        if(isObfuscated[i])
        {
            counter++;
            //std::cout << "obfuscatedInd" << i << '\n';

        }
            

    }
    std::cout << "obfuscated" << counter << '\n';

          
    funcTime(getIntegrityBreakingGatesfromOutput,circuit->details, isObfuscated, parents);
    
    delete[] parents;

    poc = - circuit->details.bitlengthInputA;
    for (auto i = 0; i < circuit->details.numWires; i++)
    {
        poc+= isObfuscated[i];
    }
    std::cout <<  "obfuscated and Integrity breaking" << poc << '\n';   
             
    funcTime(breakIntegrityOfGates,circuit, isObfuscated);
        
    delete[] isObfuscated;
    
    
    auto outputRGC = new bool[circuit->details.bitlengthOutputs*circuit->details.numOutputs];
    evaluateTransformedCircuit(circuit, obfuscatedValArr, inputB, outputRGC);
    
        std::cout << "Circuit outputs are equal?" << equalBoolArr(outputRGC, output, circuit->details.bitlengthOutputs) << '\n';

    inA = convertBoolArrToInt(obfuscatedValArr, details.bitlengthInputA);
    inB = convertBoolArrToInt(inputB, details.bitlengthInputB);
    iout = convertBoolArrToInt(outputRGC, details.bitlengthOutputs);
    std::cout << "inA" << inA <<"\n";    
    std::cout << "inB" << inB <<"\n"; 
    std::cout << "iout" << iout <<"\n"; 

    

    exportCircuitSeparateFiles(circuit, filepath);
    exportObfuscatedInput(obfuscatedValArr, circuit->details, filepath);

    auto newOut1 = new bool[circuit->details.bitlengthOutputs*circuit->details.numOutputs];
    evaluateTransformedCircuit(circuit, obfuscatedValArr, inputB, newOut1);
    inA = convertBoolArrToInt(obfuscatedValArr, details.bitlengthInputA);
    inB = convertBoolArrToInt(inputB, details.bitlengthInputB);
    auto iout1 = convertBoolArrToInt(newOut1, details.bitlengthOutputs);

   

    
    delete[] circuit->gates;
    delete[] circuit; 

    auto newDetails = importBristolCircuitDetails(filepath + "_rgc_details.txt", "rgc");
       
    
    auto newCircuit = importTransformedCircuit(filepath + "_rgc.txt", newDetails);
    

    auto newInputA = new bool[newCircuit->details.bitlengthInputA];
    importInput(filepath + "_rgc_inputA.txt",newCircuit->details.bitlengthInputA, newInputA);
    auto newOut2 = new bool[newCircuit->details.bitlengthOutputs*newCircuit->details.numOutputs];    
    evaluateTransformedCircuit(newCircuit, newInputA, inputB, newOut2);

    inA = convertBoolArrToInt(newInputA, newDetails.bitlengthInputA);
    inB = convertBoolArrToInt(inputB, newDetails.bitlengthInputB);
    auto iout2 = convertBoolArrToInt(newOut2, newDetails.bitlengthOutputs);


    }


    else
    {
                  
    auto parents = new uint_fast64_t[bristolCircuit->details.numWires * 2]();    
    getPrevofEachWireMT(bristolCircuit, parents, numThreads);
    auto po = new bool[bristolCircuit->details.numWires];    
    funcTime(getPotentiallyObfuscatedGatesMT,bristolCircuit, po, numThreads, timeSleep);
    funcTime(getPotentiallyIntegrityBreakingGatesFromOutputMT,bristolCircuit->details, po, parents, numThreads);
   

    int poc = - bristolCircuit->details.bitlengthInputA;
    for (auto i = 0; i < bristolCircuit->details.numWires; i++)
    {
        poc+= po[i];
    }
    std::cout << "potentially obfuscated and integrity breaking" << poc << '\n';

    delete[] po;    
    delete[] bristolCircuit->gates;
    delete[] bristolCircuit;      
     

    auto circuit = importBristolCircuitExNot(filepath+ ".txt", details);
    bool* inputA = new bool[circuit->details.bitlengthInputA];
    bool* inputB = new bool[circuit->details.bitlengthInputB];
    generateRandomInput(details.bitlengthInputA, inputA);
    generateRandomInput(details.bitlengthInputB, inputB);
    
    if (argc > 2)
    {        
        uint_fast64_t a = std::stoul(argv[2]);
        converIntToBoolArr(a, circuit->details.bitlengthInputA, inputA);
    }
    if (argc > 3)
    {
        uint_fast64_t b = std::stoul(argv[3]);
        converIntToBoolArr(b, circuit->details.bitlengthInputB, inputB);
    }  
  

    bool* output = new bool[circuit->details.numOutputs * circuit->details.bitlengthOutputs];
    funcTime(evaluateTransformedCircuitMT,circuit, inputA, inputB, output, numThreads, timeSleep);


    int inA = convertBoolArrToInt(inputA, details.bitlengthInputA);
    int inB = convertBoolArrToInt(inputB, details.bitlengthInputB);
    int iout = convertBoolArrToInt(output, details.bitlengthOutputs);
    std::cout << "inA" << inA <<"\n";    
    std::cout << "inB" << inB <<"\n"; 
    std::cout << "iout" << iout <<"\n"; 

    
    bool* obfuscatedValArr = new bool[details.bitlengthInputA];
    bool* flipped = new bool[details.numWires];     

    obfuscateInput(inputA,obfuscatedValArr,flipped,circuit->details);
    funcTime(getFlippedCircuitWithoutOutputsMT,circuit, flipped, numThreads, timeSleep);
    //funcTime(getFlippedCircuitWithoutOutputsN,circuit, flipped);
    delete[] flipped;

    bool *isObfuscated = new bool[circuit->details.numWires]();    
    funcTime(moreEfficientObfuscationMT,circuit, obfuscatedValArr, isObfuscated, numThreads, timeSleep);
    //funcTime(moreEfficientObfuscationArr,circuit, obfuscatedValArr, isObfuscated);
 
    uint_fast64_t counter = 0;
    for(auto i = circuit->details.bitlengthInputA+circuit->details.bitlengthInputB; i < circuit->details.numWires;i++)
    {
        
        if(isObfuscated[i])
        {
            counter++;
            //std::cout << "obfuscatedInd" << i << '\n';

        }
            

    }
    std::cout << "obfuscated" << counter << '\n';

          
    funcTime(getIntegrityBreakingGatesfromOutputMT2,circuit->details, isObfuscated, parents, numThreads);
    
    delete[] parents;

    poc = - circuit->details.bitlengthInputA;
    for (auto i = 0; i < circuit->details.numWires; i++)
    {
        poc+= isObfuscated[i];
    }
    std::cout <<  "obfuscated and Integrity breaking" << poc << '\n';   
             
    funcTime(breakIntegrityOfGatesMT,circuit, isObfuscated, numThreads);
        
    delete[] isObfuscated;
    
    
    auto outputRGC = new bool[circuit->details.bitlengthOutputs*circuit->details.numOutputs];
    evaluateTransformedCircuitMT(circuit, obfuscatedValArr, inputB, outputRGC, numThreads, timeSleep);
    

        std::cout << "Circuit outputs are equal?" << equalBoolArr(outputRGC, output, circuit->details.bitlengthOutputs) << '\n';

    inA = convertBoolArrToInt(obfuscatedValArr, details.bitlengthInputA);
    inB = convertBoolArrToInt(inputB, details.bitlengthInputB);
    iout = convertBoolArrToInt(outputRGC, details.bitlengthOutputs);
    std::cout << "inA" << inA <<"\n";    
    std::cout << "inB" << inB <<"\n"; 
    std::cout << "iout" << iout <<"\n"; 

    

    exportCircuitSeparateFiles(circuit, filepath);
    exportObfuscatedInput(obfuscatedValArr, circuit->details, filepath);

    auto newOut1 = new bool[circuit->details.bitlengthOutputs*circuit->details.numOutputs];
    evaluateTransformedCircuitMT(circuit, obfuscatedValArr, inputB, newOut1, numThreads, timeSleep);
    inA = convertBoolArrToInt(obfuscatedValArr, details.bitlengthInputA);
    inB = convertBoolArrToInt(inputB, details.bitlengthInputB);
    auto iout1 = convertBoolArrToInt(newOut1, details.bitlengthOutputs);

   

    
    delete[] circuit->gates;
    delete[] circuit; 

    auto newDetails = importBristolCircuitDetails(filepath + "_rgc_details.txt", "rgc");
       
    
    auto newCircuit = importTransformedCircuit(filepath + "_rgc.txt", newDetails);
    

    auto newInputA = new bool[newCircuit->details.bitlengthInputA];
    importInput(filepath + "_rgc_inputA.txt",newCircuit->details.bitlengthInputA, newInputA);
    auto newOut2 = new bool[newCircuit->details.bitlengthOutputs*newCircuit->details.numOutputs];    
    evaluateTransformedCircuitMT(newCircuit, newInputA, inputB, newOut2, numThreads, timeSleep);

    inA = convertBoolArrToInt(newInputA, newDetails.bitlengthInputA);
    inB = convertBoolArrToInt(inputB, newDetails.bitlengthInputB);
    auto iout2 = convertBoolArrToInt(newOut2, newDetails.bitlengthOutputs);




 
   

    
    }
    
    



    

    

    
    return 0;
}
