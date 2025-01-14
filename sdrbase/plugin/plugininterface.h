#ifndef INCLUDE_PLUGININTERFACE_H
#define INCLUDE_PLUGININTERFACE_H

#include <QtPlugin>
#include <QString>

#include "export.h"

struct SDRBASE_API PluginDescriptor {
	// general plugin description
	const QString displayedName;
	const QString version;
	const QString copyright;
	const QString website;
	bool licenseIsGPL;
	const QString sourceCodeURL;
};

class PluginAPI;
class DeviceAPI;
class DeviceUISet;
class PluginInstanceGUI;
class QWidget;
class DeviceSampleSource;
class DeviceSampleSink;
class DeviceSampleMIMO;
class BasebandSampleSink;
class BasebandSampleSource;
class MIMOChannel;
class ChannelAPI;
class ChannelWebAPIAdapter;
class DeviceWebAPIAdapter;

class SDRBASE_API PluginInterface {
public:
	struct SamplingDevice
	{
        enum SamplingDeviceType
        {
            PhysicalDevice,
            BuiltInDevice
        };

        enum StreamType
        {
            StreamSingleRx, //!< Exposes a single input stream that can be one of the streams of a physical device
            StreamSingleTx, //!< Exposes a single output stream that can be one of the streams of a physical device
            StreamMIMO      //!< May expose any number of input and/or output streams
        };

		QString displayedName;    //!< The human readable name
		QString hardwareId;       //!< The internal id that identifies the type of hardware (i.e. HackRF, BladeRF, ...)
		QString id;               //!< The internal plugin ID corresponding to the device (i.e. for HackRF input, for HackRF output ...)
		QString serial;           //!< The device serial number defined by the vendor or a fake one (SDRplay)
		int sequence;             //!< The device sequence. >0 when more than one device of the same type is connected
		SamplingDeviceType type;  //!< The sampling device type for behavior information
		StreamType streamType;    //!< This is the type of stream supported
		int deviceNbItems;        //!< Number of items (or streams) in the device. >1 for composite devices.
		int deviceItemIndex;      //!< For composite devices this is the Rx or Tx stream index. -1 if not initialized
		int claimed;              //!< This is the device set index if claimed else -1

		SamplingDevice(const QString& _displayedName,
                const QString& _hardwareId,
				const QString& _id,
				const QString& _serial,
				int _sequence,
				SamplingDeviceType _type,
				StreamType _streamType,
				int _deviceNbItems,
				int _deviceItemIndex) :
			displayedName(_displayedName),
			hardwareId(_hardwareId),
			id(_id),
			serial(_serial),
			sequence(_sequence),
			type(_type),
			streamType(_streamType),
			deviceNbItems(_deviceNbItems),
			deviceItemIndex(_deviceItemIndex),
			claimed(-1)
		{ }
	};
	typedef QList<SamplingDevice> SamplingDevices;

    /** This is the device from which the sampling devices are derived. For physical devices this represents
     * a single physical unit (a LimeSDR, HackRF, BladeRF, RTL-SDR dongle, ...) that is enumerated once and
     * reported in the system so that the "sampling devices" used in the system can be registered
    */
    struct OriginDevice
    {
        QString displayableName; //!< A human readable name
        QString hardwareId;      //!< The internal id that identifies the type of hardware (i.e. HackRF, BladeRF, ...)
        QString serial;          //!< The device serial number defined by the vendor or a fake one (SDRplay)
        int sequence;            //!< The device sequence in order of enumeration
        int nbRxStreams;         //!< Number of receiver streams
        int nbTxStreams;         //!< Number of transmitter streams

        OriginDevice(
                const QString& _displayableName,
                const QString& _hardwareId,
				const QString& _serial,
				int _sequence,
                int _nbRxStreams,
                int _nbTxStreams
        ) :
            displayableName(_displayableName),
            hardwareId(_hardwareId),
            serial(_serial),
            sequence(_sequence),
            nbRxStreams(_nbRxStreams),
            nbTxStreams(_nbTxStreams)
        {}
    };
    typedef QList<OriginDevice> OriginDevices;

    virtual ~PluginInterface() { }

	virtual const PluginDescriptor& getPluginDescriptor() const = 0;
	virtual void initPlugin(PluginAPI* pluginAPI) = 0;

	// channel Rx plugins

    virtual PluginInstanceGUI* createRxChannelGUI(
            DeviceUISet *deviceUISet,
            BasebandSampleSink *rxChannel) const
    {
        (void) deviceUISet;
        (void) rxChannel;
        return nullptr;
    }

    virtual BasebandSampleSink* createRxChannelBS(DeviceAPI *deviceAPI) const
    {
        (void) deviceAPI;
        return nullptr;
    }

