#include "Distribution.hpp"

#include "Statistics.hpp"


void Distribution::add_sample(const double sample)
{
    // from https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
    n_samples_ += 1;
    auto delta = sample - mean_;
    mean_ += delta / n_samples_;
    auto delta2 = sample - mean_;
    m2_ += delta * delta2;
}

void Distribution::reset()
{
    prepare(0, 0, 0);
}

void Distribution::prepare(int n_samples, double mean, double variance)
{
    n_samples_ = n_samples;
    mean_ = mean;
    m2_ = variance * n_samples;
}

double Distribution::std_() const
{
    return std::sqrt(m2_ / n_samples_);
}

double Distribution::std_of_the_mean() const
{
    return Statistics::sample_deviation(std_(), n_samples_);
}

std::pair<double, double> Distribution::confidence_interval(double p_value) const
{
    double val = Statistics::find_cdf_quantile(Statistics::get_two_sided_p_value(p_value), 0.01);
    return std::pair<double, double>{mean_ - val * std_(), mean_ + val * std_()};
}

std::pair<double, double> Distribution::confidence_interval_of_the_mean(double p_value) const
{
    double val = Statistics::find_cdf_quantile(Statistics::get_two_sided_p_value(p_value), 0.01);
    double std_ = std_of_the_mean();
    return std::pair<double, double>{mean_ - val * std_, mean_ + val * std_};
}

std::ostream& operator<<(std::ostream& os, const Distribution& d) {
    return os << d.mean() << " +/- " << d.variance() << " / " << d.sample_variance() << " for " << d.samples() << " samples";
}

