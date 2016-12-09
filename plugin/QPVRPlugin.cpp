#include "QPVRPlugin.h"

#include "QPVRHandler.h"

#include <QIODevice>

QImageIOPlugin::Capabilities QPVRPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
	if (format == QByteArrayLiteral("pvr") || format == QByteArrayLiteral("pvr.ccz"))
		return Capabilities(CanRead | CanWrite);

	if (!format.isEmpty())
		return 0;

	if (nullptr == device || !device->isOpen())
		return 0;

	Capabilities result;

	if (device->isReadable() && QPVRHandler::canRead(device))
		result |= CanRead;

	if (device->isWritable())
		result |= CanWrite;

	return result;
}

QImageIOHandler *QPVRPlugin::create(QIODevice *device, const QByteArray &format) const
{
	auto handler = new QPVRHandler;
	handler->setDevice(device);
	handler->setFormat(format);
	return handler;
}
