///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 Edouard Griffiths, F4EXB                                   //
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

#ifndef INCLUDE_CHANALYZERNG_H
#define INCLUDE_CHANALYZERNG_H

#include <QMutex>
#include <vector>

#include "dsp/basebandsamplesink.h"
#include "channel/channelapi.h"
#include "dsp/interpolator.h"
#include "dsp/ncof.h"
#include "dsp/fftcorr.h"
#include "dsp/fftfilt.h"
#include "dsp/phaselockcomplex.h"
#include "dsp/freqlockcomplex.h"
#include "audio/audiofifo.h"
#include "util/message.h"
#include "util/movingaverage.h"

#include "chanalyzersettings.h"

#define ssbFftLen 1024

class DeviceAPI;
class ThreadedBasebandSampleSink;
class DownChannelizer;

class ChannelAnalyzer : public BasebandSampleSink, public ChannelAPI {
public:
    class MsgConfigureChannelAnalyzer : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        const ChannelAnalyzerSettings& getSettings() const { return m_settings; }
        bool getForce() const { return m_force; }

        static MsgConfigureChannelAnalyzer* create(const ChannelAnalyzerSettings& settings, bool force)
        {
            return new MsgConfigureChannelAnalyzer(settings, force);
        }

    private:
        ChannelAnalyzerSettings m_settings;
        bool m_force;

        MsgConfigureChannelAnalyzer(const ChannelAnalyzerSettings& settings, bool force) :
            Message(),
            m_settings(settings),
            m_force(force)
        { }
    };

    class MsgConfigureChannelizer : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        int getSampleRate() const { return m_sampleRate; }
        int getCenterFrequency() const { return m_centerFrequency; }

        static MsgConfigureChannelizer* create(int sampleRate, int centerFrequency)
        {
            return new MsgConfigureChannelizer(sampleRate, centerFrequency);
        }

    private:
        int  m_sampleRate;
        int  m_centerFrequency;

        MsgConfigureChannelizer(int sampleRate, int centerFrequency) :
            Message(),
            m_sampleRate(sampleRate),
            m_centerFrequency(centerFrequency)
        { }
    };

    class MsgReportChannelSampleRateChanged : public Message {
        MESSAGE_CLASS_DECLARATION

    public:

        static MsgReportChannelSampleRateChanged* create()
        {
            return new MsgReportChannelSampleRateChanged();
        }

    private:

        MsgReportChannelSampleRateChanged() :
            Message()
        { }
    };

    ChannelAnalyzer(DeviceAPI *deviceAPI);
	virtual ~ChannelAnalyzer();
	virtual void destroy() { delete this; }
	void setSampleSink(BasebandSampleSink* sampleSink) { m_sampleSink = sampleSink; }

//	void configure(MessageQueue* messageQueue,
//			int channelSampleRate,
//			Real Bandwidth,
//			Real LowCutoff,
//			int spanLog2,
//			bool ssb,
//			bool pll,
//			bool fll,
//			unsigned int pllPskOrder);

	DownChannelizer *getChannelizer() { return m_channelizer; }
	int getInputSampleRate() const { return m_inputSampleRate; }
    int getChannelSampleRate() const { return m_settings.m_downSample ? m_settings.m_downSampleRate : m_inputSampleRate; }
    int getDecimation() const { return 1<<m_settings.m_spanLog2; }
	double getMagSq() const { return m_magsq; }
	double getMagSqAvg() const { return (double) m_channelPowerAvg; }
	bool isPllLocked() const { return m_settings.m_pll && m_pll.locked(); }
    Real getPllFrequency() const;
	Real getPllDeltaPhase() const { return m_pll.getDeltaPhi(); }
    Real getPllPhase() const { return m_pll.getPhiHat(); }

	virtual void feed(const SampleVector::const_iterator& begin, const SampleVector::const_iterator& end, bool positiveOnly);
	virtual void start();
	virtual void stop();
	virtual bool handleMessage(const Message& cmd);

	virtual void getIdentifier(QString& id) { id = objectName(); }
    virtual void getTitle(QString& title) { title = objectName(); }
    virtual qint64 getCenterFrequency() const { return m_settings.m_frequency; }

    virtual QByteArray serialize() const { return QByteArray(); }
    virtual bool deserialize(const QByteArray& data) { (void) data; return false; }

    virtual int getNbSinkStreams() const { return 1; }
    virtual int getNbSourceStreams() const { return 0; }

    virtual qint64 getStreamCenterFrequency(int streamIndex, bool sinkElseSource) const
    {
        (void) streamIndex;
        (void) sinkElseSource;
        return m_settings.m_frequency;
    }

    static const QString m_channelIdURI;
    static const QString m_channelId;
    static const unsigned int m_corrFFTLen;

