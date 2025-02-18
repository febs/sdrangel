///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 Edouard Griffiths, F4EXB                                   //
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

#include <QtPlugin>
#include <libbladeRF.h>
#include "plugin/pluginapi.h"
#include "util/simpleserializer.h"

#include "bladerf1outputplugin.h"
#include "bladerf1outputwebapiadapter.h"

#ifdef SERVER_MODE
#include "bladerf1output.h"
#else
#include "bladerf1outputgui.h"
#endif

const PluginDescriptor Bladerf1OutputPlugin::m_pluginDescriptor = {
	QString("BladeRF1 Output"),
	QString("4.12.0"),
	QString("(c) Edouard Griffiths, F4EXB"),
	QString("https://github.com/f4exb/sdrangel"),
	true,
	QString("https://github.com/f4exb/sdrangel")
};

const QString Bladerf1OutputPlugin::m_hardwareID = "BladeRF1";
const QString Bladerf1OutputPlugin::m_deviceTypeID = BLADERF1OUTPUT_DEVICE_TYPE_ID;

Bladerf1OutputPlugin::Bladerf1OutputPlugin(QObject* parent) :
	QObject(parent)
{
}

const PluginDescriptor& Bladerf1OutputPlugin::getPluginDescriptor() const
{
	return m_pluginDescriptor;
}

void Bladerf1OutputPlugin::initPlugin(PluginAPI* pluginAPI)
{
	pluginAPI->registerSampleSink(m_deviceTypeID, this);
}

void Bladerf1OutputPlugin::enumOriginDevices(QStringList& listedHwIds, OriginDevices& originDevices)
{
    if (listedHwIds.contains(m_hardwareID)) { // check if it was done
        return;
    }

    DeviceBladeRF1::enumOriginDevices(m_hardwareID, originDevices);
    listedHwIds.append(m_hardwareID);
}

PluginInterface::SamplingDevices Bladerf1OutputPlugin::enumSampleSinks(const OriginDevices& originDevices)
{
	SamplingDevices result;

    for (OriginDevices::const_iterator it = originDevices.begin(); it != originDevices.end(); ++it)
    {
        if (it->hardwareId == m_hardwareID)
        {
            result.append(SamplingDevice(
                it->displayableName,
                it->hardwareId,
                m_deviceTypeID,
                it->serial,
                it->sequence,
                PluginInterface::SamplingDevice::PhysicalDevice,
                PluginInterface::SamplingDevice::StreamSingleTx,
                1,    // Nb of Tx streams
                0     // Stream index
            ));
        }
    }

	return result;
}

#ifdef SERVER_MODE
PluginInstanceGUI* Bladerf1OutputPlugin::createSampleSinkPluginInstanceGUI(
        const QString& sinkId,
        QWidget **widget,
        DeviceUISet *deviceUISet)
{
    (void) sinkId;
    (void) widget;
    (void) deviceUISet;
    return 0;
}
#else
PluginInstanceGUI* Bladerf1OutputPlugin::createSampleSinkPluginInstanceGUI(
        const QString& sinkId,
        QWidget **widget,
        DeviceUISet *deviceUISet)
{
	if(sinkId == m_deviceTypeID)
	{
		Bladerf1OutputGui* gui = new Bladerf1OutputGui(deviceUISet);
		*widget = gui;
		return gui;
	}
	else
	{
		return 0;
	}
}
#endif

DeviceSampleSink* Bladerf1OutputPlugin::createSampleSinkPluginInstance(const QString& sinkId, DeviceAPI *deviceAPI)
{
    if(sinkId == m_deviceTypeID)
    {
        Bladerf1Output* output = new Bladerf1Output(deviceAPI);
        return output;
    }
    else
    {
        return 0;
    }
}

DeviceWebAPIAdapter *Bladerf1OutputPlugin::createDeviceWebAPIAdapter() const
{
    return new BladeRF1OutputWebAPIAdapter();
}
