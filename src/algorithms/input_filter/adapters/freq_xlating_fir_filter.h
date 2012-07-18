/*!
 * \file freq_xlating_fir_filter.h
 * \brief Adapts a gnuradio gr_freq_xlating_fir_filter designed with gr_remez
 * \author Luis Esteve, 2012. luis(at)epsilon-formacion.com
 *
 * Detailed description of the file here if needed.
 *
 * -------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2012  (see AUTHORS file for a list of contributors)
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

#ifndef GNSS_SDR_FREQ_XLATING_FIR_FILTER_H_
#define GNSS_SDR_FREQ_XLATING_FIR_FILTER_H_

#include "gnss_synchro.h"
#include "gnss_block_interface.h"
#include <gnuradio/gr_freq_xlating_fir_filter_ccc.h>
#include <gnuradio/gr_freq_xlating_fir_filter_ccf.h>
#include <gnuradio/gr_freq_xlating_fir_filter_fcc.h>
#include <gnuradio/gr_freq_xlating_fir_filter_scc.h>
#include <gnuradio/gr_msg_queue.h>

class ConfigurationInterface;

/*!
 * \brief This class adapts a gnuradio gr_freq_xlating_fir_filter designed with gr_remez
 *
 * Construct a FIR filter with the given taps and a composite frequency
 * translation that shifts center_freq down to zero Hz.  The frequency
 * translation logically comes before the filtering operation.
 *
 * See Parks-McClellan FIR filter design, http://en.wikipedia.org/wiki/Parks-McClellan_filter_design_algorithm
 * Calculates the optimal (in the Chebyshev/minimax sense) FIR filter impulse response
 * given a set of band edges, the desired response on those bands, and the weight given
 * to the error in those bands.
 */
class FreqXlatingFirFilter: public GNSSBlockInterface
{

public:

    FreqXlatingFirFilter(ConfigurationInterface* configuration,
            std::string role, unsigned int in_streams,
            unsigned int out_streams, gr_msg_queue_sptr queue);

    virtual ~FreqXlatingFirFilter();

    std::string role()
    {
        return role_;
    }
    std::string implementation()
    {
        return "Freq_Xlating_Fir_Filter";
    }
    size_t item_size()
    {
        return 0;
    }

    void connect(gr_top_block_sptr top_block);
    void disconnect(gr_top_block_sptr top_block);
    gr_basic_block_sptr get_left_block();
    gr_basic_block_sptr get_right_block();


private:

    gr_freq_xlating_fir_filter_ccf_sptr freq_xlating_fir_filter_ccf_;
    ConfigurationInterface* config_;
    bool dump_;
    std::string dump_filename_;
    std::string input_item_type_;
    std::string output_item_type_;
    std::string taps_item_type_;
    std::vector <float> taps_;
    double center_freq_;
    double sampling_freq_;
    std::string role_;
    unsigned int in_streams_;
    unsigned int out_streams_;
    gr_msg_queue_sptr queue_;
    gr_block_sptr file_sink_;
    void init();
};

#endif // GNSS_SDR_FREQ_XLATING_FIR_FILTER_H_