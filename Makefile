SUBDIRS := Simulator GameManager Algorithm

all:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

log:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir log; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
