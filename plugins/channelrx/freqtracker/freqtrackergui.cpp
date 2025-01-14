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

#include <QDockWidget>
#include <QMainWindow>
#include <QDebug>

#include "freqtrackergui.h"

#include "device/deviceuiset.h"
#include "dsp/downchannelizer.h"
#include "dsp/dspengine.h"
#include "dsp/threadedbasebandsamplesink.h"
#include "ui_freqtrackergui.h"
#include "plugin/pluginapi.h"
#include "util/simpleserializer.h"
#include "util/db.h"
#include "gui/basicchannelsettingsdialog.h"
#include "gui/devicestreamselectiondialog.h"
#include "dsp/dspengine.h"
#include "mainwindow.h"
#include "gui/crightclickenabler.h"
#include "gui/audioselectdialog.h"

#include "freqtracker.h"

FreqTrackerGUI* FreqTrackerGUI::create(PluginAPI* pluginAPI, DeviceUISet *deviceUISet, BasebandSampleSink *rxChannel)
{
	FreqTrackerGUI* gui = new FreqTrackerGUI(pluginAPI, deviceUISet, rxChannel);
	return gui;
}

void FreqTrackerGUI::destroy()
{
	delete this;
}

void FreqTrackerGUI::setName(const QString& name)
{
	setObjectName(name);
}

QString FreqTrackerGUI::getName() const
{
	return objectName();
}

qint64 FreqTrackerGUI::getCenterFrequency() const {
	return m_channelMarker.getCenterFrequency();
}

void FreqTrackerGUI::setCenterFrequency(qint64 centerFrequency)
{
	m_channelMarker.setCenterFrequency(centerFrequency);
	applySettings();
}

void FreqTrackerGUI::resetToDefaults()
{
    m_settings.resetToDefaults();
    displaySettings();
	applySettings(true);
}

QByteArray FreqTrackerGUI::serialize() const
{
    return m_settings.serialize();
}

bool FreqTrackerGUI::deserialize(const QByteArray& data)
{
    if(m_settings.deserialize(data)) {
        displaySettings();
        applySettings(true);
        return true;
    } else {
        resetToDefaults();
        return false;
    }
}

bool FreqTrackerGUI::handleMessage(const Message& message)
{
    if (FreqTracker::MsgConfigureFreqTracker::match(message))
    {
        qDebug("FreqTrackerGUI::handleMessage: FreqTracker::MsgConfigureFreqTracker");
        const FreqTracker::MsgConfigureFreqTracker& cfg = (FreqTracker::MsgConfigureFreqTracker&) message;
        m_settings = cfg.getSettings();
        blockApplySettings(true);
        displaySettings();
        blockApplySettings(false);
        return true;
    }
    else if (FreqTracker::MsgSampleRateNotification::match(message))
    {
        if (!m_settings.m_tracking) {
            qDebug("FreqTrackerGUI::handleMessage: FreqTracker::MsgSampleRateNotification");
        }
        const FreqTracker::MsgSampleRateNotification& cfg = (FreqTracker::MsgSampleRateNotification&) message;
        m_channelSampleRate = cfg.getSampleRate();
        ui->channelSampleRateText->setText(tr("%1k").arg(QString::number(m_channelSampleRate / 1000.0f, 'g', 5)));
        blockApplySettings(true);
        m_settings.m_inputFrequencyOffset = cfg.getFrequencyOffset();
        ui->deltaFrequency->setValue(m_settings.m_inputFrequencyOffset);
        m_channelMarker.setCenterFrequency(cfg.getFrequencyOffset());
        blockApplySettings(false);

        if (m_channelSampleRate > 1000) {
            ui->rfBW->setMaximum(m_channelSampleRate/100);
        }

        return true;
    }

	return false;
}

void FreqTrackerGUI::handleInputMessages()
{
    Message* message;

    while ((message = getInputMessageQueue()->pop()) != 0)
    {
        if (handleMessage(*message))
        {
            delete message;
        }
    }
}

void FreqTrackerGUI::channelMarkerChangedByCursor()
{
    ui->deltaFrequency->setValue(m_channelMarker.getCenterFrequency());
    m_settings.m_inputFrequencyOffset = m_channelMarker.getCenterFrequency();
	applySettings();
}

void FreqTrackerGUI::channelMarkerHighlightedByCursor()
{
    setHighlighted(m_channelMarker.getHighlighted());
}

