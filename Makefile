#######################################
#		VARIABLES TO COMPILATEUR 	  #
#######################################
CC=gcc
CFLAGS=-W -Wall -std=c99
LDFLAGS=

#######################################
#	    VARIABLES TO CONSTRUCT        #
#######################################

EXEC= ttar
SRC_DIR= ./src
INCLUDES_DIR= ./includes
TESTS_DIR= ./tests
OBJECTS_DIR=./objets

#######################################
#	 	TO CONSTRUCT EXECUTABLE		  #
#######################################

all:$(EXEC)

ttar: $(OBJECTS_DIR)/main.o $(OBJECTS_DIR)/options/options.o 
	$(CC) $^ -o $@ $(LDFLAGS)

#######################################
#		TO CONSTRUCT OBJECTS		  #
#######################################

$(OBJECTS_DIR)/options/options.o: $(SRC_DIR)/options/options.c $(INCLUDES_DIR)/options/options.h
	$(CC) -c $< $(CFLAGS) -o $(OBJECTS_DIR)/options/options.o

$(OBJECTS_DIR)/tests/test_options.o: $(OBJECTS_DIR)/options/options.o $(TESTS_DIR)/options/main_options.c
	$(CC) -c $< $(CFLAGS) -o $(OBJECTS_DIR)/tests/test_options.o

#######################################
#		TO CONSTRUCT TESTS			  #
#######################################

test_options: $(OBJECTS_DIR)/tests/test_options.o
	$(CC) $^ -o $@ $(LDFLAGS)

#######################################
#				TO CLEAN			  #
#######################################

clean_exec:
	rm -rf $(EXEC) 

clean_tests:
	rm -rf $(TESTS_DIR)/*

mr_proper:
	rm -rf $(EXEC)
	rm -rf $(OBJECTS_DIR)/*

clean_all:
	rm -rf $(EXEC)
	rm -rf $(TESTS_DIR)/*
	rm -rf $(OBJECTS_DIR)/*