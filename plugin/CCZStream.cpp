#include "CCZStream.h"

#include <QDebug>

#if defined(Q_OS_MAC)
#	include <zlib.h>
#elif defined(Q_OS_WIN)
#	include <QtZlib/zlib.h>
#else
#	error Unsupported platform
#endif

enum
{
	CCZ_SIGNATURE_SIZE = 4
};

static const char CCZ_SIGNATURE[] = "CCZ!";

// Format header
struct CCZHeader
{
	char sig[CCZ_SIGNATURE_SIZE];	// signature. Should be 'CCZ!' 4 bytes
	uint16_t compression_type;		// should 0 (See below for supported formats)
	uint16_t version;				// should be 2
	uint32_t reserved;				// Reserverd for users.
	uint32_t len;					// size of the uncompressed file
};

enum
{
	// supported formats
	CCZ_COMPRESSION_ZLIB = 0
};

enum
{
	// header info
	CCZ_VERSION = 2,
	CCZ_HEADER_SIZE = sizeof(CCZHeader)
};

CCZStream::CCZStream(QIODevice *source)
	: source(source)
{
}

CCZStream::CCZStream(QIODevice *source, QObject *parent)
	: QIODevice(parent)
	, source(source)
{

}

bool CCZStream::isSequential() const
{
	Q_ASSERT(nullptr != source);

	return source->isSequential();
}

bool CCZStream::open(OpenMode mode)
{
	Q_ASSERT(!isOpen());
	Q_ASSERT(source->isOpen());

	bool append = 0 != (mode & Append);
	bool truncate = !append || 0 != (mode & Truncate);

	Q_ASSERT(!append || !truncate);

	if (append || truncate)
		mode |= WriteOnly;

	if (append)
		mode |= ReadOnly;

	sourcePosition = source->pos();

	if (0 != (mode & ReadOnly))
	{
		CCZHeader header;

		if (CCZ_HEADER_SIZE != source->read(reinterpret_cast<char *>(&header, CCZ_HEADER_SIZE))
		||	0 != memcmp(header.sig, CCZ_SIGNATURE, CCZ_SIGNATURE_SIZE))
		{
			setErrorString(tr("Unknown format"));
			return false;
		}

		header.compression_type = htons(header.compression_type);

		if (CCZ_COMPRESSION_ZLIB != header.compression_type)
		{
			setErrorString(tr("Unsupported CCZ compression %1").arg(header.compression_type));
			return false;
		}

		header.version = htons(header.version);

		if (CCZ_VERSION != header.version)
		{
			setErrorString(tr("Unknown CCZ version %1").arg(header.version));
			return false;
		}

		uncompressedSize = htonl(header.len);
	}



	return QIODevice::open(mode);
}

void CCZStream::close()
{
	QIODevice::close();
}

qint64 CCZStream::size() const
{
	return uncompressedSize;
}

bool CCZStream::seek(qint64 pos)
{
	if (pos < this->pos())
	{
		return false;
	}
}

