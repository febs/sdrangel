#include <QtPlugin>
#include "plugin/pluginapi.h"

#include "loraplugin.h"
#include "lorademodgui.h"
#include "lorademod.h"

const PluginDescriptor LoRaPlugin::m_pluginDescriptor = {
	QString("LoRa Demodulator"),
	QString("3.14.5"),
	QString("(c) 2015 John Greb"),
	QString("http://www.maintech.de"),
	true,
	QString("github.com/hexameron/rtl-sdrangelove")
};

LoRaPlugin::LoRaPlugin(QObject* parent) :
	QObject(parent),
	m_pluginAPI(0)
{
}

const PluginDescriptor& LoRaPlugin::getPluginDescriptor() const
{
	return m_pluginDescriptor;
}

void LoRaPlugin::initPlugin(PluginAPI* pluginAPI)
{
	m_pluginAPI = pluginAPI;

	// register demodulator
	m_pluginAPI->registerRxChannel(LoRaDemod::m_channelIdURI, LoRaDemod::m_channelId, this);
}

PluginInstanceGUI* LoRaPlugin::createRxChannelGUI(DeviceUISet *deviceUISet, BasebandSampleSink *rxChannel) const
{
	return LoRaDemodGUI::create(m_pluginAPI, deviceUISet, rxChannel);
}

BasebandSampleSink* LoRaPlugin::createRxChannelBS(DeviceAPI *deviceAPI) const
{
    return new LoRaDemod(deviceAPI);
}

ChannelAPI* LoRaPlugin::createRxChannelCS(DeviceAPI *deviceAPI) const
{
    return new LoRaDemod(deviceAPI);
}

