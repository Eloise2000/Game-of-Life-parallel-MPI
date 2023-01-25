RLES = beehive.rle glider.rle grower.rle
HEADERS = $(RLES:.rle=.h)
JUNK = $(TAR_FILE)
EXPENSIVE_JUNK = $(HEADERS)
TAR_FILES = $(HEADERS) $(RLES) convert_rle.py Makefile
TAR_FILE = game-of-life-assignment.tar.gz

all:  $(HEADERS)

tar: $(TAR_FILE)

$(TAR_FILE): $(TAR_FILES)
	tar cvfz $(TAR_FILE) $(TAR_FILES)

clean:
	rm -rf $(JUNK)

empty:
	rm -rf $(JUNK) $(EXPENSIVE_JUNK)

%.h: %.rle
	python3 convert_rle.py $<
