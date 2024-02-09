from calendar import c
import os
from datetime import date
from ssl import cert_time_to_seconds

#input file
fin = open("PlayBufferPCH.h", "rt")
#output file to write the result to
fout = open("Play.h", "wt")
#for each line in the input file
filelist = [ ]

print( "****************************************" )
print( "Compiling Single Header: Play.h")
print( "****************************************" )

today = date.today()



for line in fin:
	#read replace the string and write to output file

	if line.find("#include \"Play") != -1:
		header_filename = line[ line.find("\"")+1 : line.rfind("\"") ]
		print( "Including " + header_filename )
		filelist.append(header_filename)
		fin_h = open(header_filename, "rt");
		#fout.write("\n//********************  PLAY BEGIN: " + header_filename + "  ********************\n")
		doxygen_block_disable_switch = False
		for line_h in fin_h:			

			if line_h.find("#pragma once") != -1:
				continue
			
			if line_h.find("@ingroup") != -1:
				continue
				
			if line_h.find("@snippet") != -1:
				continue
				
			if line_h.find("[DOXYGEN_EXCLUDE]") != -1:
				doxygen_block_disable_switch = not doxygen_block_disable_switch
				continue
			
			if doxygen_block_disable_switch == True:
				continue
			
			fout.write(line_h)

		#fout.write("\n//********************  PLAY END: " + header_filename + "  ********************\n")
		fout.write("\n")
		fin_h.close()
	else :
		if line.find("#define PLAY_VERSION") != -1:
			fout.write( "#define PLAY_VERSION	\"2.0." + today.strftime("%y.%m.%d") + "\"\n" );
		else :
			if line.find("#pragma once") == -1:
				fout.write( line )
		#	fout.write("// Disable conversion warnings that haven't been fixed\n");
		#	fout.write("#pragma warning ( disable : 4244 )\n");

fin.close()

fout.write ("\n//*******************************************************************\n");
fout.write ("//*******************************************************************\n");
fout.write("#ifdef PLAY_IMPLEMENTATION\n" )
fout.write ("//*******************************************************************\n");
fout.write ("//*******************************************************************\n");

for header_filename in filelist:
	source_filename = header_filename.replace( ".h", ".cpp" )
	if os.path.isfile( source_filename ) :
		print( "Including " + source_filename )
		fin_cpp = open(source_filename, "rt");
		#fout.write("\n//********************  PLAY BEGIN: " + source_filename + "  ********************\n")
		for line_cpp in fin_cpp:
			if line_cpp.find("#include") == -1:
					fout.write(line_cpp)
		#fout.write("\n//********************  PLAY END: " + source_filename + "  ********************\n")
		fout.write("\n")

#close input and output files

fout.write("#endif // PLAY_IMPLEMENTATION\n" )

fout.write("\n#ifdef PLAY_IMPLEMENTATION\n" )
fout.write("#undef PLAY_IMPLEMENTATION // try to prevent multiple implementations\n" )
fout.write("#endif\n" )

print( "Done" )
print( "****************************************" )

fout.close()
