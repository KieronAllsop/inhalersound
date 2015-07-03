#pragma once

#include <vector>

class TGaussian
{
public:
	float Weight;
	std::vector<float> Mean;
	std::vector<float> Covariance;
	std::vector<float> Determinant;
};

class TGaussianMixture
{
public:
	int nGaussians;
	std::vector<TGaussian> Gaussians;
};