void FreqTrackerGUI::on_deltaFrequency_changed(qint64 value)
{
    m_channelMarker.setCenterFrequency(value);
    m_settings.m_inputFrequencyOffset = m_channelMarker.getCenterFrequency();
    applySettings();
}

void FreqTrackerGUI::on_log2Decim_currentIndexChanged(int index)
{
    m_settings.m_log2Decim = index < 0 ? 0 : index > 6 ? 6 : index;
    applySettings();
}

void FreqTrackerGUI::on_rfBW_valueChanged(int value)
{
	ui->rfBWText->setText(QString("%1 kHz").arg(value / 10.0, 0, 'f', 1));
	m_channelMarker.setBandwidth(value * 100);
	m_settings.m_rfBandwidth = value * 100;
	applySettings();
}

void FreqTrackerGUI::on_tracking_toggled(bool checked)
{
    if (!checked)
    {
        ui->tracking->setStyleSheet("QToolButton { background:rgb(79,79,79); }");
        ui->tracking->setToolTip(tr("PLL for synchronous AM"));
    }

    m_settings.m_tracking = checked;
    applySettings();
}

void FreqTrackerGUI::on_alphaEMA_valueChanged(int value)
{
    m_settings.m_alphaEMA = value / 100.0;
    QString alphaEMAStr = QString::number(m_settings.m_alphaEMA, 'f', 2);
    ui->alphaEMAText->setText(alphaEMAStr);
    applySettings();
}

void FreqTrackerGUI::on_trackerType_currentIndexChanged(int index)
{
    m_settings.m_trackerType = (FreqTrackerSettings::TrackerType) index;
    applySettings();
}

void FreqTrackerGUI::on_pllPskOrder_currentIndexChanged(int index)
{
    if ((index < 0) || (index > 5)) {
        return;
    }

    m_settings.m_pllPskOrder = 1<<index;
    applySettings();
}

void FreqTrackerGUI::on_rrc_toggled(bool checked)
{
    m_settings.m_rrc = checked;
    applySettings();
}

void FreqTrackerGUI::on_rrcRolloff_valueChanged(int value)
{
    m_settings.m_rrcRolloff = value < 0 ? 0 : value > 100 ? 100 : value;
    QString rolloffStr = QString::number(value/100.0, 'f', 2);
    ui->rrcRolloffText->setText(rolloffStr);
    applySettings();
}

void FreqTrackerGUI::on_squelch_valueChanged(int value)
{
	ui->squelchText->setText(QString("%1 dB").arg(value));
	m_settings.m_squelch = value;
	applySettings();
}

void FreqTrackerGUI::on_squelchGate_valueChanged(int value)
{
    ui->squelchGateText->setText(QString("%1").arg(value * 10.0f, 0, 'f', 0));
    m_settings.m_squelchGate = value;
	applySettings();
}

void FreqTrackerGUI::onWidgetRolled(QWidget* widget, bool rollDown)
{
    (void) widget;
    (void) rollDown;
}

void FreqTrackerGUI::onMenuDialogCalled(const QPoint &p)
{
    if (m_contextMenuType == ContextMenuChannelSettings)
    {
        BasicChannelSettingsDialog dialog(&m_channelMarker, this);
        dialog.setUseReverseAPI(m_settings.m_useReverseAPI);
        dialog.setReverseAPIAddress(m_settings.m_reverseAPIAddress);
        dialog.setReverseAPIPort(m_settings.m_reverseAPIPort);
        dialog.setReverseAPIDeviceIndex(m_settings.m_reverseAPIDeviceIndex);
        dialog.setReverseAPIChannelIndex(m_settings.m_reverseAPIChannelIndex);
        dialog.move(p);
        dialog.exec();

        m_settings.m_inputFrequencyOffset = m_channelMarker.getCenterFrequency();
        m_settings.m_rgbColor = m_channelMarker.getColor().rgb();
        m_settings.m_title = m_channelMarker.getTitle();
        m_settings.m_useReverseAPI = dialog.useReverseAPI();
        m_settings.m_reverseAPIAddress = dialog.getReverseAPIAddress();
        m_settings.m_reverseAPIPort = dialog.getReverseAPIPort();
        m_settings.m_reverseAPIDeviceIndex = dialog.getReverseAPIDeviceIndex();
        m_settings.m_reverseAPIChannelIndex = dialog.getReverseAPIChannelIndex();

        setWindowTitle(m_settings.m_title);
        setTitleColor(m_settings.m_rgbColor);

        applySettings();
    }
    else if ((m_contextMenuType == ContextMenuStreamSettings) && (m_deviceUISet->m_deviceMIMOEngine))
    {
        DeviceStreamSelectionDialog dialog(this);
        dialog.setNumberOfStreams(m_freqTracker->getNumberOfDeviceStreams());
        dialog.setStreamIndex(m_settings.m_streamIndex);
        dialog.move(p);
        dialog.exec();

        m_settings.m_streamIndex = dialog.getSelectedStreamIndex();
        m_channelMarker.clearStreamIndexes();
        m_channelMarker.addStreamIndex(m_settings.m_streamIndex);
        displayStreamIndex();
        applySettings();
    }

    resetContextMenuType();
}

