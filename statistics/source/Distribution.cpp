#include "Distribution.hpp"

#include "Statistics.hpp"


void Distribution::add_sample(const double sample)
{
    // Welford's online algorithm, from https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
    n_samples_ += 1;
    auto delta = sample - mean_;
    mean_ += delta / n_samples_;
    auto delta2 = sample - mean_;
    m2_ += delta * delta2;
}

std::pair<double, double> Distribution::confidence_interval(double p_value) const
{
    double val = Statistics::find_cdf_quantile(Statistics::get_two_sided_p_value(p_value), 0.01);
    return std::pair<double, double>{mean_ - val * std(), mean_ + val * std()};
}

std::pair<double, double> Distribution::confidence_interval_of_the_mean(double p_value) const
{
    double val = Statistics::find_cdf_quantile(Statistics::get_two_sided_p_value(p_value), 0.01);
    double std_ = std_of_the_mean();
    return std::pair<double, double>{mean_ - val * std_, mean_ + val * std_};
}

std::ostream& operator<<(std::ostream& os, const Distribution& d)
{
    return os << "mean = " << d.mean() << ", std_of_the_mean = " << d.std_of_the_mean() << ", samples = " << d.samples();
}

