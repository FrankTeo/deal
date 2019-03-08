ORACLE_LIB_PATH=	$(ORACLE_HOME)/lib
ORACLE_H_PATH=		$(ORACLE_HOME)/precomp/public
CC=cc
#C_FLAG=				-g -q64 -DDEBUG -DDEBUG_MEMLEAK
C_FLAG=			-O3	-q64

PROC_FLAG=			include=.\
					include=$(ORACLE_H_PATH)\
					def_sqlcode=true\
					lines=yes\
					sqlcheck=syntax\
					code=ansi_c\
					lname=/dev/null\
					mode=oracle

OBJ_FILE=			deal_main.o\
					deal_utility.o\
					deal_communal_data.o\
					deal_public_function.o\
					deal_function.o\
					deal_user_info.o\
					deal_cdr_function.o\
					deal_count_function.o\
					deal_vc.o\
					deal_forw.o\
					deal_gprs.o\

INCLUDE_FILE=		deal_cdr.h\
					deal_config.h\
					deal_user_info.h\
					deal_utility.h

SWAP_FILE=			deal_communal_data.c\

EXE_FILE=			roam_billing\

INCLUDES=			-I.\
					-I$(ORACLE_H_PATH)

LIBS=				-L.\
					-L$(ORACLE_LIB_PATH)\
					-lclntsh

all : $(EXE_FILE)

$(EXE_FILE):$(OBJ_FILE) $(INCLUDE_FILE)
	$(CC) -o $(EXE_FILE) *.o $(C_FLAG) $(LIBS)

deal_communal_data.c:deal_communal_data.pc
	proc $(PROC_FLAG) deal_communal_data.pc

.c.o:
	$(CC) -c $< $(C_FLAG) $(INCLUDES)

clean :
	rm -f $(EXE_FILE) $(OBJ_FILE) $(SWAP_FILE)
