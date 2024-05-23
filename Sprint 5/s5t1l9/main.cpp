    #include "log_duration.h"

    #include <iostream>
    #include <cassert>
    #include <numeric>
    #include <random>
    #include <string>
    #include <vector>

    using namespace std;

    vector<float> ComputeAvgTemp(const vector<vector<float>>& measures) {
        // Пустой вектор из пустого измерения
        if(measures.empty()){
            return {};
        }
        
        // размер вектора колличества измерений в день 
        const int measures_per_day = measures[0].size();
        
        // Размер вектора всех измерений
        const int total_measures = measures.size();

        // Сумма положительных температур по каждому измерению
        vector<float> sum_temp_mes(measures_per_day);

        // кол-во положительных темп-р
        vector<float> count_temp_mes(measures_per_day);

        // средние температуры
        vector<float>average_temp(measures_per_day);

        for(int i = 0; i < total_measures; ++i){
            for(int j = 0; j < measures_per_day; ++j){
                // Если темп-ра > 0 добавляем вектор,
                sum_temp_mes[i] += (measures[i][j] > 0 ? measures[i][j] : 0);
                count_temp_mes[i] += (measures[i][j] > 0 ? 1 : 0);
            }
            average_temp[i] += (count_temp_mes[i] > 0 ? sum_temp_mes[i] / count_temp_mes[i] : 0);
        }  
        return average_temp;
    }

    vector<float> GetRandomVector(int size) {
        static mt19937 engine;
        uniform_real_distribution<float> d(-100, 100);

        vector<float> res(size);
        for (int i = 0; i < size; ++i) {
            res[i] = d(engine);
        }
        return res;
    }
    void Test() {
        // 4 дня по 3 измерения
        vector<vector<float>> v = {
            {}, // 
            {-1, -2, -2}, // 0
            {2, 3, -3}, // 2.5
            {3.45, 4.31, -4} // 3.88
        };

        // среднее для 0-го измерения (1+2+3) / 3 = 2 (не учитывам 0)
        // среднее для 1-го измерения (3+4) / 2 = 3.5 (не учитывам -1, -2)
        // среднее для 2-го не определено (все температуры отрицательны), поэтому должен быть 0

        assert(ComputeAvgTemp(v) == vector<float>({0, 2.5f, 3.88f}));
    } 

    int main() {
        //Test();
        vector<vector<float>> data;
        data.reserve(5000);

        for (int i = 0; i < 5000; ++i) {
            data.push_back(GetRandomVector(5000));
        }

        vector<float> avg;
        {
            LOG_DURATION("ComputeAvgTemp"s);
            avg = ComputeAvgTemp(data);
        }

        cout << "Total mean: "s << accumulate(avg.begin(), avg.end(), 0.f) / avg.size() << endl;
    }