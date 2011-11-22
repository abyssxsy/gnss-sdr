/*!
 * \file tracking_2rd_PLL_filter.h
 * \brief Class that implements 2 order PLL filter for tracking carrier loop
 * \author Javier Arribas, 2011. jarribas(at)cttc.es
 *
 * Class that implements 2 order PLL filter for tracking carrier loop. The algorithm is described in [1]
 *
 * [1] K.Borre, D.M.Akos, N.Bertelsen, P.Rinder, and S.~H.~Jensen, A Software-Defined GPS and Galileo Receiver. A Single-Frequency Approach,
 * Birkhauser, 2007, Applied and Numerical Harmonic Analysis.
 *
 * -------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2011  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *          Satellite Systems receiver
 *
 * This file is part of GNSS-SDR.
 *
 * GNSS-SDR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.
 *
 * GNSS-SDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNSS-SDR. If not, see <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------
 */

#ifndef TRACKING_2RD_PLL_FILTER_H_
#define TRACKING_2RD_PLL_FILTER_H_

class tracking_2rd_PLL_filter
{
private:
    // PLL filter parameters
    float d_tau1_carr;
    float d_tau2_carr;
    float d_pdi_carr;

    float d_pllnoisebandwidth;
    float d_plldampingratio;

    float d_old_carr_error;
    float d_old_carr_nco;

    void calculate_lopp_coef(float* tau1,float* tau2, float lbw, float zeta, float k);
public:
	void set_PLL_BW(float pll_bw_hz);
	void initialize(float d_acq_carrier_doppler_hz);
	float get_carrier_nco(float PLL_discriminator);
	tracking_2rd_PLL_filter();
	~tracking_2rd_PLL_filter();
};

#endif