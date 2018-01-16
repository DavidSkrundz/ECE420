include Make.defaults
include Make.rules

.PHONY: all
all: $(EXEC)
	@

.PHONY: push
push:
	@rsync -arvz -e "ssh -p9005" --progress . user_05@162.246.157.207:~/`basename "$(CURDIR)"`

.PHONY: zip
zip: $(ZIP)
	@

.PHONY: clean
clean:
	@-rm -rf $(BUILDDIR)
	@-rm -rf $(DEPDIR)
	@-rm -f $(EXEC)
	@-rm -f $(ZIP)
	@$(MAKE) post-clean

include Make.lab
include $(wildcard $(patsubst $(SOURCEDIR)/%,$(DEPDIR)/%.d,$(basename $(SOURCES))))
