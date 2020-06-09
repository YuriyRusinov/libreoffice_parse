cg.input = CG_INPUT
cg.output = $$DESTDIR/generated_files/exception.cpp
cg.variable_out = HEADERS
CG_INPUT = $$DESTDIR/generated_files/exception.cpp

# Specify custom command output file:
custom_generator.output  = $$DESTDIR/generated_files/exception.cpp
includeExc = ' ' include
includeException = $$join(includeExc, $$LITERAL_HASH)
includeException += "<com/sun/star/uno/Exception.hpp>"
message("Include exception is $${includeException}")

custom_generator.commands = 'echo "Generating..."; mkdir -p $$DESTDIR/generated_files; echo "$${includeException}" > $$DESTDIR/generated_files/exception.cpp; $${OOO_SDK_PROGRAM} $${OOO_SDK_DIR}/types.rdb $${OOO_SDK_DIR}/types/oovbaapi.rdb $${OOO_SDK_DIR}/types/offapi.rdb -O $${DESTDIR}/generated_files; echo "Generated..."'
# dependency:
custom_generator.depends = FORCE
# link to input file variable
custom_generator.input = CG_INPUT
custom_generator.output = $$DESTDIR/generated_files/exception.cpp
# link to variable to store generated file to
custom_generator.variable_out = SOURCES
custom_generator.config = target_predeps
#SOURCES
# add to qmake:
QMAKE_EXTRA_COMPILERS += custom_generator
HEADERS += $$DESTDIR/generated_files/com/sun/star/uno/Exception.hpp

DISTFILES += CG_INPUT

