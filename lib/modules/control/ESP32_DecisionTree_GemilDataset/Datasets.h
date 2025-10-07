#ifndef DATASETS_H
#define DATASETS_H

const int MAX_SAMPLES = 27;

struct WaterData {
    char ph[10];
    char tds[10];
    char uv[10];
    char microplastic[10];
};

WaterData dataset[MAX_SAMPLES] = {
    {"Asam", "Baik", "Sedikit", "Rendah"},
    {"Asam", "Baik", "Sedang", "Rendah"},
    {"Asam", "Baik", "Banyak", "Sedang"},
    {"Asam", "Normal", "Sedang", "Sedang"},
    {"Asam", "Normal", "Banyak", "Sedang"},
    {"Asam", "Normal", "Sedikit", "Rendah"},
    {"Asam", "Buruk", "Banyak", "Tinggi"},
    {"Asam", "Buruk", "Sedang", "Tinggi"},
    {"Asam", "Buruk", "Sedikit", "Sedang"},
    {"Normal", "Baik", "Sedikit", "Rendah"},
    {"Normal", "Baik", "Sedang", "Rendah"},
    {"Normal", "Baik", "Banyak", "Sedang"},
    {"Normal", "Normal", "Sedikit", "Rendah"},
    {"Normal", "Normal", "Sedang", "Sedang"},
    {"Normal", "Normal", "Banyak", "Sedang"},
    {"Normal", "Buruk", "Sedikit", "Sedang"},
    {"Normal", "Buruk", "Sedang", "Tinggi"},
    {"Normal", "Buruk", "Banyak", "Tinggi"},
    {"Basa", "Baik", "Sedikit", "Rendah"},
    {"Basa", "Baik", "Sedang", "Rendah"},
    {"Basa", "Baik", "Banyak", "Sedang"},
    {"Basa", "Normal", "Sedikit", "Rendah"},
    {"Basa", "Normal", "Sedang", "Sedang"},
    {"Basa", "Normal", "Banyak", "Sedang"},
    {"Basa", "Buruk", "Sedikit", "Sedang"},
    {"Basa", "Buruk", "Sedang", "Tinggi"},
    {"Basa", "Buruk", "Banyak", "Tinggi"}
};

const int datasetSize = 27;

#endif