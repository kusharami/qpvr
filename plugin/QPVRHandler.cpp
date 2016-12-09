#include "QPVRHandler.h"

#include <QVariant>

#include "PVRAssets/Texture/Texture.h"

QPVRHandler::QPVRHandler()
{

}

bool QPVRHandler::canRead(QIODevice *device)
{
	Q_ASSERT(nullptr != device);

	device->peek()
}

bool QPVRHandler::canRead() const
{

}

bool QPVRHandler::read(QImage *image)
{

}

bool QPVRHandler::write(const QImage &image)
{

}

QVariant QPVRHandler::option(QImageIOHandler::ImageOption option) const
{

}

void QPVRHandler::setOption(QImageIOHandler::ImageOption option, const QVariant &value)
{

}

bool QPVRHandler::supportsOption(QImageIOHandler::ImageOption option) const
{

}

bool QPVRHandler::jumpToImage(int imageNumber)
{

}

int QPVRHandler::imageCount() const
{

}
