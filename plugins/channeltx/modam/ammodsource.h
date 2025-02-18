///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 Edouard Griffiths, F4EXB                                   //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
// (at your option) any later version.                                           //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDE_AMMODSOURCE_H
#define INCLUDE_AMMODSOURCE_H

#include <QMutex>

#include <iostream>
#include <fstream>

#include "dsp/channelsamplesource.h"
#include "dsp/nco.h"
#include "dsp/ncof.h"
#include "dsp/interpolator.h"
#include "util/movingaverage.h"
#include "dsp/cwkeyer.h"
#include "audio/audiofifo.h"

#include "ammodsettings.h"

class AMModSource : public ChannelSampleSource
{
public:
    AMModSource();
    virtual ~AMModSource();

    virtual void pull(SampleVector::iterator begin, unsigned int nbSamples);
    virtual void pullOne(Sample& sample);
    virtual void prefetch(unsigned int nbSamples);

    void setInputFileStream(std::ifstream *ifstream) { m_ifstream = ifstream; }
    AudioFifo *getAudioFifo() { return &m_audioFifo; }
    AudioFifo *getFeedbackAudioFifo() { return &m_feedbackAudioFifo; }
    void applyAudioSampleRate(unsigned int sampleRate);
    void applyFeedbackAudioSampleRate(unsigned int sampleRate);
    unsigned int getAudioSampleRate() const { return m_audioSampleRate; }
    unsigned int getFeedbackAudioSampleRate() const { return m_feedbackAudioSampleRate; }
    CWKeyer& getCWKeyer() { return m_cwKeyer; }
    double getMagSq() const { return m_magsq; }
    void getLevels(Real& rmsLevel, Real& peakLevel, Real& numSamples) const
    {
        rmsLevel = m_rmsLevel;
        peakLevel = m_peakLevel;
        numSamples = m_levelNbSamples;
    }
    void applySettings(const AMModSettings& settings, bool force = false);
    void applyChannelSettings(int channelSampleRate, int channelFrequencyOffset, bool force = false);

private:
    int m_channelSampleRate;
    int m_channelFrequencyOffset;
    AMModSettings m_settings;

    NCO m_carrierNco;
    NCOF m_toneNco;
    Complex m_modSample;

    Interpolator m_interpolator;
    Real m_interpolatorDistance;
    Real m_interpolatorDistanceRemain;
    bool m_interpolatorConsumed;

    Interpolator m_feedbackInterpolator;
    Real m_feedbackInterpolatorDistance;
    Real m_feedbackInterpolatorDistanceRemain;
    bool m_feedbackInterpolatorConsumed;

    double m_magsq;
    MovingAverageUtil<double, double, 16> m_movingAverage;

    quint32 m_audioSampleRate;
    AudioVector m_audioBuffer;
    uint m_audioBufferFill;
    AudioFifo m_audioFifo;

    quint32 m_feedbackAudioSampleRate;
    AudioVector m_feedbackAudioBuffer;
    uint m_feedbackAudioBufferFill;
    AudioFifo m_feedbackAudioFifo;

    quint32 m_levelCalcCount;
    Real m_rmsLevel;
    Real m_peakLevelOut;
    Real m_peakLevel;
    Real m_levelSum;

    std::ifstream *m_ifstream;
    CWKeyer m_cwKeyer;

    static const int m_levelNbSamples;

    void processOneSample(Complex& ci);
    void pullAF(Real& sample);
    void pullAudio(unsigned int nbSamples);
    void pushFeedback(Real sample);
    void calculateLevel(Real& sample);
    void modulateSample();
};



#endif // INCLUDE_AMMODSOURCE_H
