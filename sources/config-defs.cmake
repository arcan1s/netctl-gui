# default configuration options

# [Common] section
set (CFG_LANGUAGE "en" CACHE STRING "LANGUAGE key")
set (CFG_MAINUPDATE "0" CACHE STRING "MAINUPDATE key")
set (CFG_WIFIUPDATE "0" CACHE STRING "WIFIUPDATE key")
set (CFG_SYSTRAY "true" CACHE STRING "SYSTRAY key")
set (CFG_CLOSETOTRAY "true" CACHE STRING "CLOSETOTRAY key")
set (CFG_STARTTOTRAY "false" CACHE STRING "STARTTOTRAY key")
set (CFG_SKIPCOMPONENTS "false" CACHE STRING "SKIPCOMPONENTS key")

# [Helper] section
set (CFG_USE_HELPER "true" CACHE STRING "USE_HELPER key")
set (CFG_FORCE_SUDO "false" CACHE STRING "FORCE_SUDO key")
set (CFG_CLOSE_HELPER "false" CACHE STRING "CLOSE_HELPER key")
set (CFG_HELPER_PATH "netctlgui-helper" CACHE STRING "HELPER_PATH key")
set (CFG_HELPER_SERVICE "netctlgui-helper.service" CACHE STRING "HELPER_SERVICE key")

# [netctl] section
set (CFG_SYSTEMCTL_PATH "systemctl" CACHE STRING "SYSTEMCTL_PATH key")
set (CFG_NETCTL_PATH "netctl" CACHE STRING "NETCTL_PATH key")
set (CFG_NETCTLAUTO_PATH "netctl-auto" CACHE STRING "NETCTLAUTO_PATH key")
set (CFG_NETCTLAUTO_SERVICE "netctl-auto" CACHE STRING "NETCTLAUTO_SERVICE key")
set (CFG_PROFILE_DIR "/etc/netctl/" CACHE STRING "PROFILE_DIR key")

# [sudo] section
set (CFG_SUDO_PATH "kdesu" CACHE STRING "SUDO_PATH key")

# [wpa_supplicant] section
set (CFG_WPASUP_PATH "wpa_supplicant" CACHE STRING "WPASUP_PATH key")
set (CFG_WPACLI_PATH "wpa_cli" CACHE STRING "WPACLI_PATH key")
set (CFG_PID_FILE "/run/wpa_supplicant_$i.pid" CACHE STRING "PID_FILE key")
set (CFG_WPA_DRIVERS "nl80211,wext" CACHE STRING "WPA_DRIVERS key")
set (CFG_CTRL_DIR "/run/wpa_supplicant" CACHE STRING "CTRL_DIR key")
set (CFG_CTRL_GROUP "users" CACHE STRING "CTRL_GROUP key")

# [Other] section
set (CFG_EDITOR_PATH "gvim" CACHE STRING "EDITOR_PATH key")
set (CFG_IFACE_DIR "/sys/class/net/" CACHE STRING "IFACE_DIR key")
set (CFG_RFKILL_DIR "/sys/class/rfkill/" CACHE STRING "RFKILL_DIR key")
set (CFG_PREFERED_IFACE "" CACHE STRING "PREFERED_IFACE key")
