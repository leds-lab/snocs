VERSION = 1.0.0

# OS X specific settings
mac {
    ICON = ../resources/logo.icns
    FRAMEWORK_HEADERS.version = $$[VERSION]
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS
}

# Win32 specific settings
win32 {
    QMAKE_TARGET_COMPANY = "LEDS - Laboratory of Embedded and Distributed Systems"
    QMAKE_TARGET_PRODUCT = $$[TARGET]
    QMAKE_TARGET_DESCRIPTION = "SoCIN Network-on-Chip Simulator"
    QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2007-2017 LEDS - Univali"
    RC_ICONS = ../resources/logo.ico
}