    virtual ChannelAPI* createRxChannelCS(DeviceAPI *deviceAPI) const
    {
        (void) deviceAPI;
        return nullptr;
    }

    // channel Tx plugins

	virtual PluginInstanceGUI* createTxChannelGUI(
            DeviceUISet *deviceUISet,
            BasebandSampleSource *txChannel) const
    {
        (void) deviceUISet;
        (void) txChannel;
        return nullptr;
    }

    virtual BasebandSampleSource* createTxChannelBS(DeviceAPI *deviceAPI) const
    {
        (void) deviceAPI;
        return nullptr;
    }

    virtual ChannelAPI* createTxChannelCS(DeviceAPI *deviceAPI) const
    {
        (void) deviceAPI;
        return nullptr;
    }

    // channel MIMO plugins

	virtual PluginInstanceGUI* createMIMOChannelGUI(
            DeviceUISet *deviceUISet,
            MIMOChannel *mimoChannel) const
    {
        (void) deviceUISet;
        (void) mimoChannel;
        return nullptr;
    }

    virtual MIMOChannel* createMIMOChannelBS(DeviceAPI *deviceAPI) const
    {
        (void) deviceAPI;
        return nullptr;
    }

    virtual ChannelAPI* createMIMOChannelCS(DeviceAPI *deviceAPI) const
    {
        (void) deviceAPI;
        return nullptr;
    }

    // any channel

    virtual ChannelWebAPIAdapter* createChannelWebAPIAdapter() const
    {
        return nullptr;
    }

    // any device

    virtual void enumOriginDevices(QStringList& listedHwIds, OriginDevices& originDevices)
    {
        (void) listedHwIds;
        (void) originDevices;
    }

    // device source plugins only

	virtual SamplingDevices enumSampleSources(const OriginDevices& originDevices)
    {
        (void) originDevices;
        return SamplingDevices();
    }

	virtual PluginInstanceGUI* createSampleSourcePluginInstanceGUI(
            const QString& sourceId,
            QWidget **widget,
            DeviceUISet *deviceUISet)
    {
        (void) sourceId;
        (void) widget;
        (void) deviceUISet;
        return nullptr;
    }

    virtual DeviceSampleSource* createSampleSourcePluginInstance( // creates the input "core"
            const QString& sourceId,
            DeviceAPI *deviceAPI)
    {
        (void) sourceId;
        (void) deviceAPI;
        return nullptr;
    }
	virtual void deleteSampleSourcePluginInstanceGUI(PluginInstanceGUI *ui);
	virtual void deleteSampleSourcePluginInstanceInput(DeviceSampleSource *source);

	// device sink plugins only

	virtual SamplingDevices enumSampleSinks(const OriginDevices& originDevices)
    {
        (void) originDevices;
        return SamplingDevices();
    }

	virtual PluginInstanceGUI* createSampleSinkPluginInstanceGUI(
            const QString& sinkId,
            QWidget **widget,
            DeviceUISet *deviceUISet)
    {
        (void) sinkId;
        (void) widget;
        (void) deviceUISet;
        return nullptr;
    }

    virtual DeviceSampleSink* createSampleSinkPluginInstance( // creates the output "core"
            const QString& sinkId,
            DeviceAPI *deviceAPI)
    {
        (void) sinkId;
        (void) deviceAPI;
        return nullptr;
    }

    virtual void deleteSampleSinkPluginInstanceGUI(PluginInstanceGUI *ui);
    virtual void deleteSampleSinkPluginInstanceOutput(DeviceSampleSink *sink);

    // device MIMO plugins only

	virtual SamplingDevices enumSampleMIMO(const OriginDevices& originDevices)
    {
        (void) originDevices;
        return SamplingDevices();
    }

	virtual PluginInstanceGUI* createSampleMIMOPluginInstanceGUI(
            const QString& mimoId,
            QWidget **widget,
            DeviceUISet *deviceUISet)
    {
        (void) mimoId;
        (void) widget;
        (void) deviceUISet;
        return nullptr;
    }

    virtual DeviceSampleMIMO* createSampleMIMOPluginInstance( // creates the MIMO "core"
            const QString& mimoId,
            DeviceAPI *deviceAPI)
    {
        (void) mimoId;
        (void) deviceAPI;
        return nullptr;
    }

    virtual void deleteSampleMIMOPluginInstanceGUI(PluginInstanceGUI *ui);
    virtual void deleteSampleMIMOPluginInstanceMIMO(DeviceSampleMIMO *mimo);

    // all devices

    virtual DeviceWebAPIAdapter* createDeviceWebAPIAdapter() const {
        return nullptr;
    }
};

Q_DECLARE_INTERFACE(PluginInterface, "SDRangel.PluginInterface/0.1")

#endif // INCLUDE_PLUGININTERFACE_H
