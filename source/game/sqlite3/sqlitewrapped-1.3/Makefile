VERSION =	1.3

INSTALL_PREFIX = /usr/devel
INSTALL_LIB =	$(INSTALL_PREFIX)/lib
INSTALL_INCLUDE = $(INSTALL_PREFIX)/include
INSTALL =	/usr/bin/install

INCLUDE =	-I/usr/devel/include -I.
CFLAGS =	-Wall -g -O2 $(INCLUDE) -MD
# namespace
#CFLAGS +=	-DSQLITEW_NAMESPACE=sqlitew
CPPFLAGS =	$(CFLAGS)

LIBS =		-L/usr/devel/lib -L/usr/local/lib -lsqlitewrapped -lsqlite3
LIBNAME =	libsqlitewrapped

PROGS =		

all:		$(PROGS) $(LIBNAME).a $(LIBNAME).h

		
LIBM =		Database.o Query.o StderrLog.o SysLog.o 
$(LIBNAME).a: 	$(LIBM)
		ar cru $(LIBNAME).a $(LIBM)
		ranlib $(LIBNAME).a

$(LIBNAME).h:	IError.h StderrLog.h SysLog.h Database.h Query.h 
		cat IError.h StderrLog.h SysLog.h Database.h Query.h > $(LIBNAME).h

install:	all
		@mkdir -p $(INSTALL_LIB)
		@mkdir -p $(INSTALL_INCLUDE)
		$(INSTALL) $(LIBNAME).a $(INSTALL_LIB)
		$(INSTALL) $(LIBNAME).h $(INSTALL_INCLUDE)

clean:
		rm -f *~ *.o $(PROGS) *.d slask *.a $(LIBNAME).h

sqlite3test:	sqlite3test.o
		g++ -o $@ $^ $(LIBS)

-include	*.d

TARFILE =	sqlitewrapped-$(VERSION)
tar:		clean
		tar czf $(TARFILE).tar.gz \
			Query.* \
			Database.* \
			IError.h \
			StderrLog.* \
			SysLog.* \
			sqlite3test.cpp \
			Makefile gpl.txt \
			Project/*.dsp Project/*.dsw
		/usr/local/bin/tarfix.sh $(TARFILE)
		cp $(TARFILE).tar.gz /usr/local/apache/www.alhem.net/htdocs/project/sqlite
		cp $(TARFILE).zip /usr/local/apache/www.alhem.net/htdocs/project/sqlite

docs:
		./mkdot.sh
