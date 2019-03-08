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

C_FILE=				billing_init.c\
					function.c\
					business.c\
					business_InitBase.c\
					business_ChangeBase.c\
					business_InitRelation.c\
					business_ChangeRelation.c\
					business_InitFav.c\
					ErrLog.c\
					CookieFunc.c\
					BaseFile.c\
					FavFile.c\
					IniFile.c\
					RelationFile.c

OBJ_FILE=			billing_init.o\
					function.o\
					business.o\
					business_InitBase.o\
					business_ChangeBase.o\
					business_InitRelation.o\
					business_ChangeRelation.o\
					business_InitFav.o\
					database.o\
					business_db.o\
					ErrLog.o\
					CookieFunc.o\
					BaseFile.o\
					FavFile.o\
					IniFile.o\
					RelationFile.o

INCLUDE_FILE=		mydefine.h\
					billing_init.h\
					business.h\
					function.h\
					database.h\
					business_db.h\
					ErrLog.h\
					CookieFunc.h\
					BaseFile.h\
					FavFile.h\
					IniFile.h\
					RelationFile.h

SWAP_FILE=			database.c\
					business_db.c

EXE_FILE=			billing_init

INCLUDES=			-I.\
					-I$(ORACLE_H_PATH)

LIBS=				-L.\
					-L$(ORACLE_LIB_PATH)\
					-lclntsh

all : $(EXE_FILE)
	cp $(EXE_FILE) init_base
	cp $(EXE_FILE) change_base
	cp $(EXE_FILE) init_fav
	
billing_init : $(OBJ_FILE) $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -o $@ $(OBJ_FILE) $(LIBS)	

billing_init.o : billing_init.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c billing_init.c $(INCLUDES)

business.o : business.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c business.c $(INCLUDES)

business_InitBase.o : business_InitBase.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c business_InitBase.c $(INCLUDES)
	
business_ChangeBase.o : business_ChangeBase.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c business_ChangeBase.c $(INCLUDES)
	
business_InitRelation.o : business_InitRelation.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c business_InitRelation.c $(INCLUDES)
	
business_ChangeRelation.o : business_ChangeRelation.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c business_ChangeRelation.c $(INCLUDES)			
	
business_InitFav.o : business_InitFav.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c business_InitFav.c $(INCLUDES)				

function.o : function.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c function.c $(INCLUDES)

database.o : database.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c database.c $(INCLUDES)

ErrLog.o : ErrLog.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c ErrLog.c $(INCLUDES)

CookieFunc.o : CookieFunc.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c CookieFunc.c $(INCLUDES)

database.c : database.pc $(INCLUDE_FILE)
	proc database.pc $(PROC_FLAG)

business_db.c : business_db.pc $(INCLUDE_FILE)
	proc business_db.pc $(PROC_FLAG)
	
business_db.o : business_db.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c business_db.c $(INCLUDES)

BaseFile.o : BaseFile.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c BaseFile.c $(INCLUDES)	

FavFile.o : FavFile.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c FavFile.c $(INCLUDES)		

RelationFile.o : RelationFile.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c RelationFile.c $(INCLUDES)		

IniFile.o : IniFile.c $(INCLUDE_FILE)
	$(CC) $(C_FLAG) -c IniFile.c $(INCLUDES)		
	
clean:
	rm $(EXE_FILE) $(OBJ_FILE) $(SWAP_FILE) 2>/dev/null
	rm init_base change_base init_fav
