#include "QPVRHandler.h"

#include <QVariant>

#include "PVRAssets/Texture/Texture.h"
#include "PVRAssets/FileIO/FileDefinesPVR.h"
#include "QCCZStream.h"

#ifdef Q_OS_WIN
#include <WinSock2.h>
#endif

#if defined(_MSC_VER)
#define ALIGNED_STRUCT(Name, alignment) \
	__pragma(pack(1)) \
	struct __declspec (align(alignment)) \
	Name
#else
#define ALIGNED_STRUCT(Name, alignment) \
	_Pragma("pack(1)") \
	struct __attribute__((aligned(alignment))) \
	Name
#endif

#if defined(_MSC_VER)
#define ALIGNED_STRUCT_END() \
	__pragma(pack())
#else
#define ALIGNED_STRUCT_END() \
	_Pragma("pack()")
#endif

using namespace pvr::assets;
using namespace pvr::texture_legacy;

ALIGNED_STRUCT(PVR3TexHeader, 1)
{
	quint32 version;
	TextureHeader::Header header;
};
ALIGNED_STRUCT_END()

QPVRHandler::QPVRHandler()
	: mFormat(UnknownFormat)
{

}

QPVRHandler::Format QPVRHandler::detectFormat(QIODevice *device)
{
	Q_ASSERT(nullptr == device);
	Q_ASSERT(device->isOpen());

	Format result = UnknownFormat;
	auto oldPos = device->pos();
	bool ccz = false;
	{
		QCCZDecompressionStream decompressor(device);
		if (decompressor.open())
		{
			device = &decompressor;
			ccz = true;
		}

		{
			HeaderV1 headerV1;
			if (device->peek(
					(char *) headerV1,
					c_headerSizeV1) == c_headerSizeV1 &&
				headerV1.headerSize == c_headerSizeV1)
			{
				result = ccz ? PVR1_CCZ : PVR1;
			}
		}

		{
			HeaderV2 headerV2;
			if (device->peek(
					(char *) headerV2,
					c_headerSizeV2) == c_headerSizeV2 &&
				headerV2.headerSize == c_headerSizeV2 &&
				headerV2.pvrMagic == htonl(c_identifierV2))
			{
				result = ccz ? PVR2_CCZ : PVR2;
			}
		}

		{
			PVR3TexHeader headerV3;
			if (device->peek(
					(char *) headerV3,
					sizeof(headerV3)) == sizeof(headerV3) &&
				ntohl(headerV3.version) == TextureHeader::Header::PVRv3Reversed)
			{
				result = ccz ? PVR3_CCZ : PVR3;
			}
		}

	}

	if (!device->seek(oldPos))
		return UnknownFormat;

	return result;
}

bool QPVRHandler::canRead() const
{
	mFormat = detectFormat(device());
	switch (mFormat)
	{
		case PVR2:
		case PVR3:
			setFormat(QByteArrayLiteral("pvr"));
			return true;

		case PVR2_CCZ:
		case PVR3_CCZ:
			setFormat(QByteArrayLiteral("pvr.ccz"));
			return true;

		case CCZ:
		case UnknownFormat:
			break;
	}

	return false;
}

bool QPVRHandler::read(QImage *image)
{
	if (!canRead())
		return false;

	Q_ASSERT(nullptr != image);

	auto device = this->device();
	std::unique_ptr<QCCZDecompressionStream> decompress;
	if (0 != (mFormat & CCZ))
	{
		decompress.reset(new QCCZDecompressionStream(device));
		device = decompress.get();
	}

	return false;
}

bool QPVRHandler::write(const QImage &image)
{
	return false;
}

QVariant QPVRHandler::option(ImageOption option) const
{
	return QVariant();
}

void QPVRHandler::setOption(ImageOption option, const QVariant &value)
{

}

bool QPVRHandler::supportsOption(ImageOption option) const
{
	return false;
}

bool QPVRHandler::jumpToImage(int imageNumber)
{
	return false;
}

int QPVRHandler::imageCount() const
{
	return 0;
}
