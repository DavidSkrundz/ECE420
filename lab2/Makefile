include Make.defaults
include Make.rules

.PHONY: all
all: $(CLIENT) $(SERVER)
	@

.PHONY: push
push: clean
	@rsync -arvz -e "ssh -p9032" --progress . user_32@162.246.157.207:~/`basename "$(CURDIR)"`

.PHONY: push
push2: clean
	@rsync -arvz --progress . dskrundz@e5-05-10.ece.ualberta.ca:~/`basename "$(CURDIR)"`

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
include $(wildcard $(patsubst $(SOURCEDIR)/%,$(DEPDIR)/%.d,$(basename $(CLIENT_SOURCES))))
include $(wildcard $(patsubst $(SOURCEDIR)/%,$(DEPDIR)/%.d,$(basename $(SERVER_SOURCES))))
