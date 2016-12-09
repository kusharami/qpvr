#include "QPVRHandler.h"

#include <QVariant>


QPVRHandler::QPVRHandler()
{

}

QByteArray QPVRHandler::name() const
{

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
