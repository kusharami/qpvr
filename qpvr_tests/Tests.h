#pragma once

#include <QObject>

class QDir;

class PVRTests : public QObject
{
	Q_OBJECT

private slots:
	void testInstallation();
	void testIO_data();
	void testIO();

private:
	struct Options;

	void testWrite(const Options &options, const QDir &dir, bool compressed);
	void testRead(const Options &options, const QDir &dir, bool compressed);

	static QByteArray getSubType(int ver, const QByteArray &str);
	static QString filePathForSubType(
		const QDir &dir, const QByteArray &subType, bool compressed);
};
