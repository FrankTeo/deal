ORACLE_LIB_PATH=	$(ORACLE_HOME)/lib
ORACLE_H_PATH=		$(ORACLE_HOME)/precomp/public
CC=cc
C_FLAG=				-q64

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

C_FILE=				billing_sync.c\
					function.c\
					business.c\
					ErrLog.c\
					CookieFunc.c\
					database.c\
					business_db.c

OBJ_FILE=			billing_sync.o\
					function.o\
					business.o\
					database.o\
					business_db.o\
					ErrLog.o\
					CookieFunc.o

INCLUDE_FILE=		mydefine.h\
					billing_sync.h\
					business.h\
					function.h\
					database.h\
					business_db.h\
					ErrLog.h\
					CookieFunc.h

SWAP_FILE=			database.c\
					business_db.c

EXE_FILE=			change_imsi change_rent 

INCLUDES=			-I.\
					-I$(ORACLE_H_PATH)

LIBS=				-L.\
					-L$(ORACLE_LIB_PATH)\
					-lclntsh

all : $(EXE_FILE)
	
change_imsi : $(OBJ_FILE) $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -DAPP_TRADECODE=1 -c business.c $(INCLUDES)
	$(CC) $(C_FLAG) -o $@ $(OBJ_FILE) $(LIBS)	

change_rent : $(OBJ_FILE) $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -DAPP_TRADECODE=2 -c business.c $(INCLUDES)
	$(CC) $(C_FLAG) -o $@ $(OBJ_FILE) $(LIBS)		

billing_sync.o : billing_sync.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c billing_sync.c $(INCLUDES)

business.o : business.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -DAPP_TRADECODE=0 -c business.c $(INCLUDES)	
	
business_InitFav.o : business_InitFav.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c business_InitFav.c $(INCLUDES)				

function.o : function.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c function.c $(INCLUDES)

ErrLog.o : ErrLog.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c ErrLog.c $(INCLUDES)

CookieFunc.o : CookieFunc.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c CookieFunc.c $(INCLUDES)

database.c : database.pc $(INCLUDE_FILE)
	proc database.pc $(PROC_FLAG)

database.o : database.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c database.c $(INCLUDES)	

business_db.c : business_db.pc $(INCLUDE_FILE)
	proc business_db.pc $(PROC_FLAG)
	
business_db.o : business_db.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c business_db.c $(INCLUDES)
	
clean:
	rm $(EXE_FILE) $(OBJ_FILE) $(SWAP_FILE)