FreqTrackerGUI::FreqTrackerGUI(PluginAPI* pluginAPI, DeviceUISet *deviceUISet, BasebandSampleSink *rxChannel, QWidget* parent) :
	RollupWidget(parent),
	ui(new Ui::FreqTrackerGUI),
	m_pluginAPI(pluginAPI),
	m_deviceUISet(deviceUISet),
	m_channelMarker(this),
    m_channelSampleRate(0),
	m_doApplySettings(true),
	m_squelchOpen(false),
	m_tickCount(0)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);
	connect(this, SIGNAL(widgetRolled(QWidget*,bool)), this, SLOT(onWidgetRolled(QWidget*,bool)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onMenuDialogCalled(const QPoint &)));

	m_freqTracker = reinterpret_cast<FreqTracker*>(rxChannel); //new FreqTracker(m_deviceUISet->m_deviceSourceAPI);
	m_freqTracker->setMessageQueueToGUI(getInputMessageQueue());

	connect(&MainWindow::getInstance()->getMasterTimer(), SIGNAL(timeout()), this, SLOT(tick())); // 50 ms

    ui->deltaFrequencyLabel->setText(QString("%1f").arg(QChar(0x94, 0x03)));
    ui->deltaFrequency->setColorMapper(ColorMapper(ColorMapper::GrayGold));
    ui->deltaFrequency->setValueRange(false, 7, -9999999, 9999999);
	ui->channelPowerMeter->setColorTheme(LevelMeterSignalDB::ColorGreenAndBlue);

	m_channelMarker.blockSignals(true);
	m_channelMarker.setColor(Qt::yellow);
	m_channelMarker.setBandwidth(5000);
	m_channelMarker.setCenterFrequency(0);
    m_channelMarker.setTitle("AM Demodulator");
    m_channelMarker.blockSignals(false);
    m_channelMarker.setVisible(true); // activate signal on the last setting only

    setTitleColor(m_channelMarker.getColor());
    m_settings.setChannelMarker(&m_channelMarker);

    m_deviceUISet->registerRxChannelInstance(FreqTracker::m_channelIdURI, this);
	m_deviceUISet->addChannelMarker(&m_channelMarker);
	m_deviceUISet->addRollupWidget(this);

	connect(&m_channelMarker, SIGNAL(changedByCursor()), this, SLOT(channelMarkerChangedByCursor()));
    connect(&m_channelMarker, SIGNAL(highlightedByCursor()), this, SLOT(channelMarkerHighlightedByCursor()));
    connect(getInputMessageQueue(), SIGNAL(messageEnqueued()), this, SLOT(handleInputMessages()));

	displaySettings();
	applySettings(true);
}

FreqTrackerGUI::~FreqTrackerGUI()
{
    m_deviceUISet->removeRxChannelInstance(this);
	delete m_freqTracker; // TODO: check this: when the GUI closes it has to delete the demodulator
	delete ui;
}

void FreqTrackerGUI::blockApplySettings(bool block)
{
    m_doApplySettings = !block;
}

void FreqTrackerGUI::applySettings(bool force)
{
	if (m_doApplySettings)
	{
	    FreqTracker::MsgConfigureFreqTracker* message = FreqTracker::MsgConfigureFreqTracker::create( m_settings, force);
	    m_freqTracker->getInputMessageQueue()->push(message);
	}
}

