CC = g++-6
LANG_VER = std=c++1z
DIAG_FLAGS = -Wall -Werror -Wpedantic

%: $@.cpp
	$(CC) $(LANG_VER) $(DIAG_FLAGS) -o