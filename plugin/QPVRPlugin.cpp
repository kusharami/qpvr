#include "QPVRPlugin.h"

#include "QPVRHandler.h"

#include <QIODevice>
#include <QFile>

QImageIOPlugin::Capabilities QPVRPlugin::capabilities(
	QIODevice *device, const QByteArray &format) const
{
	if (0 == strcmp(format.data(), "pvr") ||
		0 == strcmp(format.data(), "pvr.ccz"))
	{
		return Capabilities(CanRead | CanWrite);
	}

	auto file = dynamic_cast<QFile *>(device);

	if (nullptr != file && file->fileName().endsWith(
			QLatin1String(".pvr.ccz"), Qt::CaseInsensitive) &&
		0 == strcmp(format.data(), "ccz"))
	{
		// detected pvr.ccz
	} else
	if (!format.isEmpty())
		return 0;

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
		(0 == strcmp(format.data(), "ccz"))
		? QByteArrayLiteral("pvr.ccz")
		: format);
	return handler;
}
