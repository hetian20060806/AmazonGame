#pragma once
#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <vector>
#include <string>

// 激活函数类型
enum ActivationType {
    LINEAR, // 线性激活（回归任务）
    RELU,   // ReLU（可选，用于隐藏层）
    TANH    // Tanh（用于拟合平滑曲线）
};

// 矩阵与向量类型定义（简化实现）
using Matrix = std::vector<std::vector<double> >;
using Vector = std::vector<double>;

class DynamicNN {
private:
    std::vector<int> layer_sizes;              // 每层神经元数（输入层+隐藏层+输出层）
    std::vector<ActivationType> activations;  // 每层激活函数（不含输入层）
    std::vector<Matrix> weights;              // 权重矩阵（layer[i] -> layer[i+1]）
    std::vector<Vector> biases;               // 偏置向量
    double lr;                                 // 学习率

    // 前向传播中间结果（用于反向传播）
    std::vector<Vector> layer_inputs;
    std::vector<Vector> layer_outputs;

    double activate(double x, ActivationType type) const;
    double activateDerivative(double x, ActivationType type) const;

public:
    DynamicNN();
    void configure(const std::vector<int>& sizes,
                   const std::vector<ActivationType>& acts,
                   double learning_rate);

    Vector forward(const Vector& input);
    void trainSample(const Vector& input, const Vector& target);
    void trainBatch(const std::vector<Vector>& inputs,
                    const std::vector<Vector>& targets);
    double getLearningRate() const { return lr; }
    const std::vector<int>& getLayerSizes() const { return layer_sizes; }
    const std::vector<ActivationType>& getActivations() const { return activations; }
};

#endif
