TEMPLATE = subdirs
SUBDIRS = \
    PVRCore \
    PVRAssets \
    QZStream \
    plugin \
    qpvr_tests

plugin.depends = PVRCore PVRAssets QZStream
qpvr_tests.depends = plugin
QZStream.file = thirdparty/qzstream/lib/QZStream.pro
