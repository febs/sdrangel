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

#ifndef PLUGINS_CHANNELRX_LOCALSINK_LOCALSINKGUI_H_
#define PLUGINS_CHANNELRX_LOCALSINK_LOCALSINKGUI_H_

#include <stdint.h>

#include <QObject>
#include <QTime>

#include "plugin/plugininstancegui.h"
#include "dsp/channelmarker.h"
#include "gui/rollupwidget.h"
#include "util/messagequeue.h"

#include "localsinksettings.h"

class PluginAPI;
class DeviceUISet;
class LocalSink;
class BasebandSampleSink;

namespace Ui {
    class LocalSinkGUI;
}

class LocalSinkGUI : public RollupWidget, public PluginInstanceGUI {
    Q_OBJECT
public:
    static LocalSinkGUI* create(PluginAPI* pluginAPI, DeviceUISet *deviceUISet, BasebandSampleSink *rxChannel);
    virtual void destroy();

    void setName(const QString& name);
    QString getName() const;
    virtual qint64 getCenterFrequency() const;
    virtual void setCenterFrequency(qint64 centerFrequency);

    void resetToDefaults();
    QByteArray serialize() const;
    bool deserialize(const QByteArray& data);
    virtual MessageQueue *getInputMessageQueue() { return &m_inputMessageQueue; }
    virtual bool handleMessage(const Message& message);

private:
    Ui::LocalSinkGUI* ui;
    PluginAPI* m_pluginAPI;
    DeviceUISet* m_deviceUISet;
    ChannelMarker m_channelMarker;
    LocalSinkSettings m_settings;
    int m_sampleRate;
    double m_shiftFrequencyFactor; //!< Channel frequency shift factor
    bool m_doApplySettings;

    LocalSink* m_localSink;
    MessageQueue m_inputMessageQueue;

    QTime m_time;
    uint32_t m_tickCount;

    explicit LocalSinkGUI(PluginAPI* pluginAPI, DeviceUISet *deviceUISet, BasebandSampleSink *rxChannel, QWidget* parent = 0);
    virtual ~LocalSinkGUI();

    void blockApplySettings(bool block);
    void applySettings(bool force = false);
    void applyChannelSettings();
    void displaySettings();
    void displayStreamIndex();
    void displayRateAndShift();
    void updateLocalDevices();

    void leaveEvent(QEvent*);
    void enterEvent(QEvent*);

    void applyDecimation();
    void applyPosition();

private slots:
    void handleSourceMessages();
    void on_decimationFactor_currentIndexChanged(int index);
    void on_position_valueChanged(int value);
    void on_localDevice_currentIndexChanged(int index);
    void on_localDevicesRefresh_clicked(bool checked);
    void onWidgetRolled(QWidget* widget, bool rollDown);
    void onMenuDialogCalled(const QPoint& p);
    void tick();
};



#endif /* PLUGINS_CHANNELRX_LOCALSINK_LOCALSINKGUI_H_ */
