///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Edouard Griffiths, F4EXB                                   //
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

#include <QLocale>

#include "device/deviceuiset.h"
#include "gui/basicchannelsettingsdialog.h"
#include "gui/devicestreamselectiondialog.h"
#include "dsp/hbfilterchainconverter.h"
#include "mainwindow.h"

#include "remotesinkgui.h"
#include "remotesink.h"
#include "ui_remotesinkgui.h"

RemoteSinkGUI* RemoteSinkGUI::create(PluginAPI* pluginAPI, DeviceUISet *deviceUISet, BasebandSampleSink *channelRx)
{
    RemoteSinkGUI* gui = new RemoteSinkGUI(pluginAPI, deviceUISet, channelRx);
    return gui;
}

void RemoteSinkGUI::destroy()
{
    delete this;
}

void RemoteSinkGUI::setName(const QString& name)
{
    setObjectName(name);
}

QString RemoteSinkGUI::getName() const
{
    return objectName();
}

qint64 RemoteSinkGUI::getCenterFrequency() const {
    return 0;
}

void RemoteSinkGUI::setCenterFrequency(qint64 centerFrequency)
{
    (void) centerFrequency;
}

void RemoteSinkGUI::resetToDefaults()
{
    m_settings.resetToDefaults();
    displaySettings();
    applySettings(true);
}

QByteArray RemoteSinkGUI::serialize() const
{
    return m_settings.serialize();
}

bool RemoteSinkGUI::deserialize(const QByteArray& data)
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

bool RemoteSinkGUI::handleMessage(const Message& message)
{
    if (RemoteSink::MsgSampleRateNotification::match(message))
    {
        RemoteSink::MsgSampleRateNotification& notif = (RemoteSink::MsgSampleRateNotification&) message;
        //m_channelMarker.setBandwidth(notif.getSampleRate());
        m_sampleRate = notif.getSampleRate();
        updateTxDelayTime();
        displayRateAndShift();
        return true;
    }
    else if (RemoteSink::MsgConfigureRemoteSink::match(message))
    {
        const RemoteSink::MsgConfigureRemoteSink& cfg = (RemoteSink::MsgConfigureRemoteSink&) message;
        m_settings = cfg.getSettings();
        blockApplySettings(true);
        displaySettings();
        blockApplySettings(false);
        return true;
    }
    else
    {
        return false;
    }
}

RemoteSinkGUI::RemoteSinkGUI(PluginAPI* pluginAPI, DeviceUISet *deviceUISet, BasebandSampleSink *channelrx, QWidget* parent) :
        RollupWidget(parent),
        ui(new Ui::RemoteSinkGUI),
        m_pluginAPI(pluginAPI),
        m_deviceUISet(deviceUISet),
        m_sampleRate(0),
        m_tickCount(0)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    connect(this, SIGNAL(widgetRolled(QWidget*,bool)), this, SLOT(onWidgetRolled(QWidget*,bool)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onMenuDialogCalled(const QPoint &)));

    m_remoteSink = (RemoteSink*) channelrx;
    m_remoteSink->setMessageQueueToGUI(getInputMessageQueue());

    m_channelMarker.blockSignals(true);
    m_channelMarker.setColor(m_settings.m_rgbColor);
    m_channelMarker.setCenterFrequency(0);
    m_channelMarker.setTitle("Remote source");
    m_channelMarker.blockSignals(false);
    m_channelMarker.setVisible(true); // activate signal on the last setting only

    m_settings.setChannelMarker(&m_channelMarker);

    m_deviceUISet->registerRxChannelInstance(RemoteSink::m_channelIdURI, this);
    m_deviceUISet->addChannelMarker(&m_channelMarker);
    m_deviceUISet->addRollupWidget(this);

    connect(getInputMessageQueue(), SIGNAL(messageEnqueued()), this, SLOT(handleSourceMessages()));
    //connect(&(m_deviceUISet->m_deviceSourceAPI->getMasterTimer()), SIGNAL(timeout()), this, SLOT(tick()));

    m_time.start();

    displaySettings();
    applySettings(true);
}

