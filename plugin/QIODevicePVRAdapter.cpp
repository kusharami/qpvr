﻿#include "QIODevicePVRAdapter.h"

#include <QIODevice>

#include <QDebug>

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

QIODevicePVRAdapter::QIODevicePVRAdapter(QIODevice *adaptee)
	: Stream("")
	, mAdaptee(adaptee)
	, mOriginalPosition(0)
	, mIsOpen(false)
	, mTransactionStarted(false)
{
	Q_ASSERT(nullptr != mAdaptee);
	Q_ASSERT(mAdaptee->isOpen());

	m_isReadable = mAdaptee->isReadable();
	m_isWritable = mAdaptee->isWritable();
}

QIODevicePVRAdapter::~QIODevicePVRAdapter()
{
	close();
}

bool QIODevicePVRAdapter::read(
	size_t elementSize, size_t elementCount,
	void *buffer, size_t &elementsRead) const
{
	if (!isReadable())
		return false;

	return ioOperation(
		elementSize, elementCount,
		(char *) buffer, elementsRead,
		&QIODevice::read);
}

bool QIODevicePVRAdapter::write(
	size_t elementSize, size_t elementCount,
	const void *buffer, size_t &elementsWritten)
{
	if (!isWritable())
		return false;

	return ioOperation(
		elementSize, elementCount,
		(char *) buffer, elementsWritten,
		(Operation) static_cast<Write>(&QIODevice::write));
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

bool QIODevicePVRAdapter::ioOperation(
	size_t elementSize, size_t elementCount,
	char *buffer, size_t &resultCount,
	Operation op) const
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