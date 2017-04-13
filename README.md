
README.md
=========

Shared Libraries : 
-------------------
libDAQ/
Directory containing the files that generates the library used to convert the data file from ANU experiment to basic root tree.
To creat the library : 
	> cd libDAQ/
	> make distclean
	> make

libAnalysis/
Directory containing the files that generates the library used to generate a root tree file from a basic root tree 
to another more compact and useful root tree to replay data with selection of segments, and live graphical cuts.
To creat the library : 
	> cd libAnalysis/
	> make distclean
	> make

Files description : 
-------------------
dcp2root.cxx : code that converts ANU data to basic root tree  (compile and execute as described in the first comment on the top of dcp2root.cxx )
convert      : dcp2root.cxx  executable { converts "XXXX.RUNYYY" to "XXXX.RUNYYY.root" }

struct2class.cxx  : code that convertsbasic root tree to a more compact (vectorial) root tree (compile and execute as described in the first comment on the top of struct2class.cxx )  
struct2class      : struct2class.cxx  executable { converts "XXXX.RUNYYY.root" to "Data_XXXX.RUNYYY.root" }
                    e.g. > ./struct2class  FE54_0713.RUN004.root
                    
                   
