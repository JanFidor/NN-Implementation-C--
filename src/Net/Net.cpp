#include "Net.h"
#include <algorithm>


Net::Net(
    const LossFunction& lossFunction, 
    const std::vector<LayerStructure>& netStructure, 
    const std::pair<double, double> weightInitialRange,
    double alpha
) : generator(weightInitialRange.first, weightInitialRange.second), lossFunction(lossFunction), alpha(alpha){

    for(int i = 0; i < netStructure.size() - 1; i++){
        LayerStructure currStructure = netStructure[i];
        LayerStructure nextStructure = netStructure[i + 1];

        Layer layer = generateLayer(currStructure.getFunction(), currStructure.getSize() + 1,nextStructure.getSize());
        layers.push_back(layer);
    }

    LayerStructure outputStructure = netStructure.back();
    Layer output = generateLayer(outputStructure.getFunction(), outputStructure.getSize(), 0);
    layers.push_back(output);

    int i;
}


// TODO not accounting for bias
Layer Net::generateLayer(const ActivationFunction& function, int size, int weightCountEach){
    // std::vector<std::vector<double>> weights(structure.getSize());
    // std::generate(weights.begin(), weights.end(), [this](int nextLayerSize) { 
    //     return generateWeights(nextLayerSize); 
    // });
    std::vector<std::vector<double>> weights;
    
    for(int i = 0; i < size; i++)
        weights.emplace_back(generateWeights(weightCountEach));   

    return Layer(function, weights);
}

std::vector<double> Net::generateWeights(int weightsCount){
    // std::vector<std::vector<double>> weights(structure.getSize());
    // std::generate(weights.begin(), weights.end(), [this](int nextLayerSize) { 
    //     return generateWeights(nextLayerSize); 
    // });
    std::vector<double> weights;
    
    for(int i = 0; i < weightsCount; i++)
        weights.emplace_back(generator.generate());   

    return weights;
}



void Net::propagate(const std::vector<double>& inputs, const std::vector<double>& targets){
    setInput(inputs);
    propagateForward();
    calculateOutputDerivatives(targets);
    propagateBackward();
}

void Net::setInput(const std::vector<double>& inputs){
    layers[0].calculateInputs(inputs);
}

void Net::calculateOutputDerivatives(const std::vector<double>& targets){
    Layer& outputLayer = layers.back();
    const std::vector<double>& outputs = outputLayer.getOutputs();
    // std::vector<std::vector<double>> weights(structure.getSize());
    // std::generate(weights.begin(), weights.end(), [this](int nextLayerSize) { 
    //     return generateWeights(nextLayerSize); 
    // });
    
    std::vector<double> derivatives = lossFunction.calculateDerivatives(targets, outputs);

    outputLayer.setTotalDerivatives(derivatives);

}

void Net::propagateForward(){
    for(int i = 0; i < layers.size() - 1; i++){
        Layer currLayer = layers[i];
        Layer nextLayer = layers[i + 1];
        
        std::vector<double> inputs = currLayer.forwardPropagation();
        nextLayer.calculateInputs(inputs);
        
    }
}

void Net::propagateBackward(){
    int d = layers.size();

    for(int i = d - 1; i > 1; i--){
        Layer& curr = layers[i];
        Layer& prev = layers[i - 1];

        propagateBackwardForLayers(curr, prev);
    }
}

void Net::propagateBackwardForLayers(Layer& curr, Layer& prev){
    std::vector<double> currDerivatives = curr.getTotalDerivatives();
    prev.updateTotalDerivatives(currDerivatives);
    prev.adjustWeights(currDerivatives, alpha);
}

std::vector<double> Net::getOutputs(const std::vector<double>& inputs){
    setInput(inputs);
    propagateForward();
    return layers.back().getOutputs();
}