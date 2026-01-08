#include "NeuralNetwork.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

struct Normalizer {
    double min_val;
    double max_val;
};

static double normalize(double value, const Normalizer& norm) {
    if (norm.max_val - norm.min_val == 0.0) {
        return 0.0;
    }
    return (value - norm.min_val) / (norm.max_val - norm.min_val);
}

static double denormalize(double value, const Normalizer& norm) {
    return value * (norm.max_val - norm.min_val) + norm.min_val;
}

int main() {
    // 读取数据文件
    std::cout << "请输入数据文件路径（每行: x\t y）: ";
    std::string file_path;
    std::getline(std::cin, file_path);

    std::ifstream file(file_path.c_str());
    if (!file.is_open()) {
        std::cout << "无法打开文件: " << file_path << std::endl;
        return 1;
    }

    // 解析数据（输入x、输出y）
    std::vector<double> xs;
    std::vector<double> ys;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        std::stringstream ss(line);
        double x = 0.0;
        double y = 0.0;
        ss >> x >> y;
        xs.push_back(x);
        ys.push_back(y);
    }

    if (xs.empty()) {
        std::cout << "数据为空。" << std::endl;
        return 1;
    }

    std::cout << "读取数据完成: 样本数=" << xs.size() << " 特征维度=1" << std::endl;

    // 计算归一化所需的最小值与最大值
    Normalizer x_norm = { xs[0], xs[0] };
    Normalizer y_norm = { ys[0], ys[0] };
    for (size_t i = 1; i < xs.size(); ++i) {
        if (xs[i] < x_norm.min_val) x_norm.min_val = xs[i];
        if (xs[i] > x_norm.max_val) x_norm.max_val = xs[i];
        if (ys[i] < y_norm.min_val) y_norm.min_val = ys[i];
        if (ys[i] > y_norm.max_val) y_norm.max_val = ys[i];
    }

    // 构建归一化后的训练样本
    std::vector<Vector> inputs;
    std::vector<Vector> targets;
    for (size_t i = 0; i < xs.size(); ++i) {
        Vector in(1, normalize(xs[i], x_norm));
        Vector out(1, normalize(ys[i], y_norm));
        inputs.push_back(in);
        targets.push_back(out);
    }

    std::cout << "数据归一化完成" << std::endl;

    // 配置网络结构与训练参数
    int hidden_layers = 0;
    std::cout << "请输入隐藏层数量(0-3): ";
    std::cin >> hidden_layers;
    if (hidden_layers < 0) hidden_layers = 0;
    if (hidden_layers > 3) hidden_layers = 3;

    std::vector<int> layer_sizes;
    layer_sizes.push_back(1); // 输入层
    for (int i = 0; i < hidden_layers; ++i) {
        int neurons = 0;
        std::cout << "请输入隐藏层" << (i + 1) << "神经元数量(<=10): ";
        std::cin >> neurons;
        if (neurons < 1) neurons = 1;
        if (neurons > 10) neurons = 10;
        layer_sizes.push_back(neurons);
    }
    layer_sizes.push_back(1); // 输出层

    double learning_rate = 0.01;
    std::cout << "请输入学习率(例如0.05): ";
    std::cin >> learning_rate;
    if (learning_rate <= 0.0) learning_rate = 0.01;

    int epochs = 200;
    std::cout << "请输入训练轮次(>=200): ";
    std::cin >> epochs;
    if (epochs < 200) epochs = 200;

    std::vector<ActivationType> activations;
    for (int i = 0; i < hidden_layers; ++i) {
        activations.push_back(RELU);
    }
    activations.push_back(LINEAR);

    DynamicNN model;
    model.configure(layer_sizes, activations, learning_rate);

    std::cout << "\n网络结构:" << std::endl;
    std::cout << "输入层神经元数: 1" << std::endl;
    for (int i = 0; i < hidden_layers; ++i) {
        std::cout << "隐藏层" << (i + 1) << "神经元数: " << layer_sizes[i + 1] << " | 激活函数: ReLU" << std::endl;
    }
    std::cout << "输出层神经元数: 1 | 激活函数: Linear" << std::endl;
    std::cout << "学习率: " << model.getLearningRate() << std::endl;

    // 训练网络
    std::cout << "\n开始训练..." << std::endl;
    for (int epoch = 0; epoch <= epochs; ++epoch) {
        double loss_sum = 0.0;
        for (size_t i = 0; i < inputs.size(); ++i) {
            model.trainSample(inputs[i], targets[i]);
            Vector pred = model.forward(inputs[i]);
            double diff = pred[0] - targets[i][0];
            loss_sum += diff * diff;
        }
        double avg_loss = loss_sum / inputs.size();
        if (epoch % 100 == 0 || epoch == epochs) {
            std::cout << "Epoch: " << epoch << " | 平均损失: " << std::fixed << std::setprecision(6) << avg_loss << std::endl;
        }
    }

    // 预测与误差计算
    std::cout << "训练完成，开始预测。" << std::endl;
    double input_x = 0.0;
    std::cout << "请输入x进行预测: ";
    std::cin >> input_x;
    Vector input_vec(1, normalize(input_x, x_norm));
    Vector pred = model.forward(input_vec);
    double pred_y = denormalize(pred[0], y_norm);
    std::cout << "预测值是: " << std::fixed << std::setprecision(6) << pred_y << std::endl;

    double actual_y = 0.0;
    std::cout << "请输入对应的真实值进行对比: ";
    std::cin >> actual_y;
    double error = std::fabs(pred_y - actual_y);
    std::cout << "误差是: " << std::fixed << std::setprecision(6) << error << std::endl;

    return 0;
}
