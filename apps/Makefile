load $(shell pkg-config --variable=libdir dwmgmk)/dwm_gmk.so(dwm_gmk_setup)

appsDir := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

$(dwm_include $(abspath $(appsDir)/dwmcflags/Makefile))
$(dwm_include $(abspath $(appsDir)/dwmnet/Makefile))
$(dwm_include $(abspath $(appsDir)/dwmspath/Makefile))
$(dwm_include $(abspath $(appsDir)/dwmwhat/Makefile))
