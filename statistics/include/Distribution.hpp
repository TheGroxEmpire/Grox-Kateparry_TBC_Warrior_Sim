#ifndef WOW_SIMULATOR_DISTRIBUTION_HPP
#define WOW_SIMULATOR_DISTRIBUTION_HPP

#include <utility>
#include <ostream>
#include <cmath>

class Distribution
{
public:
    void add_sample(double sample);

    void add(const Distribution& other);

    [[nodiscard]] int samples() const { return n_samples_; }
    [[nodiscard]] double mean() const { return mean_; }

    [[nodiscard]] double variance() const { return m2_ / n_samples_; }
    [[nodiscard]] double std() const { return std::sqrt(m2_ / n_samples_); }

    [[nodiscard]] double var_of_the_mean() const { return m2_ / (static_cast<double>(n_samples_) * n_samples_); }
    [[nodiscard]] double std_of_the_mean() const { return std::sqrt(m2_) / n_samples_; }

    [[nodiscard]] double last_sample() const { return last_sample_; }

    [[nodiscard]] std::pair<double, double> confidence_interval(double quantile) const;
    [[nodiscard]] std::pair<double, double> confidence_interval_of_the_mean(double quantile) const;
private:
    int n_samples_;
    double mean_;
    double m2_;

    double last_sample_;
};

std::ostream& operator<<(std::ostream& os, const Distribution& d);

#endif // WOW_SIMULATOR_DISTRIBUTION_HPP
