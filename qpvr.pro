TEMPLATE = subdirs
SUBDIRS = \
    PVRCore \
    PVRAssets \
    QZStream \
    plugin \
    tests

plugin.depends = PVRCore PVRAssets QZStream
tests.depends = plugin
QZStream.file = thirdparty/qzstream/lib/QZStream.pro
