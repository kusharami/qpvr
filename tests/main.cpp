#include "Tests.h"

#include <QDebug>
#include <QtTest>

int main(int argc, char *argv[])
{
	qDebug() << "Init tests...";

	QTEST_SET_MAIN_SOURCE_PATH

	Tests tests;

	return QTest::qExec(&tests, argc, argv);
}
