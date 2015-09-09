///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 Edouard Griffiths, F4EXB                                   //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDE_AIRSPYGUI_H
#define INCLUDE_AIRSPYGUI_H

#include <QTimer>

#include "airspyinput.h"
#include "plugin/plugingui.h"

#define AIRSPY_MAX_DEVICE (32)

class PluginAPI;

namespace Ui {
	class AirspyGui;
	class AirspySampleRates;
}

class AirspyGui : public QWidget, public PluginGUI {
	Q_OBJECT

public:
	explicit AirspyGui(PluginAPI* pluginAPI, QWidget* parent = NULL);
	virtual ~AirspyGui();
	void destroy();

	void setName(const QString& name);
	QString getName() const;

	void resetToDefaults();
	qint64 getCenterFrequency() const;
	QByteArray serialize() const;
	bool deserialize(const QByteArray& data);
	virtual bool handleMessage(const Message& message);

private:
	Ui::AirspyGui* ui;

	PluginAPI* m_pluginAPI;
	AirspyInput::Settings m_settings;
	QTimer m_updateTimer;
	std::vector<int> m_gains;
	SampleSource* m_sampleSource;

	void displaySettings();
	void sendSettings();
	unsigned int getXb200Index(bool xb_200, bladerf_xb200_path xb200Path, bladerf_xb200_filter xb200Filter);

private slots:
	void on_centerFrequency_changed(quint64 value);
	void on_samplerate_valueChanged(int value);
	void on_bandwidth_valueChanged(int value);
	void on_decim_valueChanged(int value);
	void on_lna_valueChanged(int value);
	void on_vga1_valueChanged(int value);
	void on_vga2_valueChanged(int value);
	void on_xb200_currentIndexChanged(int index);
	void on_fcPos_currentIndexChanged(int index);
	void updateHardware();
};

class AirspySampleRates {
public:
	static unsigned int getRate(unsigned int rate_index);
	static unsigned int getRateIndex(unsigned int rate);
private:
	static unsigned int m_rates[14];
	static unsigned int m_nb_rates;
};

class AirspyBandwidths {
public:
	static unsigned int getBandwidth(unsigned int bandwidth_index);
	static unsigned int getBandwidthIndex(unsigned int bandwidth);
private:
	static unsigned int m_halfbw[16];
	static unsigned int m_nb_halfbw;
};

#endif // INCLUDE_AIRSPYGUI_H