void FreqTrackerGUI::displaySettings()
{
    m_channelMarker.blockSignals(true);
    m_channelMarker.setCenterFrequency(m_settings.m_inputFrequencyOffset);
    m_channelMarker.setBandwidth(m_settings.m_rfBandwidth);
    m_channelMarker.setTitle(m_settings.m_title);
    m_channelMarker.blockSignals(false);
    m_channelMarker.setColor(m_settings.m_rgbColor); // activate signal on the last setting only

    setTitleColor(m_settings.m_rgbColor);
    setWindowTitle(m_channelMarker.getTitle());

    blockApplySettings(true);

    ui->deltaFrequency->setValue(m_channelMarker.getCenterFrequency());
    ui->log2Decim->setCurrentIndex(m_settings.m_log2Decim);
    int displayValue = m_settings.m_rfBandwidth/100.0;
    ui->rfBW->setValue(displayValue);
    ui->rfBWText->setText(QString("%1 kHz").arg(displayValue / 10.0, 0, 'f', 1));
    ui->squelch->setValue(m_settings.m_squelch);
    ui->squelchText->setText(QString("%1 dB").arg(m_settings.m_squelch));
    ui->tracking->setChecked(m_settings.m_tracking);
    ui->trackerType->setCurrentIndex((int) m_settings.m_trackerType);
    QString alphaEMAStr = QString::number(m_settings.m_alphaEMA, 'f', 2);
    ui->alphaEMAText->setText(alphaEMAStr);
    ui->alphaEMA->setValue(m_settings.m_alphaEMA*100.0);

    int i = 0;
    for(; ((m_settings.m_pllPskOrder>>i) & 1) == 0; i++);
    ui->pllPskOrder->setCurrentIndex(i);

    ui->rrc->setChecked(m_settings.m_rrc);
    ui->rrcRolloff->setValue(m_settings.m_rrcRolloff);
    QString rolloffStr = QString::number(m_settings.m_rrcRolloff/100.0, 'f', 2);
    ui->rrcRolloffText->setText(rolloffStr);
    ui->squelchGateText->setText(QString("%1").arg(m_settings.m_squelchGate * 10.0f, 0, 'f', 0));
    ui->squelchGate->setValue(m_settings.m_squelchGate);

    displayStreamIndex();

    blockApplySettings(false);
}

void FreqTrackerGUI::displayStreamIndex()
{
    if (m_deviceUISet->m_deviceMIMOEngine) {
        setStreamIndicator(tr("%1").arg(m_settings.m_streamIndex));
    } else {
        setStreamIndicator("S"); // single channel indicator
    }
}

void FreqTrackerGUI::leaveEvent(QEvent*)
{
	m_channelMarker.setHighlighted(false);
}

void FreqTrackerGUI::enterEvent(QEvent*)
{
	m_channelMarker.setHighlighted(true);
}

void FreqTrackerGUI::tick()
{
    double magsqAvg, magsqPeak;
    int nbMagsqSamples;
    m_freqTracker->getMagSqLevels(magsqAvg, magsqPeak, nbMagsqSamples);
    double powDbAvg = CalcDb::dbPower(magsqAvg);
    double powDbPeak = CalcDb::dbPower(magsqPeak);

    ui->channelPowerMeter->levelChanged(
            (100.0f + powDbAvg) / 100.0f,
            (100.0f + powDbPeak) / 100.0f,
            nbMagsqSamples);

    if (m_tickCount % 4 == 0) {
        ui->channelPower->setText(QString::number(powDbAvg, 'f', 1));
    }

	bool squelchOpen = m_freqTracker->getSquelchOpen();

    if (squelchOpen) {
        ui->squelchLabel->setStyleSheet("QLabel { background-color : green; }");
    } else {
        ui->squelchLabel->setStyleSheet("QLabel { background:rgb(50,50,50); }");
    }

    if (m_freqTracker->getPllLocked()) {
        ui->tracking->setStyleSheet("QToolButton { background-color : green; }");
    } else {
        ui->tracking->setStyleSheet("QToolButton { background:rgb(79,79,79); }");
    }

    int freq = m_freqTracker->getAvgDeltaFreq();
    QLocale loc;
    ui->trackingFrequencyText->setText(tr("%1 Hz").arg(loc.toString(freq)));

	if (m_settings.m_tracking) {
        ui->tracking->setToolTip("Tracking on");
	} else {
        ui->tracking->setToolTip("Tracking off");
    }

	m_tickCount++;
}

