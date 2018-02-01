include Make.defaults
include Make.rules

.PHONY: all
all: $(CLIENT) $(SERVER)
	@

.PHONY: push
push:
	@rsync -arvz -e "ssh -p9032" --progress . user_32@162.246.157.207:~/`basename "$(CURDIR)"`

.PHONY: zip
zip: $(ZIP)
	@

.PHONY: clean
clean:
	@-rm -rf $(BUILDDIR)
	@-rm -rf $(DEPDIR)
	@-rm -f $(CLIENT)
	@-rm -f $(SERVER)
	@-rm -f $(ZIP)

include Make.lab
include $(wildcard $(patsubst $(SOURCEDIR)/%,$(DEPDIR)/%.d,$(basename $(SOURCES))))
