////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LSTMLayerNode.h (nodes)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "NeuralNetworkLayerNode.h"

// model
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "PortElements.h"

// predictors
#include "LSTMLayer.h"

// stl
#include <string>
#include <type_traits>

namespace ell
{
namespace nodes
{
    /// <summary> A node that wraps a neural net GRULayer. </summary>
    template <typename ValueType>
    class LSTMLayerNode : public NeuralNetworkLayerNode<LSTMLayerNode<ValueType>, predictors::neural::LSTMLayer<ValueType>, ValueType>
    {
    public:
        using LayerType = predictors::neural::LSTMLayer<ValueType>;
        using BaseType = NeuralNetworkLayerNode<LSTMLayerNode<ValueType>, LayerType, ValueType>;

        /// @name Input and Output Ports
        /// @{
        using BaseType::input;
        const model::InputPort<int>& reset = _reset;
        using BaseType::output;
        /// @}

        /// <summary> Default constructor. </summary>
        LSTMLayerNode();

        /// <summary> Constructor from a layer. </summary>
        ///
        /// <param name="input"> The input signal </param>
        /// <param name="reset"> The reset signal (will reset when this boolean transitions from 1 to 0) </param>
        /// <param name="layer"> The bias layer to wrap. </param>
        LSTMLayerNode(const model::PortElements<ValueType>& input, const model::PortElements<int>& reset, const LayerType& layer);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("LSTMLayerNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        ///
        /// <returns> true if this node is able to compile itself to code. </returns>
        bool IsCompilable(const model::MapCompiler* compiler) const override { return false; }

    protected:
        bool Refine(model::ModelTransformer& transformer) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Reset input signal
        model::InputPort<int> _reset;
    };

    //
    // Implementation: LSTMNode
    //
    template<typename ValueType>
    class LSTMNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputWeightsPortName = "inputWeights";
        static constexpr const char* resetTriggerPortName = "resetTrigger";
        static constexpr const char* forgetMeWeightsPortName = "forgetMeWeights";
        static constexpr const char* candidateWeightsPortName = "candidateWeights";
        static constexpr const char* outputWeightsPortName = "outputWeights";
        static constexpr const char* inputBiasPortName = "inputBias";
        static constexpr const char* forgetMeBiasPortName = "forgetMeBias";
        static constexpr const char* candidateBiasPortName = "candidateBias";
        static constexpr const char* outputBiasPortName = "outputBias";
        const model::InputPort<ValueType>& input = _input;
        const model::InputPort<ValueType>& inputWeights = _inputWeights;
        const model::InputPort<ValueType>& forgetMeWeights = _forgetMeWeights;
        const model::InputPort<ValueType>& candidateWeights = _candidateWeights;
        const model::InputPort<ValueType>& outputWeights = _outputWeights;
        const model::InputPort<ValueType>& inputBias = _inputBias;
        const model::InputPort<ValueType>& forgetMeBias = _forgetMeBias;
        const model::InputPort<ValueType>& candidateBias = _candidateBias;
        const model::InputPort<ValueType>& outputBias = _outputBias;
        const model::InputPort<int>& resetTrigger = _resetTrigger;
        const model::OutputPort<ValueType>& output = _output;
        using ActivationType = predictors::neural::Activation<ValueType>;
        /// @}

        /// <summary> Default contructor. </summary>
        LSTMNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="resetTrigger"> Port elements for the reset trigger. </param>
        /// <param name="inputWeights"> The weights to be applied to the input layer. </param>
        /// <param name="forgetMeWeights"> The weights to be applied to the forgotten layer. </param>
        /// <param name="candidateWeights"> The weights to be applied to the candidate layer. </param>
        /// <param name="outputWeights"> The weights to be applied to the output layer. </param>
        /// <param name="inputBias"> The bias to be applied to the input nodes. </param>
        /// <param name="forgetMeBias"> The bias to be applied to the forgotten layer nodes. </param>
        /// <param name="candidateBias"> The bias to be applied to the candidate nodes. </param>
        /// <param name="outputBias"> The bias to be applied to the output nodes. </param>
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        LSTMNode(const model::PortElements<ValueType>& input,
                        const model::PortElements<int>& resetTrigger,
                        const model::PortElements<ValueType>& inputWeights,
                        const model::PortElements<ValueType>& forgetMeWeights,
                        const model::PortElements<ValueType>& candidateWeights,
                        const model::PortElements<ValueType>& outputWeights,
                        const model::PortElements<ValueType>& inputBias,
                        const model::PortElements<ValueType>& forgetMeBias,
                        const model::PortElements<ValueType>& candidateBias,
                        const model::PortElements<ValueType>& outputBias,
                        const ActivationType& activation,
                        const ActivationType& recurrentActivation,
                        const model::PortMemoryLayout& inputMemoryLayout,
                        const model::PortMemoryLayout& outputMemoryLayout);

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets information about the output memory layout </summary>
        model::PortMemoryLayout GetOutputMemoryLayout() const { return _output.GetMemoryLayout(); }

        /// <summary> Returns true if the node can accept input with this memory layout order, else false </summary>
        ///
        /// <param name="order"> The memory layout order for all the input ports </summary>
        /// <returns> If the node can accept the input memory layout order, true, else false </returns>
        bool CanAcceptInputLayout(const utilities::DimensionOrder& order) const override
        {
            return GetInputMemoryLayout().GetLogicalDimensionOrder() == order;
        }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("LSTMNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Resets any state on the node, if any </summary>
        virtual void Reset() override;

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool HasState() const override { return true; }

        void WriteToArchive(utilities::Archiver& archiver) const override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        void ReadFromArchive(utilities::Unarchiver& archiver) override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Input
        model::InputPort<ValueType> _input;

        // Reset input
        model::InputPort<int> _resetTrigger;

        // Weights
        model::InputPort<ValueType> _inputWeights;
        model::InputPort<ValueType> _forgetMeWeights;
        model::InputPort<ValueType> _candidateWeights;
        model::InputPort<ValueType> _outputWeights;

        // Biases
        model::InputPort<ValueType> _inputBias;
        model::InputPort<ValueType> _forgetMeBias;
        model::InputPort<ValueType> _candidateBias;
        model::InputPort<ValueType> _outputBias;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;
        ActivationType _activation;
        ActivationType _recurrentActivation;

        void ApplySoftmax(emitters::IRFunctionEmitter& function, llvm::Value* data, size_t dataLength);

        void ApplyActivation(emitters::IRFunctionEmitter& function, const ActivationType& activation, llvm::Value* data, size_t dataLength);
    };
}
}
