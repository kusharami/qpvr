#include "QPVRPlugin.h"

#include "QPVRHandler.h"

#include <QIODevice>
#include <QFileDevice>

QImageIOPlugin::Capabilities QPVRPlugin::capabilities(
	QIODevice *device, const QByteArray &format) const
{
	if (format == QPVRHandler::PVR_Format() ||
		format == QPVRHandler::PVR_CCZ_Format())
	{
		return Capabilities(CanRead | CanWrite);
	}

	auto file = dynamic_cast<QFileDevice *>(device);

	if (nullptr != file &&
		file->fileName().endsWith(
			QLatin1String(".pvr.ccz"), Qt::CaseInsensitive) &&
		format == "ccz")
	{
		// detected pvr.ccz
	} else if (!format.isEmpty())
	{
		return 0;
	}

	if (nullptr == device || !device->isOpen())
		return 0;

	Capabilities result;

	if (device->isReadable() &&
		QPVRHandler::detectFileFormat(device) != QPVRHandler::UnknownFormat)
	{
		result |= CanRead;
	}

	if (device->isWritable())
	{
		result |= CanWrite;
	}

	return result;
}

QImageIOHandler *QPVRPlugin::create(
	QIODevice *device, const QByteArray &format) const
{
	auto handler = new QPVRHandler;
	handler->setDevice(device);

	handler->setFormat(
		format == "ccz" ? QPVRHandler::PVR_CCZ_Format() : format);
	return handler;
}
