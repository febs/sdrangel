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

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <algorithm>
#include <QDebug>

#include "dsp/samplesourcefifo.h"
#include "util/timeutil.h"
#include "remoteoutputthread.h"

RemoteOutputThread::RemoteOutputThread(SampleSourceFifo* sampleFifo, QObject* parent) :
	QThread(parent),
	m_running(false),
	m_samplesChunkSize(0),
	m_sampleFifo(sampleFifo),
	m_samplesCount(0),
	m_chunkCorrection(0),
    m_samplerate(0),
    m_throttlems(REMOTEOUTPUT_THROTTLE_MS),
    m_maxThrottlems(50),
    m_throttleToggle(false)
{
}

RemoteOutputThread::~RemoteOutputThread()
{
	if (m_running) {
		stopWork();
	}
}

void RemoteOutputThread::startWork()
{
	qDebug() << "RemoteOutputThread::startWork: ";
	m_udpSinkFEC.start();
    m_maxThrottlems = 0;
    m_startWaitMutex.lock();
    m_elapsedTimer.start();
    start();
    while(!m_running)
        m_startWaiter.wait(&m_startWaitMutex, 100);
    m_startWaitMutex.unlock();
}

void RemoteOutputThread::stopWork()
{
	qDebug() << "RemoteOutputThread::stopWork";
	m_running = false;
	wait();
	m_udpSinkFEC.stop();
}

void RemoteOutputThread::setSamplerate(int samplerate)
{
	if (samplerate != m_samplerate)
	{
	    qDebug() << "RemoteOutputThread::setSamplerate:"
	            << " new:" << samplerate
	            << " old:" << m_samplerate;

	    bool wasRunning = false;

		if (m_running)
		{
			stopWork();
			wasRunning = true;
		}

		// resize sample FIFO
		if (m_sampleFifo)
        {
            unsigned int fifoRate = std::max(
                (unsigned int) samplerate,
                48000U);
            m_sampleFifo->resize(SampleSourceFifo::getSizePolicy(fifoRate));
		}

        m_samplerate = samplerate;
        m_samplesChunkSize = (m_samplerate * m_throttlems) / 1000;
        m_udpSinkFEC.setSampleRate(m_samplerate);

        if (wasRunning) {
            startWork();
        }
	}
}

void RemoteOutputThread::run()
{
	m_running = true;
	m_startWaiter.wakeAll();

	while(m_running) // actual work is in the tick() function
	{
		sleep(1);
	}

	m_running = false;
}

void RemoteOutputThread::connectTimer(const QTimer& timer)
{
	qDebug() << "RemoteOutputThread::connectTimer";
	connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));
}

void RemoteOutputThread::tick()
{
	if (m_running)
	{
        qint64 throttlems = m_elapsedTimer.restart();

        if (throttlems != m_throttlems)
        {
            m_throttlems = throttlems;
            m_samplesChunkSize = (m_samplerate * (m_throttlems+(m_throttleToggle ? 1 : 0))) / 1000;
            m_samplesChunkSize = m_samplesChunkSize + m_chunkCorrection > 0 ? m_samplesChunkSize + m_chunkCorrection : m_samplesChunkSize;
            m_throttleToggle = !m_throttleToggle;
        }

        SampleVector::iterator readUntil;

        SampleVector& data = m_sampleFifo->getData();
        unsigned int iPart1Begin, iPart1End, iPart2Begin, iPart2End;
        m_sampleFifo->read(m_samplesChunkSize, iPart1Begin, iPart1End, iPart2Begin, iPart2End);

        if (iPart1Begin != iPart1End)
        {
            SampleVector::iterator beginRead = data.begin() + iPart1Begin;
            unsigned int partSize = iPart1End - iPart1Begin;
            m_udpSinkFEC.write(beginRead, partSize);
        }

        if (iPart2Begin != iPart2End)
        {
            SampleVector::iterator beginRead = data.begin() + iPart2Begin;
            unsigned int partSize = iPart2End - iPart2Begin;
            m_udpSinkFEC.write(beginRead, partSize);
        }
	}
}

uint32_t RemoteOutputThread::getSamplesCount(uint64_t& ts_usecs) const
{
    ts_usecs = TimeUtil::nowus();
    return m_samplesCount;
}
