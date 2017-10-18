#include "QIODevicePVRAdapter.h"

#include <QIODevice>

#include <QDebug>

using namespace pvr;

Q_DECL_CONSTEXPR static inline qint64 sizeToInt64(size_t size)
{
	return qint64(qMin(size,
		size_t((sizeof(size_t) < 8) ? qint64(std::numeric_limits<size_t>::max())
									: std::numeric_limits<qint64>::max())));
}

QIODevicePVRAdapter::QIODevicePVRAdapter(QIODevice *adaptee)
	: Stream(std::string())
	, mAdaptee(adaptee)
	, mOriginalPosition(0)
	, mIsOpen(false)
	, mTransactionStarted(false)
{
	Q_ASSERT(nullptr != mAdaptee);
	Q_ASSERT(mAdaptee->isOpen());

	_isReadable = mAdaptee->isReadable();
	_isWritable = mAdaptee->isWritable();
}

QIODevicePVRAdapter::~QIODevicePVRAdapter()
{
	close();
}

bool QIODevicePVRAdapter::read(size_t elementSize, size_t elementCount,
	void *buffer, size_t &elementsRead) const
{
	if (!isReadable())
		return false;

	return ioOperation(elementSize, elementCount, buffer, elementsRead,
		reinterpret_cast<IOOperation>(static_cast<Read>(&QIODevice::read)));
}

bool QIODevicePVRAdapter::write(size_t elementSize, size_t elementCount,
	const void *buffer, size_t &elementsWritten)
{
	if (!isWritable())
		return false;

	return ioOperation(elementSize, elementCount, const_cast<void *>(buffer),
		elementsWritten,
		reinterpret_cast<IOOperation>(static_cast<Write>(&QIODevice::write)));
}

bool QIODevicePVRAdapter::seek(long offset, SeekOrigin origin) const
{
	if (!mIsOpen || mAdaptee->isSequential())
		return false;

	switch (origin)
	{
		case SeekOriginFromStart:
			return mAdaptee->seek(offset + mOriginalPosition);

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

	Q_ASSERT(mAdaptee->isOpen());

	mIsOpen = true;

	if (!mAdaptee->isTransactionStarted())
	{
		mTransactionStarted = true;
		mAdaptee->startTransaction();
	}

	mOriginalPosition = mAdaptee->pos();
	return true;
}

void QIODevicePVRAdapter::close()
{
	if (mIsOpen)
	{
		mIsOpen = false;

		if (mTransactionStarted)
		{
			mTransactionStarted = false;
			Q_ASSERT(mAdaptee->isTransactionStarted());
			mAdaptee->rollbackTransaction();
		}
	}
}

bool QIODevicePVRAdapter::isopen() const
{
	return mIsOpen;
}

size_t QIODevicePVRAdapter::getPosition() const
{
	return static_cast<size_t>(mAdaptee->pos() - mOriginalPosition);
}

size_t QIODevicePVRAdapter::getSize() const
{
	return static_cast<size_t>(mAdaptee->size() - mOriginalPosition);
}

bool QIODevicePVRAdapter::ioOperation(size_t elementSize, size_t elementCount,
	void *buffer, size_t &resultCount, IOOperation op) const
{
	if (!isopen())
		return false;

	auto requestedSize = elementSize * elementCount;
	auto size = sizeToInt64(requestedSize);
	resultCount = static_cast<size_t>((mAdaptee->*op)(buffer, size));
	bool result = resultCount == requestedSize ||
		(!mAdaptee->isSequential() && mAdaptee->atEnd());

	resultCount /= elementSize;
	return result;
}
