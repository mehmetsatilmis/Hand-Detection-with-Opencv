


all:
	g++ -o 101044011_HW02 main.cpp Attribute.cpp AttributeManager.cpp Tracking.cpp  `pkg-config --cflags --libs opencv` 

