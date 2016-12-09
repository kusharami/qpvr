#pragma once

#include <QIODevice>

class CCZStream : public QIODevice
{
	Q_OBJECT

public:
	CCZStream(QIODevice *source);
	explicit CCZStream(QIODevice *source, QObject *parent = nullptr);

	virtual bool isSequential() const override;

	virtual bool open(OpenMode mode) override;
	virtual void close() override;

	virtual qint64 size() const override;
	virtual bool seek(qint64 pos) override;

	virtual qint64 bytesAvailable() const override;
	virtual qint64 bytesToWrite() const override;

	virtual bool canReadLine() const override;

	virtual bool waitForReadyRead(int msecs) override;
	virtual bool waitForBytesWritten(int msecs) override;

protected:
	virtual qint64 readData(char *data, qint64 maxlen) override;
	virtual qint64 readLineData(char *data, qint64 maxlen) override;
	virtual qint64 writeData(const char *data, qint64 len) override;

private:
	QIODevice *source;
	qint64 sourcePosition = 0;
	qint64 uncompressedSize = 0;
};