RemoteSinkGUI::~RemoteSinkGUI()
{
    m_deviceUISet->removeRxChannelInstance(this);
    delete m_remoteSink; // TODO: check this: when the GUI closes it has to delete the demodulator
    delete ui;
}

void RemoteSinkGUI::blockApplySettings(bool block)
{
    m_doApplySettings = !block;
}

void RemoteSinkGUI::applySettings(bool force)
{
    if (m_doApplySettings)
    {
        setTitleColor(m_channelMarker.getColor());

        RemoteSink::MsgConfigureRemoteSink* message = RemoteSink::MsgConfigureRemoteSink::create(m_settings, force);
        m_remoteSink->getInputMessageQueue()->push(message);
    }
}

void RemoteSinkGUI::applyChannelSettings()
{
    if (m_doApplySettings)
    {
        RemoteSink::MsgConfigureChannelizer *msgChan = RemoteSink::MsgConfigureChannelizer::create(
                m_settings.m_log2Decim,
                m_settings.m_filterChainHash);
        m_remoteSink->getInputMessageQueue()->push(msgChan);
    }
}

void RemoteSinkGUI::displaySettings()
{
    m_channelMarker.blockSignals(true);
    m_channelMarker.setCenterFrequency(0);
    m_channelMarker.setTitle(m_settings.m_title);
    m_channelMarker.setBandwidth(m_sampleRate); // TODO
    m_channelMarker.setMovable(false); // do not let user move the center arbitrarily
    m_channelMarker.blockSignals(false);
    m_channelMarker.setColor(m_settings.m_rgbColor); // activate signal on the last setting only

    setTitleColor(m_settings.m_rgbColor);
    setWindowTitle(m_channelMarker.getTitle());

    blockApplySettings(true);
    ui->decimationFactor->setCurrentIndex(m_settings.m_log2Decim);
    ui->dataAddress->setText(m_settings.m_dataAddress);
    ui->dataPort->setText(tr("%1").arg(m_settings.m_dataPort));
    QString s = QString::number(128 + m_settings.m_nbFECBlocks, 'f', 0);
    QString s1 = QString::number(m_settings.m_nbFECBlocks, 'f', 0);
    ui->nominalNbBlocksText->setText(tr("%1/%2").arg(s).arg(s1));
    ui->txDelayText->setText(tr("%1%").arg(m_settings.m_txDelay));
    ui->txDelay->setValue(m_settings.m_txDelay);
    updateTxDelayTime();
    applyDecimation();
    displayStreamIndex();
    blockApplySettings(false);
}

void RemoteSinkGUI::displayStreamIndex()
{
    if (m_deviceUISet->m_deviceMIMOEngine) {
        setStreamIndicator(tr("%1").arg(m_settings.m_streamIndex));
    } else {
        setStreamIndicator("S"); // single channel indicator
    }
}

void RemoteSinkGUI::displayRateAndShift()
{
    int shift = m_shiftFrequencyFactor * m_sampleRate;
    double channelSampleRate = ((double) m_sampleRate) / (1<<m_settings.m_log2Decim);
    QLocale loc;
    ui->offsetFrequencyText->setText(tr("%1 Hz").arg(loc.toString(shift)));
    ui->channelRateText->setText(tr("%1k").arg(QString::number(channelSampleRate / 1000.0, 'g', 5)));
    m_channelMarker.setCenterFrequency(shift);
    m_channelMarker.setBandwidth(channelSampleRate);
}

void RemoteSinkGUI::leaveEvent(QEvent*)
{
    m_channelMarker.setHighlighted(false);
}

void RemoteSinkGUI::enterEvent(QEvent*)
{
    m_channelMarker.setHighlighted(true);
}

void RemoteSinkGUI::handleSourceMessages()
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

