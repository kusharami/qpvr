TEMPLATE = subdirs
SUBDIRS = \
    PVRCore \
    PVRAssets \
    QZStream \
    plugin

plugin.depends = PVRCore PVRAssets QZStream
QZStream.file = thirdparty/qzstream/lib/QZStream.pro
