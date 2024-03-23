PYTHON ?= python
PYTHON2 ?= python2
INET_DIR = extern/inet4.5
SIMU5G_DIR = extern/Simu5G
VANETZA_DIR = extern/vanetza
VANETZA_BUILD_TYPE ?= Release
VANETZA_BUILD_DIR ?= $(VANETZA_DIR)/build
VEINS_DIR = extern/veins

all: inet vanetza veins simu5g
clean:
	-$(MAKE) -C $(INET_DIR) cleanall
	-$(MAKE) -C $(VEINS_DIR) cleanall
	-rm -rf $(VANETZA_BUILD_DIR)
	-$(MAKE) -C $(SIMU5G_DIR) cleanall
	

inet: $(INET_DIR)/src/Makefile
	$(MAKE) -C $(INET_DIR)/src

$(SIMU5G_DIR)/src/Makefile: $(SIMU5G_DIR)/Version
	$(MAKE) -C $(SIMU5G_DIR) makefiles INET_PROJ=$(INET_DIR)
	$(MAKE) -C $(SIMU5G_DIR)/src depend

simu5g: $(SIMU5G_DIR)/src/Makefile
	$(MAKE) -C $(SIMU5G_DIR)/src

$(VEINS_DIR)/src/Makefile: $(VEINS_DIR)/configure
	cd $(VEINS_DIR); $(PYTHON2) configure
	$(MAKE) -C $(VEINS_DIR)/src depend

veins: $(VEINS_DIR)/src/Makefile
	$(MAKE) -C $(VEINS_DIR)
	
$(VANETZA_BUILD_DIR):
	mkdir -p $(VANETZA_BUILD_DIR)

$(VANETZA_BUILD_DIR)/Makefile: $(VANETZA_BUILD_DIR)
	cd $<; cmake -DCMAKE_BUILD_TYPE=$(VANETZA_BUILD_TYPE) -DBUILD_SHARED_LIBS=ON ..

vanetza: $(VANETZA_BUILD_DIR)/Makefile
	$(MAKE) -C $(VANETZA_BUILD_DIR)


