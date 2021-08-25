#ifndef WOW_SIMULATOR_DISTRIBUTION_HPP
#define WOW_SIMULATOR_DISTRIBUTION_HPP

#include <utility>
#include <iostream>

class Distribution
{
public:
    Distribution() = default;

    void add_sample(double sample);

    void reset();
    void prepare(int n_samples, double mean, double variance);

    [[nodiscard]] double std_() const;
    [[nodiscard]] double std_of_the_mean() const;

    [[nodiscard]] std::pair<double, double> confidence_interval(double quantile) const;
    [[nodiscard]] std::pair<double, double> confidence_interval_of_the_mean(double quantile) const;

    [[nodiscard]] int samples() const { return n_samples_; }
    [[nodiscard]] double mean() const { return mean_; }
    [[nodiscard]] double variance() const { return m2_ / n_samples_; }
    [[nodiscard]] double sample_variance() const { return m2_ / (n_samples_ - 1); }
private:
    int n_samples_;
    double mean_;
    double m2_;
};

std::ostream& operator<<(std::ostream& os, const Distribution& d);

#endif // WOW_SIMULATOR_DISTRIBUTION_HPP