void RemoteSinkGUI::onWidgetRolled(QWidget* widget, bool rollDown)
{
    (void) widget;
    (void) rollDown;
}

void RemoteSinkGUI::onMenuDialogCalled(const QPoint &p)
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
        dialog.setNumberOfStreams(m_remoteSink->getNumberOfDeviceStreams());
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

void RemoteSinkGUI::on_decimationFactor_currentIndexChanged(int index)
{
    m_settings.m_log2Decim = index;
    applyDecimation();
}

void RemoteSinkGUI::on_position_valueChanged(int value)
{
    m_settings.m_filterChainHash = value;
    applyPosition();
}

void RemoteSinkGUI::on_dataAddress_returnPressed()
{
    m_settings.m_dataAddress = ui->dataAddress->text();
    applySettings();
}

void RemoteSinkGUI::on_dataPort_returnPressed()
{
    bool dataOk;
    int dataPort = ui->dataPort->text().toInt(&dataOk);

    if((!dataOk) || (dataPort < 1024) || (dataPort > 65535))
    {
        return;
    }
    else
    {
        m_settings.m_dataPort = dataPort;
    }

    applySettings();
}

void RemoteSinkGUI::on_dataApplyButton_clicked(bool checked)
{
    (void) checked;
    m_settings.m_dataAddress = ui->dataAddress->text();

    bool dataOk;
    int udpDataPort = ui->dataPort->text().toInt(&dataOk);

    if((dataOk) && (udpDataPort >= 1024) && (udpDataPort < 65535))
    {
        m_settings.m_dataPort = udpDataPort;
    }

    applySettings();
}

void RemoteSinkGUI::on_txDelay_valueChanged(int value)
{
    m_settings.m_txDelay = value; // percentage
    ui->txDelayText->setText(tr("%1%").arg(value));
    updateTxDelayTime();
    applySettings();
}

void RemoteSinkGUI::on_nbFECBlocks_valueChanged(int value)
{
    m_settings.m_nbFECBlocks = value;
    int nbOriginalBlocks = 128;
    int nbFECBlocks = value;
    QString s = QString::number(nbOriginalBlocks + nbFECBlocks, 'f', 0);
    QString s1 = QString::number(nbFECBlocks, 'f', 0);
    ui->nominalNbBlocksText->setText(tr("%1/%2").arg(s).arg(s1));
    updateTxDelayTime();
    applySettings();
}

void RemoteSinkGUI::updateTxDelayTime()
{
    double txDelayRatio = m_settings.m_txDelay / 100.0;
    int samplesPerBlock = RemoteNbBytesPerBlock / sizeof(Sample);
    double delay = m_sampleRate == 0 ? 0.0 : (127*samplesPerBlock*txDelayRatio) / m_sampleRate;
    delay /= 128 + m_settings.m_nbFECBlocks;
    ui->txDelayTime->setText(tr("%1µs").arg(QString::number(delay*1e6, 'f', 0)));
}

void RemoteSinkGUI::applyDecimation()
{
    uint32_t maxHash = 1;

    for (uint32_t i = 0; i < m_settings.m_log2Decim; i++) {
        maxHash *= 3;
    }

    ui->position->setMaximum(maxHash-1);
    ui->position->setValue(m_settings.m_filterChainHash);
    m_settings.m_filterChainHash = ui->position->value();
    applyPosition();
}

void RemoteSinkGUI::applyPosition()
{
    ui->filterChainIndex->setText(tr("%1").arg(m_settings.m_filterChainHash));
    QString s;
    m_shiftFrequencyFactor = HBFilterChainConverter::convertToString(m_settings.m_log2Decim, m_settings.m_filterChainHash, s);
    ui->filterChainText->setText(s);

    displayRateAndShift();
    applyChannelSettings();
}

void RemoteSinkGUI::tick()
{
    if (++m_tickCount == 20) { // once per second
        m_tickCount = 0;
    }
}
