ORACLE_LIB_PATH=	$(ORACLE_HOME)/lib
ORACLE_H_PATH=		$(ORACLE_HOME)/precomp/public
CC=					cc
C_FLAG=				-q64 -DDEBUG
PROC_FLAG=			include=.\
					include=/usr/include\
					include=$(ORACLE_H_PATH)\
					def_sqlcode=true\
					lines=yes\
					sqlcheck=syntax\
					code=ansi_c\
					lname=/dev/null\
					mode=oracle

PROC_FILE=			database.pc\
					business_db.pc

SWAP_FILE=			database.c\
					business_db.c


OTHER_FILE=			billing_qfav.cfg

C_FILE=				BaseFile.c\
					CookieFunc.c\
					ErrLog.c\
					FavFile.c\
					IniFile.c\
					billing_qfav.c\
					business.c\
					function.c\
					network.c\
					billing_qfav.client.c
					

OBJ_FILE=			BaseFile.o\
					CookieFunc.o\
					ErrLog.o\
					FavFile.o\
					IniFile.o\
					billing_qfav.o\
					business.o\
					function.o\
					network.o\
					business_db.o\
					database.o

INCLUDE_FILE=		network.h\
					function.h\
					mydefine.h\
					database.h\
					business_db.h\
					business.h\
					billing_qfav.h\
					FavFile.h\
					BaseFile.h\
					CookieFunc.h\
					ErrLog.h\
					IniFile.h

EXE_FILE=			billing_qfav billing_qfav.client

INCLUDES=			-I.\
					-I$(ORACLE_H_PATH)

LIBS=				-L.\
					-L$(ORACLE_LIB_PATH)\
					`cat $(ORACLE_HOME)/lib/ldflags` `cat $(ORACLE_HOME)/lib/sysliblist`\
					-lclntsh

all : $(EXE_FILE)

billing_qfav : $(OBJ_FILE) $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -o $@ $(OBJ_FILE) $(LIBS)

billing_qfav.client : billing_qfav.client.c
	cc $(C_FLAG) -o $@ $?

BaseFile.o : BaseFile.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c BaseFile.c $(INCLUDES)
	
CookieFunc.o : CookieFunc.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c CookieFunc.c $(INCLUDES)
	
ErrLog.o : ErrLog.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c ErrLog.c $(INCLUDES)
	
FavFile.o : FavFile.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c FavFile.c $(INCLUDES)

IniFile.o : IniFile.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c IniFile.c $(INCLUDES)
	
billing_qfav.o : billing_qfav.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c billing_qfav.c $(INCLUDES)
	
business.o : business.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c business.c $(INCLUDES)

function.o : function.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c function.c $(INCLUDES)
	
network.o : network.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c network.c $(INCLUDES)

database.c : database.pc $(INCLUDE_FILE)
	proc database.pc $(PROC_FLAG)

business_db.c : business_db.pc $(INCLUDE_FILE)
	proc business_db.pc $(PROC_FLAG)

database.o : database.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c database.c $(INCLUDES)
	
business_db.o : business_db.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c business_db.c $(INCLUDES)

clean:
	rm $(EXE_FILE) $(OBJ_FILE) $(SWAP_FILE)
