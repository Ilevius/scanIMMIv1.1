#pragma once
//extern "C" void txFourier(size_t t_n, size_t freq_n, size_t x_n, size_t alfa_n, double t_min, double freq_min, double x_min, double alfa_min, 
//	double t_step, double freq_step, double x_step, double alfa_step, double signals, double H_re, double H_im);

extern "C" {

    void txFourier(
        int* t_n,
        int* freq_n,
        int* x_n,
        int* alfa_n,
        double* t_min,
        double* freq_min,
        double* x_min,
        double* alfa_min,
        double* t_step,
        double* freq_step,
        double* x_step,
        double* alfa_step,
        const double* signals,  // intent(in)
        double* H_re,     // intent(inout)
        double* H_im,      // intent(inout)
		double* filter_t, // intent(in)
		double* filter_x      // intent(in)
    );

    void xtFourier(
        int* t_n,
        int* freq_n,
        int* x_n,
        int* alfa_n,
        double* t_min,
        double* freq_min,
        double* x_min,
        double* alfa_min,
        double* t_step,
        double* freq_step,
        double* x_step,
        double* alfa_step,
        const double* signals,  // intent(in)
        double* H_re,     // intent(inout)
        double* H_im,      // intent(inout)
		double* filter_t, // intent(in)
		double* filter_x      // intent(in)
    );

    void MPMxF(
        int* x_n,
        int* alfa_n,
        int* t_n,
        double* x_min,
        double* alfa_min,
        double* t_min,
        double* x_step,
        double* alfa_step,
        double* t_step,
        int* lambda,
        double* delta,
        double* filter_x,
        const double* signals,
        double* zetas_re,
        double* zetas_im,
        int* mu
    );
}