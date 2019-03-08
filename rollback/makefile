ORACLE_LIB_PATH=	$(ORACLE_HOME)/lib
ORACLE_H_PATH=		$(ORACLE_HOME)/precomp/public
CC=					cc
#C_FLAG=			-xarch=native64
C_FLAG=				-q64

C_FILE=				CookieFunc.c\
					ErrLog.c\
					FavFile.c\
					IniFile.c\
					billing_roll.c\
					business.c\
					function.c
					
OBJ_FILE=			CookieFunc.o\
					ErrLog.o\
					FavFile.o\
					IniFile.o\
					billing_roll.o\
					business.o\
					function.o
					
INCLUDE_FILE=		function.h\
					mydefine.h\
					business.h\
					billing_roll.h\
					FavFile.h\
					CookieFunc.h\
					ErrLog.h\
					IniFile.h

EXE_FILE=			billing_roll

INCLUDES=			-I. -I$(ORACLE_H_PATH)

LIBS=				-L. -L$(ORACLE_LIB_PATH) -lclntsh


all : $(EXE_FILE)

$(EXE_FILE) : $(OBJ_FILE) $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -o $@ $(OBJ_FILE) $(LIBS)

CookieFunc.o : CookieFunc.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c CookieFunc.c $(INCLUDES)
	
ErrLog.o : ErrLog.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c ErrLog.c $(INCLUDES)
	
FavFile.o : FavFile.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c FavFile.c $(INCLUDES)

IniFile.o : IniFile.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c IniFile.c $(INCLUDES)
	
billing_roll.o : billing_roll.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c billing_roll.c $(INCLUDES)
	
business.o : business.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c business.c $(INCLUDES)

function.o : function.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c function.c $(INCLUDES)

clean:
	rm $(EXE_FILE) $(OBJ_FILE) $(SWAP_FILE)
