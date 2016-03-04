NAME = dark_frame_sub
PROGRAM = $(NAME).exe
CXXFLAGS= -march=i486

LDFLAGS=-lopencv_core.dll -lopencv_highgui.dll -lopencv_imgcodecs.dll -lopencv_imgproc.dll -lboost_filesystem-mt -lboost_system-mt -lboost_serialization-mt

OBJECTS=$(NAME).o

.SUFFIXES: .cpp .o

$(PROGRAM): $(OBJECTS)
	$(CXX)  $^  -o $(PROGRAM) $(LDFLAGS) 

.c.o:
	$(CXX) -c $(CXXFLAGS) $<

.PHONY:clean
clean:
	rm -f $(NAME).exe  *.o
