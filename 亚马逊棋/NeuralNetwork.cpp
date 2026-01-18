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
        double scale = 1.0;
        if (activations[i] == RELU) {
            scale = std::sqrt(2.0 / static_cast<double>(in_size));
        } else {
            scale = std::sqrt(1.0 / static_cast<double>(in_size));
        }

        for (int r = 0; r < out_size; ++r) {
            for (int c = 0; c < in_size; ++c) {
                double rand_weight = (std::rand() / (double)RAND_MAX) * 2.0 - 1.0;
                w[r][c] = rand_weight * scale;
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
    if (type == TANH) {
        return std::tanh(x);
    }
    return x;
}

double DynamicNN::activateDerivative(double x, ActivationType type) const {
    if (type == RELU) {
        return x > 0.0 ? 1.0 : 0.0;
    }
    if (type == TANH) {
        double t = std::tanh(x);
        return 1.0 - t * t;
    }
    return 1.0;
}

Vector DynamicNN::forward(const Vector& input) {
    layer_inputs.clear();
    layer_outputs.clear();
    layer_outputs.push_back(input);

    Vector current = input;
    for (size_t layer = 0; layer < weights.size(); ++layer) {
        int out_size = static_cast<int>(weights[layer].size());
        Vector next(out_size, 0.0);
        Vector z(out_size, 0.0);
        for (int j = 0; j < out_size; ++j) {
            double sum = biases[layer][j];
            for (size_t i = 0; i < current.size(); ++i) {
                sum += weights[layer][j][i] * current[i];
            }
            z[j] = sum;
            next[j] = activate(sum, activations[layer]);
        }
        layer_inputs.push_back(z);
        current = next;
        layer_outputs.push_back(current);
    }
    return current;
}

void DynamicNN::trainSample(const Vector& input, const Vector& target) {
    std::vector<Vector> inputs(1, input);
    std::vector<Vector> targets(1, target);
    trainBatch(inputs, targets);
}

void DynamicNN::trainBatch(const std::vector<Vector>& inputs,
                           const std::vector<Vector>& targets) {
    if (inputs.empty()) {
        return;
    }
    size_t layer_count = weights.size();
    std::vector<Matrix> grad_w(layer_count);
    std::vector<Vector> grad_b(layer_count);
    for (size_t layer = 0; layer < layer_count; ++layer) {
        grad_w[layer] = Matrix(weights[layer].size(),
                               Vector(weights[layer][0].size(), 0.0));
        grad_b[layer] = Vector(weights[layer].size(), 0.0);
    }

    for (size_t sample = 0; sample < inputs.size(); ++sample) {
        Vector output = forward(inputs[sample]);
        std::vector<Vector> deltas(layer_count);

        size_t last = layer_count - 1;
        deltas[last] = Vector(output.size(), 0.0);
        for (size_t j = 0; j < output.size(); ++j) {
            double error = output[j] - targets[sample][j];
            double deriv = activateDerivative(layer_inputs[last][j], activations[last]);
            deltas[last][j] = error * deriv;
        }

        for (int layer = static_cast<int>(layer_count) - 1; layer >= 0; --layer) {
            if (layer > 0) {
                int prev_size = layer_sizes[layer];
                deltas[layer - 1] = Vector(prev_size, 0.0);
                for (int i = 0; i < prev_size; ++i) {
                    double sum = 0.0;
                    for (size_t j = 0; j < deltas[layer].size(); ++j) {
                        sum += weights[layer][j][i] * deltas[layer][j];
                    }
                    double deriv = activateDerivative(layer_inputs[layer - 1][i], activations[layer - 1]);
                    deltas[layer - 1][i] = sum * deriv;
                }
            }

            for (size_t j = 0; j < weights[layer].size(); ++j) {
                for (size_t i = 0; i < weights[layer][j].size(); ++i) {
                    grad_w[layer][j][i] += deltas[layer][j] * layer_outputs[layer][i];
                }
                grad_b[layer][j] += deltas[layer][j];
            }
        }
    }

    double scale = lr / static_cast<double>(inputs.size());
    for (size_t layer = 0; layer < layer_count; ++layer) {
        for (size_t j = 0; j < weights[layer].size(); ++j) {
            for (size_t i = 0; i < weights[layer][j].size(); ++i) {
                weights[layer][j][i] -= scale * grad_w[layer][j][i];
            }
            biases[layer][j] -= scale * grad_b[layer][j];
        }
    }
}
