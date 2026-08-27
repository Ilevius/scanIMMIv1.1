#pragma once
extern "C" {
	int RealPolesPlot(const char* app, const char* waveguide);
	int InitWithH(int *alfas_n, int *freqs_n, double* alfas, double* freqs, double* H_re, double* H_im);
}