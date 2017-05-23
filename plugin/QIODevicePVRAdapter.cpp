#include "QIODevicePVRAdapter.h"

#include <QIODevice>

using namespace pvr;

static inline qint64 sizeToInt64(size_t size)
{
	return static_cast<qint64>(
		qMin(
			size,
			static_cast<size_t>(
				(sizeof(size_t) < 8)
				? static_cast<qint64>(
					std::numeric_limits<size_t>::max())
				: std::numeric_limits<qint64>::max())));
}

template <typename ADAPTER, typename BUFTYPE, typename FUNCTION>
static inline bool ioOperation(
	ADAPTER *adapter,
	size_t elementSize, size_t elementCount,
	BUFTYPE *buffer, size_t &resultCount,
	FUNCTION function)
{
	if (!adapter->isReadable() || !adapter->isopen())
		return false;

	auto adaptee = adapter->adaptee();
	auto requestedSize = elementSize * elementCount;
	auto size = sizeToInt64(requestedSize);
	resultCount = static_cast<size_t>(
			(adaptee->*function)(buffer, size));
	return resultCount == requestedSize ||
		   (adaptee->isSequential() && adaptee->atEnd());
}

QIODevicePVRAdapter::QIODevicePVRAdapter(QIODevice *adaptee)
	: Stream("")
	, mAdaptee(adaptee)
	, mIsOpen(false)
{
	Q_ASSERT(nullptr != mAdaptee);
	Q_ASSERT(mAdaptee->isOpen());

	m_isReadable = mAdaptee->isReadable();
	m_isWritable = mAdaptee->isWritable();
	mOriginalPosition = mAdaptee->pos();
}

bool QIODevicePVRAdapter::read(
	size_t elementSize, size_t elementCount,
	void *buffer, size_t &dataRead) const
{
	return ioOperation(
		this, elementSize, elementCount,
		(char *) buffer, dataRead,
		&QIODevice::read);
}

bool QIODevicePVRAdapter::write(
	size_t elementSize, size_t elementCount,
	const void *buffer, size_t &dataWritten)
{
	return ioOperation(
		this, elementSize, elementCount,
		(const char *) buffer, dataWritten,
		&QIODevice::write);
}

bool QIODevicePVRAdapter::seek(long offset, SeekOrigin origin) const
{
	if (!mIsOpen || mAdaptee->isSequential())
		return false;

	switch (origin)
	{
		case SeekOriginFromStart:
			return mAdaptee->seek(offset);

		case SeekOriginFromCurrent:
			return mAdaptee->seek(mAdaptee->pos() + offset);

		case SeekOriginFromEnd:
			return mAdaptee->seek(mAdaptee->size() + offset);
	}

	return false;
}

bool QIODevicePVRAdapter::open() const
{
	if (mIsOpen)
		return true;

	if (mAdaptee->seek(mOriginalPosition))
	{
		mIsOpen = true;
		return true;
	}

	return false;
}

void QIODevicePVRAdapter::close()
{
	mIsOpen = false;
}

bool QIODevicePVRAdapter::isopen() const
{
	return mIsOpen;
}

size_t QIODevicePVRAdapter::getPosition() const
{
	return static_cast<size_t>(mAdaptee->pos());
}

size_t QIODevicePVRAdapter::getSize() const
{
	return static_cast<size_t>(mAdaptee->size());
}