private:
	DeviceAPI *m_deviceAPI;
    ThreadedBasebandSampleSink* m_threadedChannelizer;
    DownChannelizer* m_channelizer;
    ChannelAnalyzerSettings m_settings;

    int m_inputSampleRate;
    int m_inputFrequencyOffset;
	int m_undersampleCount;
	fftfilt::cmplx m_sum;
	bool m_usb;
	double m_magsq;
	bool m_useInterpolator;

	NCOF m_nco;
	PhaseLockComplex m_pll;
	FreqLockComplex m_fll;
    Interpolator m_interpolator;
    Real m_interpolatorDistance;
    Real m_interpolatorDistanceRemain;

	fftfilt* SSBFilter;
	fftfilt* DSBFilter;
	fftfilt* RRCFilter;
	fftcorr* m_corr;

	BasebandSampleSink* m_sampleSink;
	SampleVector m_sampleBuffer;
	MovingAverageUtil<double, double, 480> m_channelPowerAvg;
	QMutex m_settingsMutex;

//	void apply(bool force = false);
	void applyChannelSettings(int inputSampleRate, int inputFrequencyOffset, bool force = false);
	void applySettings(const ChannelAnalyzerSettings& settings, bool force = false);
	void setFilters(int sampleRate, float bandwidth, float lowCutoff);
	void processOneSample(Complex& c, fftfilt::cmplx *sideband);

	inline void feedOneSample(const fftfilt::cmplx& s, const fftfilt::cmplx& pll)
	{
	    switch (m_settings.m_inputType)
	    {
	        case ChannelAnalyzerSettings::InputPLL:
            {
                if (m_settings.m_ssb & !m_usb) { // invert spectrum for LSB
                    m_sampleBuffer.push_back(Sample(pll.imag()*SDR_RX_SCALEF, pll.real()*SDR_RX_SCALEF));
                } else {
                    m_sampleBuffer.push_back(Sample(pll.real()*SDR_RX_SCALEF, pll.imag()*SDR_RX_SCALEF));
                }
            }
	            break;
	        case ChannelAnalyzerSettings::InputAutoCorr:
	        {
	            //std::complex<float> a = m_corr->run(s/(SDR_RX_SCALEF/768.0f), 0);
                std::complex<float> a = m_corr->run(s/SDR_RX_SCALEF, 0);

                if (m_settings.m_ssb & !m_usb) { // invert spectrum for LSB
                    m_sampleBuffer.push_back(Sample(a.imag(), a.real()));
                } else {
                    m_sampleBuffer.push_back(Sample(a.real(), a.imag()));
                }
	        }
	            break;
            case ChannelAnalyzerSettings::InputSignal:
            default:
            {
                if (m_settings.m_ssb & !m_usb) { // invert spectrum for LSB
                    m_sampleBuffer.push_back(Sample(s.imag(), s.real()));
                } else {
                    m_sampleBuffer.push_back(Sample(s.real(), s.imag()));
                }
            }
                break;
	    }
	}
};

#endif // INCLUDE_CHANALYZERNG_H
