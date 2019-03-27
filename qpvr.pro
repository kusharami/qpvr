TEMPLATE = subdirs
SUBDIRS = \
    PVRCore \
    PVRAssets \
    plugin \
    qpvr_tests

plugin.depends = PVRCore PVRAssets
qpvr_tests.depends = plugin
