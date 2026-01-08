#include "NeuralNetwork.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

DynamicNN::DynamicNN() : lr(0.01) {
    std::srand(static_cast<unsigned>(std::time(NULL)));
}

void DynamicNN::configure(const std::vector<int>& sizes,
                          const std::vector<ActivationType>& acts,
                          double learning_rate) {
    layer_sizes = sizes;
    activations = acts;
    lr = learning_rate;

    weights.clear();
    biases.clear();

    for (size_t i = 0; i + 1 < layer_sizes.size(); ++i) {
        int in_size = layer_sizes[i];
        int out_size = layer_sizes[i + 1];
        Matrix w(out_size, Vector(in_size));
        Vector b(out_size, 0.0);

        for (int r = 0; r < out_size; ++r) {
            for (int c = 0; c < in_size; ++c) {
                double rand_weight = (std::rand() / (double)RAND_MAX) * 2.0 - 1.0;
                w[r][c] = rand_weight * 0.5;
            }
            b[r] = 0.0;
        }
        weights.push_back(w);
        biases.push_back(b);
    }
}

double DynamicNN::activate(double x, ActivationType type) const {
    if (type == RELU) {
        return x > 0.0 ? x : 0.0;
    }
    return x;
}

double DynamicNN::activateDerivative(double x, ActivationType type) const {
    if (type == RELU) {
        return x > 0.0 ? 1.0 : 0.0;
    }
    return 1.0;
}

Vector DynamicNN::forward(const Vector& input) {
    layer_outputs.clear();
    layer_outputs.push_back(input);

    Vector current = input;
    for (size_t layer = 0; layer < weights.size(); ++layer) {
        int out_size = static_cast<int>(weights[layer].size());
        Vector next(out_size, 0.0);
        for (int j = 0; j < out_size; ++j) {
            double sum = biases[layer][j];
            for (size_t i = 0; i < current.size(); ++i) {
                sum += weights[layer][j][i] * current[i];
            }
            next[j] = activate(sum, activations[layer]);
        }
        current = next;
        layer_outputs.push_back(current);
    }
    return current;
}

void DynamicNN::trainSample(const Vector& input, const Vector& target) {
    Vector output = forward(input);
    size_t layer_count = weights.size();
    std::vector<Vector> deltas(layer_count);

    // 输出层误差
    size_t last = layer_count - 1;
    deltas[last] = Vector(output.size(), 0.0);
    for (size_t j = 0; j < output.size(); ++j) {
        double error = output[j] - target[j];
        double deriv = activateDerivative(output[j], activations[last]);
        deltas[last][j] = error * deriv;
    }

    // 反向传播
    for (int layer = static_cast<int>(layer_count) - 1; layer >= 0; --layer) {
        Vector prev_output = layer_outputs[layer];

        // 计算上一层误差（若存在）
        if (layer > 0) {
            int prev_size = layer_sizes[layer];
            deltas[layer - 1] = Vector(prev_size, 0.0);
            for (int i = 0; i < prev_size; ++i) {
                double sum = 0.0;
                for (size_t j = 0; j < deltas[layer].size(); ++j) {
                    sum += weights[layer][j][i] * deltas[layer][j];
                }
                double deriv = activateDerivative(layer_outputs[layer][i], activations[layer - 1]);
                deltas[layer - 1][i] = sum * deriv;
            }
        }

        // 更新权重与偏置
        for (size_t j = 0; j < weights[layer].size(); ++j) {
            for (size_t i = 0; i < weights[layer][j].size(); ++i) {
                weights[layer][j][i] -= lr * deltas[layer][j] * prev_output[i];
            }
            biases[layer][j] -= lr * deltas[layer][j];
        }
    }
}
